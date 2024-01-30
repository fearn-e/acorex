#pragma once

#include "ofMain.h"
#include "ofxAudioFile.h"
#include "ofxFft.h"
#include "ofxGui.h"

#include "ListAudioFilesInDirectory.h"
#include "DataController.h"

class ofApp : public ofBaseApp {

public:

	void setup();
	void exit();

	void update();

	void draw();

	void fileProcessingLocks();
	void fileProcessingUnlocks();
	void selectDirectory();
	void listAudioFiles();
	void analyseAudioFiles();

	void quantiseFFTBufferSizeSlider(int& value);
	void quantiseSTFTHopRatioSlider(int& value);
	void quantiseMinimumRMSAmplitudeSlider(float& value);
	//void meshRotation(float deltaSpeed);
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
	
	// state boolean flags
	bool bDebugText;
	bool bPointPicker;
	bool bDrawPoints;
	bool bListing;
	bool bAnalysing;

	// camera
	ofEasyCam camera;

	// mesh rotating
	float rotationSpeed;
	bool rotatePoints[6];

	// debug fps and delta times
	float drawFPS, updateFPS;
	float previousDrawTime, previousUpdateTime;
		
	// nearest point
	int nearestIndex;

	// file listing
	ListAudioFilesInDirectory _fileLister;
	ofFile selectedDirectory;
	vector<ofFile> audioFiles;

	// file analysis
	DataController _dataCtrl;

	// gui
	ofxPanel gui;
	ofxButton selectDirectoryButton;
	ofxLabel currentDirectoryLabel;
	ofParameter<int> fftBufferSizeSlider;
	ofParameter<int> stftHopRatioSlider;
	ofParameter<float> minimumRMSAmplitudeSlider;
	ofParameter<bool> logFreqToggle;
	ofxButton beginAnalysisButton;
};
