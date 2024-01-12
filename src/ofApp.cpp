#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(30);

	bDebugText = true;

	bLoading = false;
	bAnalysing = false;
	
	previousTime = ofGetElapsedTimef();
	deltaTime = 0.0;

	// Mesh //
	numPoints = 0;
	points.setMode(OF_PRIMITIVE_POINTS);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(2.0);

	bDrawPoints = true;

	// Point Picker //
	bPointPicker = false;
	nearestIndex = 0;
	nearestVertexScreenCoordinate = { 0,0 };
	nearestDistance = 0;

	// Camera //
	camera.setGlobalPosition({ 0,0,camera.getImagePlaneDistance(ofGetCurrentViewport()) });

	rotationSpeed = 45.0;
	for (int i = 0; i < 6; i++) {
		rotatePoints[i] = 0;
	}

	rmsAmplitudeScale = 400.0;
	spectralCentroidScale = 400.0;
	lengthScale = 50.0;
}

//--------------------------------------------------------------
void ofApp::update() {

	// Delta Time //
	float currentTime = ofGetElapsedTimef();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;

	// Loading/Analysing //
	if (bLoading)
	{
		updateWhileLoading();
		return;
	}
	else if (bAnalysing) {
		updateWhileAnalysing();
		return;
	}
	// Skip Below If Loading/Analysing ------------------------ //

	bool movement = rotatePoints[0] || rotatePoints[1] || rotatePoints[2] || rotatePoints[3] || rotatePoints[4] || rotatePoints[5];
	if (movement)
	{
		float deltaSpeed = rotationSpeed * deltaTime;
		meshRotation(deltaSpeed);
	}

	if (bPointPicker)
	{
		pointPicker();
	}

	soundController();
}

void ofApp::updateWhileLoading() {
	if (folders.size() > 0) {
		partialLoad(folders.back().getAbsolutePath());
	}
	else {
		bLoading = false;

		if (audioFiles.size() > 0) {
			bAnalysing = true;
		}
		else {
			ofSystemAlertDialog("No audio files found.");

			if (numPoints > 0)
				bPointPicker = true;
		}
	}
}

void ofApp::updateWhileAnalysing() {
	if (analysisIndex < audioFiles.size()) {
		partialAnalyse();
	}
	else {
		bAnalysing = false;

		numPoints = points.getNumVertices();
		pointOrigins = points;
		
		stringstream ss;
		ss << "Analysed " << audioFiles.size() << " audio files." << endl;
		ss << "Points: " << ofToString(numPoints) << endl;
		ss << "Failed to load " << failedAnalysisCount << " audio files." << endl;
		ss << "Analysis complete." << endl;
		ofSystemAlertDialog(ss.str().c_str());

		bPointPicker = true;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	// Reference Sphere //
	{
		camera.begin();
		ofSetColor(ofColor::darkSlateGray);
		ofNoFill();
		ofDrawSphere(0, 0, 0, 300);
		camera.end();
	}

	// Draw Points //
	if (bDrawPoints) {
		ofEnableDepthTest();
		camera.begin();
		ofSetColor(220);
		points.draw();
		camera.end();
		ofDisableDepthTest();
	}

	// Draw Nearest Point //
	if (!bLoading && bPointPicker && nearestDistance < 15) {
		ofFill();
		ofSetColor(ofColor::gray);
		ofDrawLine(nearestVertexScreenCoordinate, mouse);

		ofNoFill();
		ofSetColor(ofColor::yellow);
		ofSetLineWidth(2);
		ofDrawCircle(nearestVertexScreenCoordinate, 4);
		ofSetLineWidth(1);

		glm::vec2 offset(10, -10);
		ofDrawBitmapStringHighlight(ofToString(nearestIndex) + " - " + audioFiles[nearestIndex].getFileName(), mouse + offset);
	}

	// Draw Loading/Analysing Text //
	if (bLoading) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

		stringstream ss;
		ss << "Loading audio files..." << endl;
		ss << "Found " << audioFiles.size() << " files." << endl;
		ss << "Searched " << searchedFolders << " folders." << endl;
		ss << "Remaining " << folders.size() << " folders.";

		ofDrawBitmapString(ss.str().c_str(), ofGetWidth() - 200, 20);
	}
	else if (bAnalysing) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

		stringstream ss;
		ss << "Analysing audio files..." << endl;
		ss << "Analysed " << analysisIndex << "/" << audioFiles.size() << endl;
		ss << "Failed to load " << failedAnalysisCount << " files." << endl;

		ofDrawBitmapString(ss.str().c_str(), ofGetWidth() - 200, 20);
	}

	// Draw Debug Text //
	if (bDebugText) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

		stringstream ss;

		ss << "Screen: " << ofToString(ofGetWidth()) << "x" << ofToString(ofGetHeight()) << endl;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl;
		ss << "Delta time:  " << ofToString(ofGetLastFrameTime(), 4) << endl;
		ss << endl;

		ss << "Camera Position: " << endl << ofToString(camera.getGlobalPosition(), 2) << endl;
		ss << "Camera Orientation: " << endl << ofToString(camera.getOrientationEuler(), 2) << endl;
		ss << endl;

		ss << "Points: " << ofToString(numPoints) << endl;
		ss << endl;

		ss << "Point Picker: " << ofToString(bPointPicker) << endl;
		ss << "Draw Points: " << ofToString(bDrawPoints) << endl;
		ss << endl;

		if (bPointPicker)
		{
			ss << "Nearest Point:" << endl;
			ss << "Index: " << ofToString(nearestIndex) << endl;
			ss << "Distance: " << ofToString(nearestDistance) << endl;
			ss << "Audio File: " << endl << audioFiles[nearestIndex].getFileName() << endl;
			ss << "RMS Amplitude: " << pointOrigins.getVertex(nearestIndex).y / rmsAmplitudeScale << endl;
			ss << "Spectral Centroid: " << pointOrigins.getVertex(nearestIndex).x / spectralCentroidScale << endl;
			ss << "Length: " << pointOrigins.getVertex(nearestIndex).z / lengthScale << endl;
			ss << endl;
		}

		ss << "(wasdqe): Move Mesh" << endl;
		ss << "(.): Toggle Fullscreen" << endl;
		ss << "(h): Toggle Debug Text" << endl;
		ss << "(o): Load Audio Files" << endl;
		ss << "(;): Set Point Size" << endl;
		ss << "(j): Toggle Point Picker" << endl;
		ss << "(k): Toggle Draw Points" << endl;
		ss << "(space): Reset Camera/Mesh" << endl;

		ofDrawBitmapStringHighlight(ss.str().c_str(), 20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::loadAudioFiles() {
	ofFileDialogResult result = ofSystemLoadDialog("Select samples folder", true, ofFilePath::getAbsolutePath("samples"));
	if (result.bSuccess) {
		dir.close();
		folders.clear();
		audioFiles.clear();
		points.clear();
		pointOrigins.clear();
		numPoints = 0;

		bLoading = true;
		bPointPicker = false;

		ofFile topDir(result.getPath());
		folders.insert(folders.end(), topDir);

		partialLoad(result.getPath()); // TO REMOVE
	}
}

void ofApp::partialLoad(const string& path) {
	folders.pop_back(); // remove current folder from list

	checkFolder(path, "", folders); // check for subfolders

	checkFolder(path, "mp3", audioFiles); // check for audio files

	searchedFolders++;
}

void ofApp::checkFolder(const string& path, const string& extension, vector<ofFile>& files) {
	dir.extensions.clear();
	dir.allowExt(extension);
	dir.listDir(path);
	if (dir.getFiles().size() > 0)
		files.insert(files.end(), dir.getFiles().begin(), dir.getFiles().end());
}

//--------------------------------------------------------------
void ofApp::analyseAudioFiles() {
	currentAudioFile.free();
	analysisIndex = 0;
	bAnalysing = true;
}

void ofApp::partialAnalyse() {
	currentAudioFile.load(audioFiles[analysisIndex].getAbsolutePath());
	if (!currentAudioFile.loaded()) {
		failedAnalysisCount++;
		cout << "Failed to load " << audioFiles[analysisIndex].getFileName() << endl;
		analysisIndex++;
		return;
	}

	float rms = getRMSAmplitude(currentAudioFile);
	float spectralCentroid = getSpectralCentroid(currentAudioFile);
	float length = (float)currentAudioFile.length() / (float)currentAudioFile.samplerate();

	points.addVertex({ spectralCentroid * spectralCentroidScale, rms * rmsAmplitudeScale, length * lengthScale });

	analysisIndex++;
}

float ofApp::getRMSAmplitude(ofxAudioFile& audioFile) {
	float rms = 0.0;
	int sampsCounted = 0;

	for (int i = 0; i < audioFile.length(); i++) {
		float leftSample = audioFile.sample(i, 0) * 0.5;
		float rightSample = audioFile.sample(i, 1) * 0.5;

		rms += leftSample * leftSample;
		rms += rightSample * rightSample;
		sampsCounted += 2;
	}

	rms /= (float)sampsCounted;
	rms = sqrt(rms);

	return rms;
}

float ofApp::getSpectralCentroid(ofxAudioFile& audioFile) {
	return 0.0;
}

//--------------------------------------------------------------
void ofApp::meshRotation(float deltaSpeed) {
	ofVec3f rotationAxis = {
				float(rotatePoints[0] - rotatePoints[1]),
				float(rotatePoints[2] - rotatePoints[3]),
				float(rotatePoints[4] - rotatePoints[5]) };
	glm::vec3 center = points.getCentroid();

	for (int i = 0; i < numPoints; i++) {
		ofVec3f vertex = points.getVertex(i) - center;
		vertex.rotate(deltaSpeed, rotationAxis);
		vertex += center;
		points.setVertex(i, vertex);
	}
}

void ofApp::pointPicker() {
	mouse = { mouseX, mouseY, 0 };

	for (int i = 0; i < numPoints; i++) {
		glm::vec3 vertex = camera.worldToScreen(points.getVertex(i));
		float distance = glm::distance(vertex, mouse);
		if (i == 0 || distance < nearestDistance) {
			nearestDistance = distance;
			nearestVertexScreenCoordinate = vertex;
			nearestIndex = i;
		}
	}
}

void ofApp::soundController() {
	for (int i = 0; i < sounds.size(); i++) {
		if (!sounds[i].getIsPlaying())
			sounds.erase(sounds.begin() + i);
	}

	if (nearestDistance > 15)
		lastSoundIndex = 0;

	if (bPointPicker && ofGetKeyPressed(' ') && nearestDistance < 15 && nearestIndex != lastSoundIndex)
	{
		sounds.push_back(ofSoundPlayer());
		sounds.back().load(audioFiles[nearestIndex]);
		sounds.back().setVolume(0.35);
		sounds.back().play();
		lastSoundIndex = nearestIndex;
	}
	ofSoundUpdate();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	
	switch (key) {
	case 'w':
		rotatePoints[0] = 1; break;
	case 's':
		rotatePoints[1] = 1; break;
	case 'a':
		rotatePoints[2] = 1; break;
	case 'd':
		rotatePoints[3] = 1; break;
	case 'q':
		rotatePoints[4] = 1; break;
	case 'e':
		rotatePoints[5] = 1; break;
	}
}

void ofApp::keyReleased(int key) {
	switch (key) {
	case 'w':
		rotatePoints[0] = 0; break;
	case 's':
		rotatePoints[1] = 0; break;
	case 'a':
		rotatePoints[2] = 0; break;
	case 'd':
		rotatePoints[3] = 0; break;
	case 'q':
		rotatePoints[4] = 0; break;
	case 'e':
		rotatePoints[5] = 0; break;
	case '.':
		ofToggleFullscreen(); break;
	case 'h':
		bDebugText = !bDebugText; break;
	case 'j':
		if (!bLoading && !bAnalysing && numPoints > 0) {
			bPointPicker = !bPointPicker;
		}
		break;
	case 'k':
		if (points.getNumVertices() > 0) {
			bDrawPoints = !bDrawPoints;
		}
		break;
	case 'o':
		loadAudioFiles(); break;
	case ';':
		glPointSize(ofToFloat(ofSystemTextBoxDialog("Enter point size: "))); break;
	case 'r':
		camera.setGlobalPosition({ 0,0,camera.getImagePlaneDistance(ofGetCurrentViewport()) });
		camera.setGlobalOrientation({ 0,0,0,1 });
		points = pointOrigins;
		break;
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

void ofApp::mouseDragged(int x, int y, int button) {

}

void ofApp::mousePressed(int x, int y, int button) {

}

void ofApp::mouseReleased(int x, int y, int button) {

}

void ofApp::mouseEntered(int x, int y) {

}

void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

void ofApp::gotMessage(ofMessage msg) {

}

void ofApp::dragEvent(ofDragInfo dragInfo) { 

}
