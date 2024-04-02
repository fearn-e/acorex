#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#define DATA_CHANGE_CHECK_8

#define DATA_NUM_STATS 7

namespace AcorexCorpus {

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
	std::vector<std::string> fileList; // [file]

	AcorexCorpus::TimeData time;

	AcorexCorpus::StatsData stats;

	AcorexCorpus::AnalysisSettings analysisSettings;
};

} // namespace AcorexCorpus