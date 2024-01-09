#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	ofBackground(30);

	bDebugText = true;
	
	previousTime = ofGetElapsedTimef();
	deltaTime = 0.0;

	// Mesh //

	//pointCloudNode.setGlobalPosition({ 0,0,0 });

	numPoints = 0;
	points.setMode(OF_PRIMITIVE_POINTS);

	glEnable(GL_POINT_SMOOTH);
	glPointSize(2.0);

	// Camera //

	//mainCam.setGlobalPosition({ 0,0,0 });
	camera.setGlobalPosition({ 0,0,camera.getImagePlaneDistance(ofGetCurrentViewport()) });

	cameraSpeed = 75.0;
	for (int i = 0; i < 6; i++) {
		cameraMove[i] = 0;
	}
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

	//pointCloudNode.rotateDeg(0.1, 0, 1, 0);
	//pointCloudNode.rotateDeg(deltaSpeed, (cameraMove[0] - cameraMove[1]), (cameraMove[2] - cameraMove[3]), (cameraMove[4] - cameraMove[5]));
}

//--------------------------------------------------------------
void ofApp::draw() {

	// Reference Sphere //
	{
		camera.begin();
		ofSetColor(ofColor::white);
		ofNoFill();
		ofDrawSphere(0, 0, 0, 300);
		camera.end();
	}

	// Draw Points //
	if (bDrawPoints) {
		ofEnableDepthTest();
		camera.begin();
		//pointCloudNode.transformGL();
		ofSetColor(220);
		points.draw();
		//pointCloudNode.restoreTransformGL();
		camera.end();
		ofDisableDepthTest();
	}

	/*ofFill();
	for (int i = 0; i < numPoints; i++) {
		ofDrawSphere(points[i], 0.8);
	}

	ofDisableDepthTest();
	ofFill();
	camera.end();*/

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
		ss << "Points: " << ofToString(numPoints) << endl << endl;

		ss << "Camera Position: " << endl << ofToString(camera.getGlobalPosition(), 2) << endl;
		ss << "Camera Orientation: " << endl << ofToString(camera.getOrientationEuler(), 2) << endl;
		ss << "Camera Speed:" << endl;
		ss << ofToString(cameraMove[0] - cameraMove[1]);
		ss << ", " << ofToString(cameraMove[2] - cameraMove[3]);
		ss << ", " << ofToString(cameraMove[4] - cameraMove[5]) << endl << endl;

		if (bPointPicker)
		{
			ss << "Nearest Point:" << endl;
			ss << "Index: " << ofToString(nearestIndex) << endl;
			ss << "Distance: " << ofToString(nearestDistance) << endl;
			ss << "Vertex: " << ofToString(nearestVertex) << endl << endl;
		}
		ss << "(wasdrf): Move Camera/Mesh" << endl;
		ss << "(.): Toggle Fullscreen" << endl;
		ss << "(h): Toggle Debug Text" << endl;
		ss << "(p): Spawn Random Points" << endl;
		ss << "(;): Set Point Size" << endl;
		ss << "(j): Toggle Point Picker" << endl;
		ss << "(k): Toggle Draw Points" << endl;
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
		break;
	case ';':
		glPointSize(ofToFloat(ofSystemTextBoxDialog("Enter point size: ")));
		break;
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
