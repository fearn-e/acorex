#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		bool bDebugText;
		bool bPointPicker;
		bool bDrawPoints;
		
		float previousTime;
		float deltaTime;

		ofEasyCam camera;

		float rotationSpeed;
		bool rotatePoints[6];
		ofNode pointCloudCenter;

		//ofNode pointCloudNode;

		int numPoints;
		ofMesh points;
		
		int nearestIndex;
		glm::vec2 nearestVertex;
		float nearestDistance;
};
