#pragma once

#include "ofMain.h"
#include "ofxAudioFile.h"
#include "ofxFft.h"

class Corpus {

public:
	Corpus();
	~Corpus();

	void init(bool logFreqSetting, float rmsMinCutoffSetting, 
			int fftBufferSizeSetting, int stftHopRatioSetting);

	void beginAnalyse(const vector<ofFile>& fileList, bool logFreqSetting, 
					float rmsMinCutoffSetting, int fftBufferSizeSetting, int stftHopRatioSetting);

	void draw(ofCamera& camera, glm::vec3 mouse, bool drawPointsEnabled, bool listingInProgress, bool pointPickEnabled, bool pointPickSelected);

	bool process();

	void analyseOneFile();
	void deinterleaveAudioData(vector<vector<float>>& deinterleavedData, float* interleavedData, 
								int fileSize, int numChannels);
	float spectralCentroidOneFrame(float* input, float sampleRate, bool logFreq);

	void updateScales();

	int pointPicker(glm::vec3 mouse, ofCamera camera, bool selected);

	void soundController(bool pointPickEnabled);

	void setPointPickerSelectedSubset(bool& selected);

	glm::vec3 getMaxDimensions();
	int getPointCount();
	int getFileFromPoint(int index);

private:
	ofMesh points;

	vector<int> audioFileIndexLink;
	vector<bool> connectToNextPoint;

	ofMesh pointsSelectedSubset;
	vector<int> selectedSubsetIndexLink;

	vector<ofFile> audioFiles;

	int nearestIndex;
	int analysisIndex;
	ofxAudioFile currentAudioFile;

	ofxFft* fft;
	int fftBufferSize;
	int stftHopRatio;
	int stftHopSize;

	float rmsScale, centroidScale, timeScale;
	float rmsMax, centroidMax, timeMax;
	float rmsMinCutoff;

	bool bLogFreq;

	vector<ofSoundPlayer> sounds;
	int lastSoundIndex;

	bool bInProgress;
};

