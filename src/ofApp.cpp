#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(30);

	bDebugText = true;
	
	previousTime = ofGetElapsedTimef();
	deltaTime = 0.0;

	// Mesh //

	numPoints = 0;
	points.setMode(OF_PRIMITIVE_POINTS);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(2.0);

	// Camera //

	//mainCam.setGlobalPosition({ 0,0,0 });
	camera.setGlobalPosition({ 0,0,camera.getImagePlaneDistance(ofGetCurrentViewport()) });

	rotationSpeed = 45.0;
	for (int i = 0; i < 6; i++) {
		rotatePoints[i] = 0;
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	float currentTime = ofGetElapsedTimef();
	deltaTime = currentTime - previousTime;
	previousTime = currentTime;
	
	float deltaSpeed = rotationSpeed * deltaTime;

	bool movement = rotatePoints[0] || rotatePoints[1] || rotatePoints[2] || rotatePoints[3] || rotatePoints[4] || rotatePoints[5];

	if (movement)
	{
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

	// Nearest Point //
	if (bPointPicker) {

		glm::vec3 mouse(mouseX, mouseY, 0);

		for (int i = 0; i < numPoints; i++) {
			glm::vec3 vertex = camera.worldToScreen(points.getVertex(i));
			float distance = glm::distance(vertex, mouse);
			if (i == 0 || distance < nearestDistance) {
				nearestDistance = distance;
				nearestVertex = vertex;
				nearestIndex = i;
			}
		}

		// Draw Nearest Point //
		if (nearestDistance < 10) {
			ofFill();
			ofSetColor(ofColor::gray);
			ofDrawLine(nearestVertex, mouse);

			ofNoFill();
			ofSetColor(ofColor::yellow);
			ofSetLineWidth(2);
			ofDrawCircle(nearestVertex, 4);
			ofSetLineWidth(1);

			glm::vec2 offset(10, -10);
			ofDrawBitmapStringHighlight(ofToString(nearestIndex), mouse + offset);
		}
	}

	// Draw Debug Text //
	if (bDebugText) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
		stringstream ss;
		ss << "Screen: " << ofToString(ofGetWidth()) << "x" << ofToString(ofGetHeight()) << endl << endl;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "Delta Time: " << ofToString(deltaTime, 4) << endl << endl;

		ss << "Points: " << ofToString(numPoints) << endl;
		ss << "Point Rotation:" << endl;
		ss << ofToString(rotatePoints[0] - rotatePoints[1]);
		ss << ", " << ofToString(rotatePoints[2] - rotatePoints[3]);
		ss << ", " << ofToString(rotatePoints[4] - rotatePoints[5]) << endl << endl;

		ss << "Camera Position: " << endl << ofToString(camera.getGlobalPosition(), 2) << endl;
		ss << "Camera Orientation: " << endl << ofToString(camera.getOrientationEuler(), 2) << endl;

		if (bPointPicker)
		{
			ss << "Nearest Point:" << endl;
			ss << "Index: " << ofToString(nearestIndex) << endl;
			ss << "Distance: " << ofToString(nearestDistance) << endl;
			ss << "Vertex: " << ofToString(nearestVertex) << endl << endl;
		}
		ss << "(wasdqe): Move Mesh" << endl;
		ss << "(.): Toggle Fullscreen" << endl;
		ss << "(h): Toggle Debug Text" << endl;
		ss << "(p): Spawn Random Points" << endl;
		ss << "(;): Set Point Size" << endl;
		ss << "(j): Toggle Point Picker" << endl;
		ss << "(k): Toggle Draw Points" << endl;
		ss << "(space): Reset Camera/Mesh" << endl;
		ofDrawBitmapStringHighlight(ss.str().c_str(), 20, 20);
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
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
	case '.':
		ofToggleFullscreen(); break;
	case 'h':
		bDebugText = !bDebugText; break;
	case 'j':
		bPointPicker = !bPointPicker; break;
	case 'k':
		bDrawPoints = !bDrawPoints; break;
	case 'p':
		numPoints = ofToInt(ofSystemTextBoxDialog("Enter number of points: "));
		points.clear();
		for (int i = 0; i < numPoints; i++) {
			points.addVertex({ ofRandom(-100, 100), ofRandom(-100, 100), ofRandom(-100, 100) });
		}
		pointOrigins = points;
		break;
	case ';':
		glPointSize(ofToFloat(ofSystemTextBoxDialog("Enter point size: ")));
		break;
	case ' ':
		camera.setGlobalPosition({ 0,0,camera.getImagePlaneDistance(ofGetCurrentViewport()) });
		camera.setGlobalOrientation({ 0,0,0,1 });
		points = pointOrigins;
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	switch (key) {
	case 'w':
		rotatePoints[0] = 0;
		break;
	case 's':
		rotatePoints[1] = 0;
		break;
	case 'a':
		rotatePoints[2] = 0;
		break;
	case 'd':
		rotatePoints[3] = 0;
		break;
	case 'q':
		rotatePoints[4] = 0;
		break;
	case 'e':
		rotatePoints[5] = 0;
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
