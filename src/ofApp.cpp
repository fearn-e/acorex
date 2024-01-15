#include "ofApp.h"
#include <cmath>

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(true);
	ofBackground(30);

	bDebugText = true;

	bListing = false;
	bAnalysing = false;
	
	previousUpdateTime = ofGetElapsedTimef();
	previousDrawTime = ofGetElapsedTimef();

	allowedExtensions = { "mp3", "wav", "flac", "ogg" };

	// Mesh //
	points.setMode(OF_PRIMITIVE_POINTS);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(3.0);

	bDrawPoints = true;

	// Point Picker //
	bPointPicker = false;
	nearestIndex = 0;
	nearestVertexScreenCoordinate = { 0,0 };
	nearestDistance = 0;

	// Mesh Rotation //
	rotationSpeed = 45.0;
	for (int i = 0; i < 6; i++) {
		rotatePoints[i] = 0;
	}

	// Point Scales //
	bLogFreq = true;

	if (bLogFreq) { spectralCentroidScale = -3.0; }
	else { spectralCentroidScale = -0.02; }
	rmsAmplitudeScale = 400.0;
	timePointScale = 60.0;

	if (bLogFreq) { maxSpectralCentroid = 127.0 * spectralCentroidScale; }
	else { maxSpectralCentroid = 20000.0 * spectralCentroidScale; }
	maxRMSAmplitude = 1.0 * rmsAmplitudeScale;
	maxTimePoint = 5.0 * timePointScale;

	// Camera //
	camera.setNearClip(0.1);
	camera.setFarClip(10000);
	resetCamera();

	// Analysis //
	fftBufferSize = 4096;
	stftHopSize = fftBufferSize / 2;
	minimumRMSAmplitude = 0.02;
}

//--------------------------------------------------------------
void ofApp::update() {

	// Calculate Update FPS & Delta Time //
	float currentTime = ofGetElapsedTimef();
	float deltaTime = currentTime - previousUpdateTime;
	previousUpdateTime = currentTime;

	updateFPS *= 0.9;
	updateFPS += 1.0 / deltaTime * 0.1;

	// Loading/Analysing //
	if (bListing)
	{
		updateWhileListing();
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

void ofApp::updateWhileListing() {
	if (folders.size() > 0) {
		partialList(folders.back().getAbsolutePath());
	}
	else {
		bListing = false;

		if (audioFiles.size() > 0) {
			analyseAudioFiles();
		}
		else {
			ofSystemAlertDialog("No audio files found.");

			if (points.getNumVertices() > 0)
				bPointPicker = true;
		}
	}
}

void ofApp::updateWhileAnalysing() {
	if (analysisIndex < audioFiles.size()) {
		partialAnalyse(bLogFreq);
	}
	else {
		bAnalysing = false;

		pointOrigins = points;
		
		stringstream ss;
		ss << "Analysed " << audioFiles.size() << " audio files." << endl;
		ss << "Points: " << ofToString(points.getNumVertices()) << endl;
		ss << "Failed to load " << failedAnalysisCount << " audio files." << endl;
		ss << "Analysis complete." << endl;
		ofSystemAlertDialog(ss.str().c_str());

		resetCamera();

		bPointPicker = true;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	// Calculate Draw FPS //
	float currentTime = ofGetElapsedTimef();
	float deltaTime = currentTime - previousDrawTime;
	previousDrawTime = currentTime;

	drawFPS *= 0.9;
	drawFPS += 1.0 / deltaTime * 0.1;

	// Draw axes, grid, and labels //
	{
		camera.begin();
		ofDrawBitmapMode(OF_BITMAPMODE_BILLBOARD);

		/*int numSubdivisions = 5;
		ofSetColor(ofColor::darkSlateGray);
		for (int x = 0; x < numSubdivisions; x++) {
			for (int y = 0; y < numSubdivisions; y++) {
				for (int z = 0; z < numSubdivisions; z++) {
					ofDrawLine(x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions,
						(x + 1) * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions);
					ofDrawLine(x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions,
						x * maxSpectralCentroid / numSubdivisions, (y + 1) * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions);
					ofDrawLine(x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions,
						x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, (z + 1) * maxTimePoint / numSubdivisions);
				}
			}
		}*/

		ofSetColor(ofColor::darkGray);
		ofNoFill();
		ofDrawLine(0, 0, 0, maxSpectralCentroid, 0, 0);
		ofDrawLine(0, 0, 0, 0, maxRMSAmplitude, 0);
		ofDrawLine(0, 0, 0, 0, 0, maxTimePoint);

		ofSetColor(ofColor::white);
		ofDrawBitmapString("Spectral Centroid", maxSpectralCentroid, 0, 0);
		ofDrawBitmapString("RMS Amplitude", 0, maxRMSAmplitude, 0);
		ofDrawBitmapString("Time Point", 0, 0, maxTimePoint);
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

	// Draw lines connecting points from the same audio file //
	if (bDrawPoints && points.getNumVertices() != 0) {
		ofEnableDepthTest();
		camera.begin();
		ofSetColor(220);
		for (int i = 0; i < (points.getNumVertices() - 1); i++) {
			bool sameFile = audioFileIndexLink[i] == audioFileIndexLink[i + 1];
			if (sameFile) {
				if (connectToNextPoint[i]) { ofSetColor({220, 220, 220}); }
				if (!connectToNextPoint[i]) { ofSetColor({150, 140, 230}); }
				ofVec3f point = points.getVertex(i);
				ofVec3f nextPoint = points.getVertex(i + 1);
				ofDrawLine(point, nextPoint);
			}
		}
		camera.end();
		ofDisableDepthTest();
	}

	// Draw Nearest Point //
	if (!bListing && bPointPicker && nearestDistance < 15) {
		ofVec3f point = points.getVertex(nearestIndex);

		camera.begin();
		ofSetColor(ofColor::gray);
		ofDrawLine(point, { point.x, point.y, 0});
		ofDrawLine(point, { 0, point.y, point.z });
		ofDrawLine(point, { point.x, 0, point.z });
		ofDrawLine({ point.x, point.y, 0 }, { point.x, 0, 0 });
		ofDrawLine({ 0, point.y, point.z }, { 0, point.y, 0 });
		ofDrawLine({ point.x, 0, point.z }, { 0, 0, point.z });
		
		ofSetColor(ofColor::red);
		if (bLogFreq) { ofDrawBitmapString(ofToString(pow(2, ((point.x / spectralCentroidScale) - 69.0) / 12.0) * 440.0, 1) + "Hz", point.x, 0, 0); }
		else { ofDrawBitmapString(ofToString(point.x / spectralCentroidScale, 1) + "Hz", point.x, 0, 0); }
		ofDrawBitmapString(ofToString(point.y / rmsAmplitudeScale, 3), 0, point.y, 0);
		ofDrawBitmapString(ofToString(point.z / timePointScale, 2) + "s", 0, 0, point.z);

		camera.end();

		glm::vec2 offset(10, -10);
		ofSetColor(ofColor::red);
		ofDrawBitmapString(ofToString(nearestIndex) + ". " + audioFiles[audioFileIndexLink[nearestIndex]].getFileName(), mouse + offset);

		ofNoFill();
		ofSetColor(ofColor::red);
		ofSetLineWidth(2);
		ofDrawCircle(nearestVertexScreenCoordinate, 4);
		ofSetLineWidth(1);
	}

	// Highlight All Points From Same Selected Audio File //
	if (!bListing && bPointPicker && nearestDistance < 15) {
		ofSetColor(ofColor::red);
		int currentFile = audioFileIndexLink[nearestIndex];
		int fileIndex = nearestIndex;
		do {
			fileIndex--;
		} while (fileIndex >= 0 && audioFileIndexLink[fileIndex] == currentFile);
		fileIndex++;
		bool firstPoint = true;
		ofVec3f previousPoint;
		do {
			ofVec3f point = points.getVertex(fileIndex);
			ofFill();
			ofDrawCircle(camera.worldToScreen(point), 3);
			if (firstPoint) { firstPoint = false; }
			else {
				if (connectToNextPoint[fileIndex - 1]) { ofSetColor(ofColor::red); }
				if (!connectToNextPoint[fileIndex - 1]) { ofSetColor(ofColor::blue); }
				ofDrawLine(camera.worldToScreen(previousPoint), camera.worldToScreen(point));
			}
			previousPoint = point;
			fileIndex++;
		} while (fileIndex < points.getNumVertices() && audioFileIndexLink[fileIndex] == currentFile);
	}

	// Draw Loading/Analysing Text //
	if (bListing) {
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
		ss << "Update FPS: " << ofToString(updateFPS, 0) << endl;
		ss << "Draw FPS: " << ofToString(drawFPS, 0) << endl;
		ss << "Delta time:  " << ofToString(ofGetLastFrameTime(), 4) << endl;
		ss << endl;

		ss << "Camera Position: " << endl << ofToString(camera.getGlobalPosition(), 2) << endl;
		ss << "Camera Orientation: " << endl << ofToString(camera.getOrientationEuler(), 2) << endl;
		ss << endl;

		ss << "Points: " << ofToString(points.getNumVertices()) << endl;
		ss << "Audio Files: " << ofToString(audioFiles.size()) << endl;
		ss << endl;

		ss << "Point Picker: " << ofToString(bPointPicker) << endl;
		ss << "Draw Points: " << ofToString(bDrawPoints) << endl;
		ss << endl;

		ss << "(wasdqe): Move Mesh" << endl;
		ss << "(.): Toggle Fullscreen" << endl;
		ss << "(h): Toggle Debug Text" << endl;
		ss << "(o): Load Audio Files" << endl;
		ss << "(;): Set Point Size" << endl;
		ss << "(j): Toggle Point Picker" << endl;
		ss << "(k): Toggle Draw Points" << endl;
		ss << "(r): Reset Camera/Mesh" << endl;

		ofDrawBitmapStringHighlight(ss.str().c_str(), 20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::listAudioFiles() {
	ofFileDialogResult result = ofSystemLoadDialog("Select samples folder", true, ofFilePath::getAbsolutePath("samples"));
	if (result.bSuccess) {
		dir.close();
		folders.clear();
		audioFiles.clear();
		points.clear();
		pointOrigins.clear();
		searchedFolders = 0;
		
		bListing = true;
		bPointPicker = false;

		ofFile topDir(result.getPath());
		folders.insert(folders.end(), topDir);
	}
}

void ofApp::partialList(const string& path) {
	folders.pop_back(); // remove current folder from list

	checkFolder(path, { "" }, folders); // check for subfolders

	checkFolder(path, allowedExtensions, audioFiles); // check for audio files

	searchedFolders++;
}

void ofApp::checkFolder(const string& path, const vector<string>& extensions, vector<ofFile>& files) {
	dir.extensions.clear();
	dir.extensions = extensions;
	dir.listDir(path);
	if (dir.getFiles().size() > 0)
		files.insert(files.end(), dir.getFiles().begin(), dir.getFiles().end());
}

//--------------------------------------------------------------
void ofApp::analyseAudioFiles() {
	currentAudioFile.free();
	analysisIndex = 0;
	audioFileIndexLink.clear();
	maxTimePoint = 5.0 * timePointScale;
	bAnalysing = true;
	fft = ofxFft::create(fftBufferSize, OF_FFT_WINDOW_HAMMING);
}

void ofApp::partialAnalyse(bool logFreq) {
	currentAudioFile.load(audioFiles[analysisIndex].getAbsolutePath());
	if (!currentAudioFile.loaded()) {
		failedAnalysisCount++;
		cout << "Failed to load " << audioFiles[analysisIndex].getFileName() << endl;
		analysisIndex++;
		return;
	}

	// deinterleave audio data
	int channelSize = currentAudioFile.length();
	bool singleChannel = currentAudioFile.channels() == 1;
	if (!singleChannel) {
		deinterleaveAudioData(currentAudioFile.data(), currentAudioFile.length(), currentAudioFile.channels());
	}
	
	// calculate number of frames
	int numFrames = ceil((float)(channelSize - fftBufferSize) / (float)stftHopSize) + 1;
	
	// check if zero padding is needed for last frame
	bool zeroPad = false;
	if (channelSize % stftHopSize != 0) { zeroPad = true; }

	// loop over frames
	bool skippedPoint = true;
	float finalTimePoint = 0.0;
	for (int frame = 0; frame < numFrames - zeroPad; frame++) {
		// calculate spectral centroid
		float spectralCentroid = 0.0;
		if (singleChannel) {
			float* framePointer = currentAudioFile.data() + frame * stftHopSize;
			spectralCentroid = spectralCentroidOneFrame(framePointer, currentAudioFile.samplerate(), logFreq);
		}
		else {
			for (int channel = 0; channel < currentAudioFile.channels(); channel++) {
				float* framePointer = &deinterleavedAudioData[channel][frame * stftHopSize];
				spectralCentroid += spectralCentroidOneFrame(framePointer, currentAudioFile.samplerate(), logFreq);
			}
			spectralCentroid /= currentAudioFile.channels();
		}

		// calculate RMS amplitude
		float rms = 0.0;

		for (int i = 0; i < fftBufferSize * currentAudioFile.channels(); i++) {
			float sample = currentAudioFile.data()[i + frame * stftHopSize];
			rms += sample * sample;
		}

		rms /= (float)(fftBufferSize * currentAudioFile.channels());
		rms = sqrt(rms);

		// calculate time point
		float timePoint = (float)(frame * stftHopSize) / (float)currentAudioFile.samplerate();

		// add point to mesh
		if (rms > minimumRMSAmplitude) {
			finalTimePoint = timePoint;

			points.addVertex({ spectralCentroid * spectralCentroidScale, rms * rmsAmplitudeScale, timePoint * timePointScale });
			audioFileIndexLink.push_back(analysisIndex);
			connectToNextPoint.push_back(false);
			//set previous point to connect to this point
			if (!skippedPoint) {
				connectToNextPoint[points.getNumVertices() - 2] = true;
			} 
			else { skippedPoint = false; }
		} 
		else { skippedPoint = true; }
	}

	// zero pad last frame
	if (zeroPad) {
		if (singleChannel) {
			int lastFrameStart = (numFrames - 1) * stftHopSize;
			int lastFrameSize = channelSize - lastFrameStart;
			float* lastFramePointer = currentAudioFile.data() + lastFrameStart;

			std::vector<float> lastFrame(lastFrameSize);
			memcpy(&lastFrame[0], lastFramePointer, sizeof(float) * lastFrameSize);
			for (int i = lastFrameSize; i < fftBufferSize; i++) {
				lastFrame.push_back(0.0);
			}

			float spectralCentroid = spectralCentroidOneFrame(&lastFrame[0], currentAudioFile.samplerate(), logFreq);
			float rms = 0.0;
			
			for (int i = 0; i < fftBufferSize; i++) {
				float sample = lastFrame[i];
				rms += sample * sample;
			}

			rms /= (float)fftBufferSize;
			rms = sqrt(rms);

			float timePoint = (float)(lastFrameStart) / (float)currentAudioFile.samplerate();
			finalTimePoint = timePoint;

			if (rms > minimumRMSAmplitude) {
				finalTimePoint = timePoint;

				points.addVertex({ spectralCentroid * spectralCentroidScale, rms * rmsAmplitudeScale, timePoint * timePointScale });
				audioFileIndexLink.push_back(analysisIndex);
				connectToNextPoint.push_back(false);
				//set previous point to connect to this point
				if (!skippedPoint) {
					connectToNextPoint[points.getNumVertices() - 2] = true;
				}
				else { skippedPoint = false; }
			}
			else { skippedPoint = true; }
		}
		else {
			float timePoint = (float)currentAudioFile.length() / (float)currentAudioFile.samplerate();
			float spectralCentroid = 0.0;
			float rms = 0.0;
			if (rms > minimumRMSAmplitude) {
				finalTimePoint = timePoint;

				points.addVertex({ spectralCentroid * spectralCentroidScale, rms * rmsAmplitudeScale, timePoint * timePointScale });
				audioFileIndexLink.push_back(analysisIndex);
				connectToNextPoint.push_back(false);
				//set previous point to connect to this point
				if (!skippedPoint) {
					connectToNextPoint[points.getNumVertices() - 2] = true;
				}
				else { skippedPoint = false; }
			}
			else { skippedPoint = true; }
			// TODO - zero pad last frame for multichannel audio
		}
	}

	if (finalTimePoint * timePointScale > maxTimePoint) { maxTimePoint = finalTimePoint * timePointScale; }

	analysisIndex++;
}

void ofApp::deinterleaveAudioData(float* interleavedData, int channelSize, int numChannels) {
	deinterleavedAudioData.resize(numChannels);

	int fileSize = channelSize * numChannels;

	for (int i = 0; i < numChannels; i++) {
		deinterleavedAudioData[i].resize(channelSize);
	}

	for (int i = 0; i < fileSize; i++) {
		deinterleavedAudioData[i % numChannels][i / numChannels] = interleavedData[i];
	}
}

float ofApp::spectralCentroidOneFrame(float* input, float sampleRate, bool logFreq) {
	std::vector<float> ampBins(fft->getBinSize());

	// normalise input
	float maxValue = 0.0;
	for (int i = 0; i < fftBufferSize; i++) {
		if (abs(input[i]) > maxValue) {
			maxValue = abs(input[i]);
		}
	}
	for (int i = 0; i < fftBufferSize; i++) {
		input[i] /= maxValue;
	}

	// do FFT
	fft->setSignal(input);

	float* fftPointer = fft->getAmplitude();
	memcpy(&ampBins[0], fftPointer, sizeof(float) * fft->getBinSize());

	// normalise output
	maxValue = 0.0;
	for (int i = 0; i < fft->getBinSize(); i++) {
		if (ampBins[i] > maxValue) {
			maxValue = ampBins[i];
		}
	}
	for (int i = 0; i < fft->getBinSize(); i++) {
		ampBins[i] /= maxValue;
	}

	int numBins = ampBins.size();
	float binHz = sampleRate / ((numBins - 1) * 2.0);
	float minBin = 0;
	float maxBin = (numBins - 1);

	// skip first bin if log frequency
	if (logFreq)
	{
		minBin = 1;
		ampBins[1] += ampBins[0];
		ampBins.erase(ampBins.begin());
	}

	// calculate frequency bins
	int size = maxBin - minBin;
	std::vector<float> freqBins(size);

	float minFreq = minBin * binHz;
	float maxFreq = maxBin * binHz;
	float step = (maxFreq - minFreq) / (size - 1);

	for (int i = 0; i < size; i++) {
		freqBins[i] = minFreq + i * step;
	}

	// log frequency scale
	if (logFreq) {
		for (int i = 0; i < size; i++) {
			freqBins[i] = 69.0 + (12.0 * log(freqBins[i] / 440.0) * 1.44269504088896340736); //log2E
		}
	}

	// calculate spectral centroid
	float ampSum = 0.0;
	for (int i = 0; i < size; i++) {
		ampSum += ampBins[i];
	}

	float ampFreqSum = 0.0;
	for (int i = 0; i < size; i++) {
		ampFreqSum += ampBins[i] * freqBins[i];
	}
	
	float centroid = ampFreqSum / ampSum;

	return centroid;
}

//--------------------------------------------------------------
void ofApp::meshRotation(float deltaSpeed) {
	ofVec3f rotationAxis = {
				float(rotatePoints[0] - rotatePoints[1]),
				float(rotatePoints[2] - rotatePoints[3]),
				float(rotatePoints[4] - rotatePoints[5]) };
	glm::vec3 center = points.getCentroid();

	int pointCount = points.getNumVertices();

	for (int i = 0; i < pointCount; i++) {
		ofVec3f vertex = points.getVertex(i) - center;
		vertex.rotate(deltaSpeed, rotationAxis);
		vertex += center;
		points.setVertex(i, vertex);
	}
}

void ofApp::pointPicker() {
	mouse = { mouseX, mouseY, 0 };

	int pointCount = points.getNumVertices();

	for (int i = 0; i < pointCount; i++) {
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
		sounds.back().load(audioFiles[audioFileIndexLink[nearestIndex]].getAbsolutePath());
		sounds.back().setVolume(0.35);
		sounds.back().play();
		lastSoundIndex = nearestIndex;
	}
	ofSoundUpdate();
}

void ofApp::resetCamera() {
	float camereDistanceMultiplier1 = 1.2;
	camera.setGlobalPosition({ 
		maxRMSAmplitude * -camereDistanceMultiplier1, 
		maxSpectralCentroid * -camereDistanceMultiplier1, 
		maxTimePoint * camereDistanceMultiplier1 
		});
	camera.lookAt({ 0, 0, 0 });
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
		if (!bListing && !bAnalysing && points.getNumVertices() > 0) {
			bPointPicker = !bPointPicker;
		}
		break;
	case 'k':
		if (points.getNumVertices() > 0) {
			bDrawPoints = !bDrawPoints;
		}
		break;
	case 'o':
		listAudioFiles(); break;
	case ';':
		glPointSize(ofToFloat(ofSystemTextBoxDialog("Enter point size: "))); break;
	case 'r':
		resetCamera();
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
