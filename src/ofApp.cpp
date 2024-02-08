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

	// Mesh //
	glEnable(GL_POINT_SMOOTH);
	glPointSize(3.0);

	bDrawPoints = true;

	// Point Picker //
	bPointPicker = false;
	nearestIndex = -1;

	// Mesh Rotation //
	rotationSpeed = 45.0;
	for (int i = 0; i < 6; i++) {
		rotatePoints[i] = 0;
	}

	// Camera //
	camera.setNearClip(0.1);
	camera.setFarClip(10000);
	resetCamera();

	// GUI //

	selectDirectoryButton.addListener(this, &ofApp::selectDirectory);
	beginAnalysisButton.addListener(this, &ofApp::listAudioFiles);
	fftBufferSizeSlider.addListener(this, &ofApp::quantiseFFTBufferSizeSlider);
	stftHopRatioSlider.addListener(this, &ofApp::quantiseSTFTHopRatioSlider);
	minimumRMSAmplitudeSlider.addListener(this, &ofApp::quantiseMinimumRMSAmplitudeSlider);

	gui.setup("Analysis Settings");
	gui.setWidthElements(500);
	gui.add(selectDirectoryButton.setup("Select Directory"));
	gui.add(currentDirectoryLabel.setup("Current Directory", ""));
	gui.add(fftBufferSizeSlider.set("FFT Buffer Size", 4096, 256, 16384));
	gui.add(stftHopRatioSlider.set("STFT Hop Ratio", 2, 1, 128));
	gui.add(minimumRMSAmplitudeSlider.set("Minimum RMS Amplitude", 0.02, 0.0, 1.0));
	gui.add(logFreqToggle.set("Log Frequency Display/Analysis", false));
	gui.add(beginAnalysisButton.setup("Begin Analysis"));
	gui.setPosition(ofGetWidth() - gui.getWidth() - 20, ofGetHeight() - gui.getHeight() - 20);
	gui.registerMouseEvents();
}

void ofApp::exit() {
	selectDirectoryButton.removeListener(this, &ofApp::selectDirectory);
	beginAnalysisButton.removeListener(this, &ofApp::listAudioFiles);
	fftBufferSizeSlider.removeListener(this, &ofApp::quantiseFFTBufferSizeSlider);
	stftHopRatioSlider.removeListener(this, &ofApp::quantiseSTFTHopRatioSlider);
	minimumRMSAmplitudeSlider.removeListener(this, &ofApp::quantiseMinimumRMSAmplitudeSlider);
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
		bListing = _fileLister.process();

		if (!bListing) {
			if (_fileLister.getAudioFiles().size() > 0) {
				audioFiles.clear();
				audioFiles = _fileLister.getAudioFiles();
				analyseAudioFiles();
			}
			else {
				ofSystemAlertDialog("No audio files found.");
				fileProcessingUnlocks();
			}
		}
		return;
	}
	else if (bAnalysing) {
		
		bAnalysing = _dataCtrl.process();

		if (!bAnalysing) {
			stringstream ss;
			ss << "Analysed " << audioFiles.size() << " audio files." << endl;
			ss << "Points: " << ofToString(_dataCtrl.getPointCount()) << endl;
			ss << "Analysis complete." << endl;
			ofSystemAlertDialog(ss.str().c_str());
			fileProcessingUnlocks();
		}
		return;
	}
	// Skip Below If Loading/Analysing ------------------------ //

	//bool movement = rotatePoints[0] || rotatePoints[1] || rotatePoints[2] || rotatePoints[3] || rotatePoints[4] || rotatePoints[5];
	//if (movement)
	//{
	//	float deltaSpeed = rotationSpeed * deltaTime;
	//	meshRotation(deltaSpeed);
	//}

	if (bPointPicker)
	{
		nearestIndex = _dataCtrl.pointPicker({ mouseX, mouseY, 0 }, camera, bPointPickerSelected);
	}

	_dataCtrl.soundController(bPointPicker);
}

//--------------------------------------------------------------
void ofApp::draw() {
	// Calculate Draw FPS //
	float currentTime = ofGetElapsedTimef();
	float deltaTime = currentTime - previousDrawTime;
	previousDrawTime = currentTime;

	drawFPS *= 0.9;
	drawFPS += 1.0 / deltaTime * 0.1;

	_dataCtrl.draw(camera, {mouseX, mouseY, 0}, bDrawPoints, bListing, bPointPicker, bPointPickerSelected);

	// Draw Listing Progress Text //
	if (bListing) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

		stringstream ss;
		ss << "Searching for audio files..." << endl;
		ss << "Found " << audioFiles.size() << " files." << endl;
		ss << "Searched " << _fileLister.getFolderCount() << " folders." << endl;
		ss << "Remaining " << _fileLister.getRemainingFolderCount() << " folders.";

		ofDrawBitmapString(ss.str().c_str(), ofGetWidth() - 200, 20);
	}

	// Draw Analysis Settings //
	gui.draw();

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

		ss << "Points: " << ofToString(_dataCtrl.getPointCount()) << endl;
		ss << "Audio Files: " << ofToString(audioFiles.size()) << endl;
		ss << endl;

		//ss << "Point Picker: " << ofToString(bPointPicker) << endl;
		//ss << "Draw Points: " << ofToString(bDrawPoints) << endl;
		//ss << endl;

		//ss << "(wasdqe): Move Mesh" << endl;
		ss << "(right click): Select file" << endl;

		ss << "(x): Open in Explorer" << endl;
		ss << "(c): Copy File Path to Clipboard" << endl;

		ss << "(r): Reset Camera/Mesh" << endl;
		ss << "(.): Toggle Fullscreen" << endl;

		//ss << "(j): Toggle Point Picker" << endl;
		//ss << "(k): Toggle Draw Points" << endl;

		ss << "(h): Toggle Debug Text" << endl;
		ss << "(;): Set Point Size" << endl;

		ofDrawBitmapStringHighlight(ss.str().c_str(), 20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::fileProcessingLocks() {
	bPointPicker = false;
	bPointPickerSelected = false;
	nearestIndex = -1;

	//hide gui off screen
	gui.setPosition(-gui.getWidth(), -gui.getHeight());
}

void ofApp::fileProcessingUnlocks() {
	if (_dataCtrl.getPointCount() > 0) {
		bPointPicker = true;
	}

	//return gui to screen
	gui.setPosition(ofGetWidth() - gui.getWidth() - 20, ofGetHeight() - gui.getHeight() - 20);
}

void ofApp::selectDirectory() {
	currentDirectoryLabel = _fileLister.setDirectory();
}

void ofApp::listAudioFiles() {
	if (_fileLister.getValidDirectorySelected()) {
		fileProcessingLocks();

		bListing = true;

		_fileLister.beginListing();
	}
	else
	{
		ofSystemAlertDialog("No directory selected.");
	}
}

void ofApp::analyseAudioFiles() {
	_dataCtrl.beginAnalyse(audioFiles, logFreqToggle, minimumRMSAmplitudeSlider, fftBufferSizeSlider, stftHopRatioSlider);
	bAnalysing = true;
}

//--------------------------------------------------------------
void ofApp::quantiseFFTBufferSizeSlider(int& value) {
	value = pow(2, round(log2(value)));
}

void ofApp::quantiseSTFTHopRatioSlider(int& value) {
	value = pow(2, round(log2(value)));
}

void ofApp::quantiseMinimumRMSAmplitudeSlider(float& value) {
	value = round(value * 1000.0) / 1000.0;
}

//void ofApp::meshRotation(float deltaSpeed) {
//	ofVec3f rotationAxis = {
//				float(rotatePoints[0] - rotatePoints[1]),
//				float(rotatePoints[2] - rotatePoints[3]),
//				float(rotatePoints[4] - rotatePoints[5]) };
//	glm::vec3 center = points.getCentroid();
//
//	int pointCount = points.getNumVertices();
//
//	for (int i = 0; i < pointCount; i++) {
//		ofVec3f vertex = points.getVertex(i) - center;
//		vertex.rotate(deltaSpeed, rotationAxis);
//		vertex += center;
//		points.setVertex(i, vertex);
//	}
//}

void ofApp::resetCamera() {
	float cameraDistanceMultiplier = 1.2;
	camera.setGlobalPosition(_dataCtrl.getMaxDimensions() * cameraDistanceMultiplier);
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
    //case 'j':
    //    if (!bListing && !bAnalysing && _dataCtrl.getPointCount() > 0) {
    //        bPointPicker = !bPointPicker;
    //    }
    //    break;
    //case 'k':
    //    if (_dataCtrl.getPointCount() > 0) {
    //        bDrawPoints = !bDrawPoints;
    //    }
    //    break;
    case ';':
        glPointSize(ofToFloat(ofSystemTextBoxDialog("Enter point size: "))); break;
    case 'r':
        resetCamera();
        break;
    case 'c':
        if (nearestIndex != -1) {
            ofSetClipboardString(audioFiles[_dataCtrl.getFileFromPoint(nearestIndex)].getAbsolutePath());
        }
        break;
    case 'x':
        if (nearestIndex != -1) {
#ifdef _WIN32 || _WIN64
			ofSystem("explorer /select," + audioFiles[_dataCtrl.getFileFromPoint(nearestIndex)].getAbsolutePath());
#endif
#ifdef __APPLE__ && __MACH__
			ofSystem("open " + audioFiles[_dataCtrl.getFileFromPoint(nearestIndex)].getAbsolutePath());
#endif
#ifdef __linux__
			ofSystem("xdg-open " + audioFiles[_dataCtrl.getFileFromPoint(nearestIndex)].getAbsolutePath());
#endif
        }
        break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

void ofApp::mouseDragged(int x, int y, int button) {

}

void ofApp::mousePressed(int x, int y, int button) {
	switch (button) {
	case 2:
		if (bPointPicker)
		{
			_dataCtrl.setPointPickerSelectedSubset(bPointPickerSelected);
		}
	}
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
