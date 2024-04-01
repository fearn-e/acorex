#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#define DATA_CHANGE_CHECK_7

#define DATA_NUM_STATS 7

namespace AcorexCorpus {

struct DataSet {
	int currentDimensionCount;
	int currentPointCount;

	std::vector<std::string> dimensionNames; // [dimension]
	std::vector<std::string> fileList; // [file]

	TimeData time;

	StatsData stats;

	AnalysisSettings analysisSettings;
};

struct TimeData {
	std::vector<std::vector<double>> samples; // [file][timepoint]
	std::vector<std::vector<double>> seconds; // [file][timepoint]

	std::vector<std::vector<std::vector<double>>> raw; // [file][timepoint][dimension]
};

struct StatsData {
	std::vector<std::vector<std::vector<double>>> raw; // [file][dimension][statistic] (mean, stdDev, skewness, kurtosis, loPercent, midPercent, hiPercent)
	std::vector<std::vector<double>> reduced; // [file][dimension]
};

struct AnalysisSettings {
	bool hasBeenReduced;
	bool bTime;
	bool bPitch;
	bool bLoudness;
	bool bShape;
	bool bMFCC;
	int windowFFTSize;
	int hopFraction;
	int nBands;
	int nCoefs;
	int minFreq;
	int maxFreq;
};

struct ReductionSettings {
	int dimensionReductionTarget;
	int maxIterations;
};

} // namespace AcorexCorpus