#include "PointCloudAndFileControl.h"

PointCloudAndFileControl::PointCloudAndFileControl() {
	points.setMode(OF_PRIMITIVE_POINTS);

	init(false, 0.02, 4096, 2);

	bInProgress = false;
}
PointCloudAndFileControl::~PointCloudAndFileControl() {
}

void PointCloudAndFileControl::init(bool logFreqSetting, float rmsMinCutoffSetting, 
							int fftBufferSizeSetting, int stftHopRatioSetting) {
	points.clear();
	audioFileIndexLink.clear();
	connectToNextPoint.clear();
	audioFiles.clear();

	nearestIndex = -1;
	analysisIndex = 0;

	rmsMinCutoff = rmsMinCutoffSetting;

	bLogFreq = logFreqSetting;
	updateScales();

	fftBufferSize = fftBufferSizeSetting;
	stftHopRatio = stftHopRatioSetting;
	stftHopSize = fftBufferSize / stftHopRatio;
	fft = ofxFft::create(fftBufferSize, OF_FFT_WINDOW_HAMMING);
}

void PointCloudAndFileControl::beginAnalyse(const vector<ofFile>& fileList, bool logFreqSetting, 
									float rmsMinCutoffSetting, int fftBufferSizeSetting, int stftHopRatioSetting) {

	init(logFreqSetting, rmsMinCutoffSetting, fftBufferSizeSetting, stftHopRatioSetting);

	audioFiles = fileList;
	bInProgress = true;
}

void PointCloudAndFileControl::draw(ofCamera& camera, glm::vec3 mouse, bool drawPointsEnabled, bool listingInProgress, bool pointPickEnabled, bool pointPickSelected) {
	// Draw axes, grid, and labels //
	{
		camera.begin();
		ofDrawBitmapMode(OF_BITMAPMODE_BILLBOARD);

		/*int numSubdivisions = 5;
		ofSetColor(ofColor::darkSlateGray);
		for (int x = 0; x < numSubdivisions; x++) {
			for (int y = 0; y < numSubdivisions; y++) {
				for (int z = 0; z < numSubdivisions; z++) {
					ofDrawLine(x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions,
						(x + 1) * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions);
					ofDrawLine(x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions,
						x * maxSpectralCentroid / numSubdivisions, (y + 1) * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions);
					ofDrawLine(x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, z * maxTimePoint / numSubdivisions,
						x * maxSpectralCentroid / numSubdivisions, y * maxRMSAmplitude / numSubdivisions, (z + 1) * maxTimePoint / numSubdivisions);
				}
			}
		}*/

		ofSetColor(ofColor::darkGray);
		ofNoFill();
		ofDrawLine(0, 0, 0, centroidMax, 0, 0);
		ofDrawLine(0, 0, 0, 0, rmsMax, 0);
		ofDrawLine(0, 0, 0, 0, 0, timeMax);

		ofSetColor(ofColor::white);
		ofDrawBitmapString("Spectral Centroid", centroidMax, 0, 0);
		ofDrawBitmapString("RMS Amplitude", 0, rmsMax, 0);
		ofDrawBitmapString("Time Point", 0, 0, timeMax);
		camera.end();
	}

	// Set Color for Unselected Points //
	int unselectedPointColor;
	if (pointPickSelected) { unselectedPointColor = 60; } //selected
	else if (nearestIndex != -1) { unselectedPointColor = 120; } //hovered
	else { unselectedPointColor = 220; } //default

	// Draw Points //
	if (drawPointsEnabled) {
		ofEnableDepthTest();
		camera.begin();
		ofSetColor(unselectedPointColor);
		points.draw();
		camera.end();
		ofDisableDepthTest();
	}

	// Draw lines connecting points from the same audio file //
	if (drawPointsEnabled && points.getNumVertices() != 0) {
		ofEnableDepthTest();
		camera.begin();
		ofSetColor(unselectedPointColor);
		for (int i = 0; i < (points.getNumVertices() - 1); i++) {
			bool sameFile = audioFileIndexLink[i]
				== audioFileIndexLink[i + 1];
			if (sameFile) {
				if (connectToNextPoint[i]) { ofSetColor(unselectedPointColor); }
				else { ofSetColor( unselectedPointColor, unselectedPointColor / 2, unselectedPointColor / 2 ); }
				ofVec3f point = points.getVertex(i);
				ofVec3f nextPoint = points.getVertex(i + 1);
				ofDrawLine(point, nextPoint);
			}
		}
		camera.end();
		ofDisableDepthTest();
	}

	// Draw Nearest Point //
	if (!listingInProgress && pointPickEnabled && nearestIndex != -1) {
		ofVec3f point = points.getVertex(nearestIndex);

		camera.begin();
		ofSetColor(ofColor::gray);
		ofDrawLine(point, { point.x, point.y, 0 });
		ofDrawLine(point, { 0, point.y, point.z });
		ofDrawLine(point, { point.x, 0, point.z });
		ofDrawLine({ point.x, point.y, 0 }, { point.x, 0, 0 });
		ofDrawLine({ 0, point.y, point.z }, { 0, point.y, 0 });
		ofDrawLine({ point.x, 0, point.z }, { 0, 0, point.z });

		ofSetColor(ofColor::white);
		if (bLogFreq) { ofDrawBitmapString(ofToString(pow(2, ((point.x / centroidScale) - 69.0) / 12.0) * 440.0, 1) + "Hz", point.x, 0, 0); }
		else { ofDrawBitmapString(ofToString(point.x / centroidScale, 1) + "Hz", point.x, 0, 0); }
		ofDrawBitmapString(ofToString(point.y / rmsScale, 3), 0, point.y, 0);
		ofDrawBitmapString(ofToString(point.z / timeScale, 2) + "s", 0, 0, point.z);

		camera.end();

		glm::vec3 filenameScreenPos(20, -20, 0);
		filenameScreenPos += camera.worldToScreen(point);
		ofSetColor(ofColor::white);
		ofDrawBitmapStringHighlight(ofToString(nearestIndex) + ". " + audioFiles[audioFileIndexLink[nearestIndex]].getFileName(), filenameScreenPos);
	}

	// Highlight All Points From Selected Subset //
	if (!listingInProgress && pointPickEnabled && pointPickSelected) {
		bool firstPoint = true;
		ofVec3f previousPoint;
		for (int i = 0; i < pointsSelectedSubset.getNumVertices(); i++) {
			ofVec3f point = pointsSelectedSubset.getVertex(i);
			ofFill();
			ofSetColor(ofColor::snow);
			ofDrawCircle(camera.worldToScreen(point), 3);
			if (firstPoint) { firstPoint = false; }
			else {
				if (connectToNextPoint[selectedSubsetIndexLink[i] - 1]) { ofSetColor(ofColor::linen); }
				if (!connectToNextPoint[selectedSubsetIndexLink[i] - 1]) { ofSetColor(ofColor::tan); }
				ofDrawLine(camera.worldToScreen(previousPoint), camera.worldToScreen(point));
			}
			previousPoint = point;
		}
	}

	// Highlight All Points From Hovered Audio File When a Subset is Not Selected //
	if (!listingInProgress && pointPickEnabled && !pointPickSelected && nearestIndex != -1) {
		int currentFile = audioFileIndexLink[nearestIndex];
		int fileIndex = nearestIndex;
		do {
			fileIndex--;
		} while (fileIndex >= 0 && audioFileIndexLink[fileIndex] == currentFile);
		fileIndex++;
		bool firstPoint = true;
		ofVec3f previousPoint;
		do {
			ofVec3f point = points.getVertex(fileIndex);
			ofFill();
			ofSetColor(ofColor::snow);
			ofDrawCircle(camera.worldToScreen(point), 3);
			if (firstPoint) { firstPoint = false; }
			else {
				if (connectToNextPoint[fileIndex - 1]) { ofSetColor(ofColor::linen); }
				if (!connectToNextPoint[fileIndex - 1]) { ofSetColor(ofColor::tan); }
				ofDrawLine(camera.worldToScreen(previousPoint), camera.worldToScreen(point));
			}
			previousPoint = point;
			fileIndex++;
		} while (fileIndex < points.getNumVertices() && audioFileIndexLink[fileIndex] == currentFile);
	}

	if (bInProgress) {
		ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);

		stringstream ss;
		ss << "Analysing audio files..." << endl;
		ss << "Analysed " << analysisIndex << "/" << audioFiles.size() << endl;

		ofDrawBitmapString(ss.str().c_str(), ofGetWidth() - 200, 20);
	}
}

bool PointCloudAndFileControl::process() {
	if (analysisIndex < audioFiles.size()) {
		analyseOneFile();
		analysisIndex++;
		bInProgress = true;
	}
	else {
		bInProgress = false;
	}

	return bInProgress;
}

void PointCloudAndFileControl::analyseOneFile() {
	currentAudioFile.load(audioFiles[analysisIndex].getAbsolutePath());
	if (!currentAudioFile.loaded()) {
		cout << "Failed to load " << audioFiles[analysisIndex].getFileName() << endl;
		return;
	}

	// deinterleave audio data
	int channelSize = currentAudioFile.length();
	bool singleChannel = currentAudioFile.channels() == 1;
	vector<vector<float>> deinterleavedData;
	if (!singleChannel) {
		deinterleaveAudioData(deinterleavedData, currentAudioFile.data(), currentAudioFile.length(), currentAudioFile.channels());
	}

	// calculate number of frames
	int numFrames = ceil((float)(channelSize - fftBufferSize) / (float)stftHopSize) + 1;

	// check if zero padding is needed for last frame
	bool zeroPad = false;
	if (channelSize % stftHopSize != 0) { zeroPad = true; }

	bool skippedPoint = true;
	float finalTimePoint = 0.0;
	for (int frame = 0; frame < numFrames - zeroPad; frame++) {
		// calculate spectral centroid
		float spectralCentroid = 0.0;
		if (singleChannel) {
			float* framePointer = currentAudioFile.data() + frame * stftHopSize;
			spectralCentroid = spectralCentroidOneFrame(framePointer, currentAudioFile.samplerate(), bLogFreq);
		}
		else {
			for (int channel = 0; channel < currentAudioFile.channels(); channel++) {
				float* framePointer = &deinterleavedData[channel][frame * stftHopSize];
				spectralCentroid += spectralCentroidOneFrame(framePointer, currentAudioFile.samplerate(), bLogFreq);
			}
			spectralCentroid /= currentAudioFile.channels();
		}

		// calculate RMS amplitude
		float rms = 0.0;

		for (int i = 0; i < fftBufferSize * currentAudioFile.channels(); i++) {
			float sample = currentAudioFile.data()[i + frame * stftHopSize];
			rms += sample * sample;
		}

		rms /= (float)(fftBufferSize * currentAudioFile.channels());
		rms = sqrt(rms);

		// calculate time point
		float timePoint = (float)(frame * stftHopSize) / (float)currentAudioFile.samplerate();

		// add point to mesh
		if (rms > rmsMinCutoff) {
			finalTimePoint = timePoint;

			points.addVertex({ spectralCentroid * centroidScale, rms * rmsScale, timePoint * timeScale });
			audioFileIndexLink.push_back(analysisIndex);
			connectToNextPoint.push_back(false);
			//set previous point to connect to this point
			if (!skippedPoint) {
				connectToNextPoint[points.getNumVertices() - 2] = true;
			}
			else { skippedPoint = false; }
		}
		else { skippedPoint = true; }
	}

	// zero pad last frame
	if (zeroPad) {
		if (singleChannel) {
			int lastFrameStart = (numFrames - 1) * stftHopSize;
			int lastFrameSize = channelSize - lastFrameStart;
			float* lastFramePointer = currentAudioFile.data() + lastFrameStart;

			std::vector<float> lastFrame(lastFrameSize);
			memcpy(&lastFrame[0], lastFramePointer, sizeof(float) * lastFrameSize);
			for (int i = lastFrameSize; i < fftBufferSize; i++) {
				lastFrame.push_back(0.0);
			}

			float spectralCentroid = spectralCentroidOneFrame(&lastFrame[0], currentAudioFile.samplerate(), bLogFreq);
			float rms = 0.0;

			for (int i = 0; i < fftBufferSize; i++) {
				float sample = lastFrame[i];
				rms += sample * sample;
			}

			rms /= (float)fftBufferSize;
			rms = sqrt(rms);

			float timePoint = (float)(lastFrameStart) / (float)currentAudioFile.samplerate();
			finalTimePoint = timePoint;

			if (rms > rmsMinCutoff) {
				finalTimePoint = timePoint;

				points.addVertex({ spectralCentroid * centroidScale, rms * rmsScale, timePoint * timeScale });
				audioFileIndexLink.push_back(analysisIndex);
				connectToNextPoint.push_back(false);
				//set previous point to connect to this point
				if (!skippedPoint) {
					connectToNextPoint[points.getNumVertices() - 2] = true;
				}
				else { skippedPoint = false; }
			}
			else { skippedPoint = true; }
		}
		else {
			float timePoint = (float)currentAudioFile.length() / (float)currentAudioFile.samplerate();
			float spectralCentroid = 0.0;
			float rms = 0.0;
			if (rms > rmsMinCutoff) {
				finalTimePoint = timePoint;

				points.addVertex({ spectralCentroid * centroidScale, rms * rmsScale, timePoint * timeScale });
				audioFileIndexLink.push_back(analysisIndex);
				connectToNextPoint.push_back(false);
				//set previous point to connect to this point
				if (!skippedPoint) {
					connectToNextPoint[points.getNumVertices() - 2] = true;
				}
				else { skippedPoint = false; }
			}
			else { skippedPoint = true; }
			// TODO - zero pad last frame for multichannel audio
		}
	}

	if (finalTimePoint * timeScale > timeMax) { timeMax = finalTimePoint * timeScale; }
}

void PointCloudAndFileControl::deinterleaveAudioData(vector<vector<float>>& deinterleavedData, float* interleavedData, 
												int channelSize, int numChannels) {
	deinterleavedData.resize(numChannels);

	int fileSize = channelSize * numChannels;

	for (int i = 0; i < numChannels; i++) {
		deinterleavedData[i].resize(channelSize);
	}

	for (int i = 0; i < fileSize; i++) {
		deinterleavedData[i % numChannels][i / numChannels] = interleavedData[i];
	}
}

float PointCloudAndFileControl::spectralCentroidOneFrame(float* input, float sampleRate, bool logFreq) {
	std::vector<float> ampBins(fft->getBinSize());

	// normalise input
	float maxValue = 0.0;
	for (int i = 0; i < fftBufferSize; i++) {
		if (abs(input[i]) > maxValue) {
			maxValue = abs(input[i]);
		}
	}
	for (int i = 0; i < fftBufferSize; i++) {
		input[i] /= maxValue;
	}

	// do FFT
	fft->setSignal(input);

	float* fftPointer = fft->getAmplitude();
	memcpy(&ampBins[0], fftPointer, sizeof(float) * fft->getBinSize());

	// normalise output
	maxValue = 0.0;
	for (int i = 0; i < fft->getBinSize(); i++) {
		if (ampBins[i] > maxValue) {
			maxValue = ampBins[i];
		}
	}
	for (int i = 0; i < fft->getBinSize(); i++) {
		ampBins[i] /= maxValue;
	}

	int numBins = ampBins.size();
	float binHz = sampleRate / ((numBins - 1) * 2.0);
	float minBin = 0;
	float maxBin = (numBins - 1);

	// skip first bin if log frequency
	if (logFreq)
	{
		minBin = 1;
		ampBins[1] += ampBins[0];
		ampBins.erase(ampBins.begin());
	}

	// calculate frequency bins
	int size = maxBin - minBin;
	std::vector<float> freqBins(size);

	float minFreq = minBin * binHz;
	float maxFreq = maxBin * binHz;
	float step = (maxFreq - minFreq) / (size - 1);

	for (int i = 0; i < size; i++) {
		freqBins[i] = minFreq + i * step;
	}

	// log frequency scale
	if (logFreq) {
		for (int i = 0; i < size; i++) {
			freqBins[i] = 69.0 + (12.0 * log(freqBins[i] / 440.0) * 1.44269504088896340736); //log2E
		}
	}

	// calculate spectral centroid
	float ampSum = 0.0;
	for (int i = 0; i < size; i++) {
		ampSum += ampBins[i];
	}

	float ampFreqSum = 0.0;
	for (int i = 0; i < size; i++) {
		ampFreqSum += ampBins[i] * freqBins[i];
	}

	float centroid = ampFreqSum / ampSum;

	return centroid;

	//// calculate loudest bin
	//float loudestBin = 0.0;
	//int loudestBinIndex = 0;
	//for (int i = 0; i < size; i++) {
	//	if (ampBins[i] > loudestBin) {
	//		loudestBin = ampBins[i];
	//		loudestBinIndex = i;
	//	}
	//}
	//loudestBin = freqBins[loudestBinIndex];

	//return loudestBin;
}

void PointCloudAndFileControl::updateScales() {
	rmsScale = 400.0;
	timeScale = 60.0;
	if (bLogFreq) { centroidScale = -3.0; }
	else { centroidScale = -0.02; }

	rmsMax = 1.0 * rmsScale;
	timeMax = 5.0 * timeScale;
	if (bLogFreq) { centroidMax = 127.0 * centroidScale; }
	else { centroidMax = 20000.0 * centroidScale; }
}

int PointCloudAndFileControl::pointPicker(glm::vec3 mouse, ofCamera camera, bool selected) {
	int pointCount = selected ? pointsSelectedSubset.getNumVertices() : points.getNumVertices();
	float nearestDistance = FLT_MAX;

	for (int i = 0; i < pointCount; i++) {
		glm::vec3 vertex = camera.worldToScreen(selected ? pointsSelectedSubset.getVertex(i) : points.getVertex(i));
		float distance = glm::distance(vertex, mouse);
		if (i == 0 || distance < nearestDistance) {
			nearestDistance = distance;
			nearestIndex = selected ? selectedSubsetIndexLink[i] : i;
		}
	}

	if (nearestDistance > 15) {
		nearestIndex = -1;
	}
	
	return nearestIndex;
}

void PointCloudAndFileControl::soundController(bool pointPickEnabled) {
	for (int i = 0; i < sounds.size(); i++) {
		if (!sounds[i].getIsPlaying())
			sounds.erase(sounds.begin() + i);
	}

	if (nearestIndex == -1)
		lastSoundIndex = 0;

	if (pointPickEnabled && ofGetKeyPressed(' ') && nearestIndex != -1 && nearestIndex != lastSoundIndex)
	{
		sounds.push_back(ofSoundPlayer());
		sounds.back().load(audioFiles[audioFileIndexLink[nearestIndex]].getAbsolutePath());
		sounds.back().setVolume(0.35);
		sounds.back().play();
		lastSoundIndex = nearestIndex;
	}
	ofSoundUpdate();
}

void PointCloudAndFileControl::setPointPickerSelectedSubset(bool& selected) {
	if (selected) {
		selected = false;
		pointsSelectedSubset.clear();
		selectedSubsetIndexLink.clear();
	}
	else if (!selected && nearestIndex != -1) {
		selected = true;
		pointsSelectedSubset.clear();
		selectedSubsetIndexLink.clear();

		int currentFile = audioFileIndexLink[nearestIndex];
		int fileIndex = nearestIndex;
		do {
			fileIndex--;
		} while (fileIndex >= 0 && audioFileIndexLink[fileIndex] == currentFile);
		fileIndex++;
		int startIndex = fileIndex;
		do {
			pointsSelectedSubset.addVertex(points.getVertex(fileIndex));
			fileIndex++;
		} while (fileIndex < points.getNumVertices() && audioFileIndexLink[fileIndex] == currentFile);

		for (int i = 0; i < pointsSelectedSubset.getNumVertices(); i++) {
			selectedSubsetIndexLink.push_back(startIndex + i);
		}
	}
}

glm::vec3 PointCloudAndFileControl::getMaxDimensions() {
	return { centroidMax, rmsMax, timeMax };
}

int PointCloudAndFileControl::getPointCount() {
	return points.getNumVertices();
}

int PointCloudAndFileControl::getFileFromPoint(int index) {
	return audioFileIndexLink[index];
}