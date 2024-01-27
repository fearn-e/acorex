#pragma once

#include "ofMain.h"
#include "ofxAudioFile.h"
#include "ofxFft.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void exit();

		void update();
		void updateWhileListing();
		void updateWhileAnalysing();
		void draw();

		void selectDirectory();
		void listAudioFiles();
		void partialList(const string& path);
		void checkFolder(const string& path, const vector<string>& extension, vector<ofFile>& files);

		void analyseAudioFiles();
		void partialAnalyse(bool logFreq);
		void deinterleaveAudioData(float* interleavedData, int fileSize, int numChannels);
		float spectralCentroidOneFrame(float* input, float sampleRate, bool logFreq);

		void quantiseFFTBufferSizeSlider(int& value);
		void quantiseSTFTHopRatioSlider(int& value);
		void quantiseMinimumRMSAmplitudeSlider(float& value);
		void updateScales(bool logFreq);
		void meshRotation(float deltaSpeed);
		void pointPicker();
		void soundController();
		void resetCamera();

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
		bool bListing;
		bool bAnalysing;
		bool bLogFreq;

		float drawFPS;
		float updateFPS;
		
		float previousDrawTime;
		float previousUpdateTime;

		ofEasyCam camera;

		float rotationSpeed;
		bool rotatePoints[6];

		ofMesh points;
		ofMesh pointOrigins;
		vector<int> audioFileIndexLink;
		vector<bool> connectToNextPoint;
		
		int nearestIndex;
		float nearestDistance;
		glm::vec3 mouse;

		vector<string> allowedExtensions;

		ofFile selectedDirectory;
		ofDirectory dir;
		vector<ofFile> folders;
		vector<ofFile> audioFiles;
		int searchedFolders;

		int analysisIndex;
		int failedAnalysisCount;
		ofxAudioFile currentAudioFile;

		vector<vector<float>> deinterleavedAudioData;

		ofxFft* fft;
		int stftHopSize;
		int fftBufferSize;
		int stftHopRatio;

		float rmsAmplitudeScale, spectralCentroidScale, timePointScale;
		float maxRMSAmplitude, maxSpectralCentroid, maxTimePoint;
		float minimumRMSAmplitude;

		ofxPanel gui;
		ofxButton selectDirectoryButton;
		ofxLabel currentDirectoryLabel;
		ofParameter<int> fftBufferSizeSlider;
		ofParameter<int> stftHopRatioSlider;
		ofParameter<float> minimumRMSAmplitudeSlider;
		ofParameter<bool> logFreqToggle;
		ofxButton beginAnalysisButton;

		vector<ofSoundPlayer> sounds;
		int lastSoundIndex;
};
