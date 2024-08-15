#pragma once

#include <ofSoundBuffer.h>
#include <string>
#include <vector>
#include <queue>
#include <nlohmann/json.hpp>

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

struct AudioPlayhead {
	AudioPlayhead ( size_t ID, size_t file, size_t sample ) : playheadID ( ID ), fileIndex ( file ), sampleIndex ( sample ) { }

	size_t playheadID = 0;

	size_t fileIndex = 0;
	size_t sampleIndex = 0;
	
	std::queue<size_t> triggerSamplePoints;
};

struct VisualPlayhead {
	VisualPlayhead ( size_t ID, size_t file, size_t sample ) : playheadID ( ID ), fileIndex ( file ), sampleIndex ( sample ) { }

	size_t playheadID = 0;

	size_t fileIndex = 0;
	size_t sampleIndex = 0;

	float position[3] = { 0.0, 0.0, 0.0 };
};

} // namespace Utils
} // namespace Acorex