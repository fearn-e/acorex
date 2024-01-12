#pragma once

#include "ofMain.h"
#include "ofxAudioFile.h"

class ofApp : public ofBaseApp{

	public:

		void setup();

		void update();
		void updateWhileLoading();
		void updateWhileAnalysing();
		void draw();

		void loadAudioFiles();
		void partialLoad(const string& path);
		void checkFolder(const string& path, const vector<string>& extension, vector<ofFile>& files);

		void analyseAudioFiles();
		void partialAnalyse();
		float getRMSAmplitude(ofxAudioFile& audioFile);
		float getSpectralCentroid(ofxAudioFile& audioFile);

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
		bool bAnalysing;

		float drawFPS;
		float updateFPS;
		
		float previousDrawTime;
		float previousUpdateTime;

		ofEasyCam camera;

		float rotationSpeed;
		bool rotatePoints[6];

		ofMesh points;
		ofMesh pointOrigins;
		
		int nearestIndex;
		glm::vec2 nearestVertexScreenCoordinate;
		float nearestDistance;
		glm::vec3 mouse;

		vector<string> allowedExtensions;

		ofDirectory dir;
		vector<ofFile> folders;
		vector<ofFile> audioFiles;
		int searchedFolders;

		int analysisIndex;
		int failedAnalysisCount;
		ofxAudioFile currentAudioFile;

		float rmsAmplitudeScale;
		float spectralCentroidScale;
		float lengthScale;

		vector<ofSoundPlayer> sounds;
		int lastSoundIndex;
};
