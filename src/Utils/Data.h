/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <ofSoundBuffer.h>
#include <string>
#include <vector>
#include <queue>
#include <nlohmann/json.hpp>
#include <ofColor.h>
#include <ofRectangle.h>

#define DATA_CHANGE_CHECK_1

#define DATA_NUM_STATS 7

namespace Acorex {
namespace Utils {

enum class Axis : int {
	X = 0,
	Y = 1,
	Z = 2,
	COLOR = 3,
	NONE = 4,
	MULTIPLE = 5
};

struct AudioData {
	std::vector<bool> loaded; // [file]
	std::vector<ofSoundBuffer> raw; // [file]
};

struct TimeData {
	std::vector<std::vector<std::vector<double>>> raw; // [file][timepoint][dimension] (first dimension is always time)
};

struct StatsData {
	std::vector<std::vector<std::vector<double>>> raw; // [file][dimension][statistic] (mean, stdDev, skewness, kurtosis, loPercent, midPercent, hiPercent)
	std::vector<std::vector<double>> reduced; // [file][dimension]
};

struct AnalysisSettings {
	int currentDimensionCount = 0;
	bool hasBeenReduced = false;
	bool bTime = false;
	bool bPitch = false;
	bool bLoudness = false;
	bool bShape = false;
	bool bMFCC = false;
	int sampleRate = 44100;
	int windowFFTSize = 1024;
	int hopFraction = 2;
	int nBands = 40;
	int nCoefs = 13;
	int minFreq = 20;
	int maxFreq = 5000;
};

struct ReductionSettings {
	int dimensionReductionTarget = 3;
	int maxIterations = 200;
};

struct DataSet {
	int currentPointCount = 0;

	std::vector<std::string> dimensionNames; // [dimension]
	std::vector<std::string> statisticNames = { "Mean Average", "Standard Deviation", "Skewness", "Kurtosis", "Low Quartile", "Median", "High Quartile" }; // [statistic]
	std::vector<std::string> fileList; // [file]

	AudioData audio;

	TimeData time;

	StatsData stats;

	AnalysisSettings analysisSettings;

};

struct PointFT {
	size_t file = 0;
	size_t time = 0;
};

struct AudioPlayhead {
	AudioPlayhead ( size_t ID, size_t file, size_t sample ) : playheadID ( ID ), fileIndex ( file ), sampleIndex ( sample ) { }

	size_t playheadID = 0;

	size_t fileIndex = 0;
	size_t sampleIndex = 0;

	bool crossfading = false;
	size_t jumpFileIndex = 0;
	size_t jumpSampleIndex = 0;
	size_t crossfadeCurrentSample = 0;
	size_t crossfadeSampleLength = 0;
	
	std::queue<size_t> triggerSamplePoints;
};

struct VisualPlayhead {
	VisualPlayhead ( size_t ID, size_t file, size_t sample ) : playheadID ( ID ), fileIndex ( file ), sampleIndex ( sample ) { }

	bool highlight = false;

	size_t playheadID = 0;

	size_t fileIndex = 0;
	size_t sampleIndex = 0;

	float position[3] = { 0.0, 0.0, 0.0 };

	ofColor color = ofColor ( 255, 255, 255, 255 );
	ofRectangle panelRect = ofRectangle ( 0, 0, 0, 0 );
};

} // namespace Utils
} // namespace Acorex