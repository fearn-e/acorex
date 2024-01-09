#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(30);

	bDebugText = true;
	
	previousTime = ofGetElapsedTimef();
	deltaTime = 0.0;

	// Camera //

	//mainCam.setGlobalPosition({ 0,0,0 });
	camera.setGlobalPosition({ 0,0,camera.getImagePlaneDistance(ofGetCurrentViewport()) });

	cameraSpeed = 75.0;
	for (int i = 0; i < 6; i++) {
		cameraMove[i] = 0;
	}

	// Points //

	numPoints = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	float currentTime = ofGetElapsedTimef();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	
	float deltaSpeed = cameraSpeed * deltaTime;

	float cameraMoveX = (cameraMove[0] - cameraMove[1]) * deltaSpeed;
	float cameraMoveY = (cameraMove[2] - cameraMove[3]) * deltaSpeed;
	float cameraMoveZ = (cameraMove[4] - cameraMove[5]) * deltaSpeed;

	camera.move(cameraMoveX, cameraMoveY, cameraMoveZ);
}

//--------------------------------------------------------------
void ofApp::draw() {
	
	// Draw Points //

	camera.begin();
	ofEnableDepthTest();

	ofFill();
	for (int i = 0; i < numPoints; i++) {
		points[i].draw();
	}

	ofDisableDepthTest();
	ofFill();
	camera.end();
	
	// Draw Debug Text //

	if (bDebugText) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
		stringstream ss;
		ss << "Screen Size: " << ofToString(ofGetWidth()) << "x" << ofToString(ofGetHeight()) << endl << endl;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "Delta Time: " << ofToString(deltaTime, 4) << endl << endl;
		ss << "Number of Points: " << ofToString(numPoints) << endl << endl;

		ss << "Camera Position: " << ofToString(camera.getGlobalPosition(), 2) << endl;
		ss << "Camera Orientation: " << ofToString(camera.getOrientationEuler(), 2) << endl;
		ss << "Camera Speed: x:" << ofToString(cameraMove[0] - cameraMove[1]) << " y: " << ofToString(cameraMove[2] - cameraMove[3]) << " z: " << ofToString(cameraMove[4] - cameraMove[5]) << endl << endl;

		ss << "(w/a/s/d/r/f): Move Camera" << endl;
		ss << "(.): Toggle Fullscreen" << endl;
		ss << "(h): Toggle Debug Text" << endl;
		ss << "(p): Spawn Random Points" << endl;
		ss << "(1/2/3/4/5): Set Point Resolution" << endl;
		ofDrawBitmapStringHighlight(ss.str().c_str(), 20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
	// camera move [x, -x, y, -y, z, -z,]

	switch (key) {
	case 'a':
		cameraMove[1] = 1; break;
	case 'd':
		cameraMove[0] = 1; break;
	case 'r':
		cameraMove[2] = 1; break;
	case 'f':
		cameraMove[3] = 1; break;
	case 'w':
		cameraMove[5] = 1; break;
	case 's':
		cameraMove[4] = 1; break;
	case '.':
		ofToggleFullscreen(); break;
	case 'h':
		bDebugText = !bDebugText; break;
	case '1':
		for (int i = 0; i < numPoints; i++) { points[i].setResolution(1); } break;
	case '2':
		for (int i = 0; i < numPoints; i++) { points[i].setResolution(2); } break;
	case '3':
		for (int i = 0; i < numPoints; i++) { points[i].setResolution(4); } break;
	case '4':
		for (int i = 0; i < numPoints; i++) { points[i].setResolution(8); } break;
	case '5':
		for (int i = 0; i < numPoints; i++) { points[i].setResolution(16); } break;
	case 'p':
		numPoints = ofToInt(ofSystemTextBoxDialog("Enter number of points: "));
		points.resize(numPoints);
		for (int i = 0; i < numPoints; i++) {
			points[i].setResolution(2);
			points[i].setRadius(0.8);
			points[i].setGlobalPosition({ ofRandom(-ofGetWidth(), ofGetWidth()), ofRandom(-ofGetHeight(), ofGetHeight()), ofRandom(-ofGetWidth(), ofGetWidth()) });
		}
		break;

		/*for (int i = 0; i < numPoints; i++) {
		glm::vec3 pos;
		pos.x = ofRandom(-100, 100);
		pos.y = ofRandom(-100, 100);
		pos.z = ofRandom(-100, 100);
		points.push_back(ofSpherePrimitive());
		points.back().setPosition(pos);
		points.back().setRadius(0.3);
		points.back().setResolution(2);
		}*/
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case 'a':
		cameraMove[1] = 0;
		break;
	case 'd':
		cameraMove[0] = 0;
		break;
	case 'r':
		cameraMove[2] = 0;
		break;
	case 'f':
		cameraMove[3] = 0;
		break;
	case 'w':
		cameraMove[5] = 0;
		break;
	case 's':
		cameraMove[4] = 0;
		break;
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
