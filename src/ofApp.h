#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp{

	public:
		void setup();

		void update();
		void updateWhileLoading();
		void draw();

		void loadAudioFiles();
		void partialLoad(const string& path);
		void checkFolder(const string& path, const string& extension, vector<ofFile>& files);

		void meshRotation(float deltaSpeed);
		void pointPicker();
		void soundController();

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
		bool bLoading;
		
		float previousTime;
		float deltaTime;

		ofEasyCam camera;

		float rotationSpeed;
		bool rotatePoints[6];

		int numPoints;
		ofMesh points;
		ofMesh pointOrigins;
		
		int nearestIndex;
		glm::vec2 nearestVertexScreenCoordinate;
		float nearestDistance;
		glm::vec3 mouse;

		ofDirectory dir;
		vector<ofFile> folders;
		vector<ofFile> audioFiles;

		vector<ofSoundPlayer> sounds;
		int lastSoundIndex;
};