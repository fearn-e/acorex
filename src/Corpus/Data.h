#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#define DATA_CHANGE_CHECK_4

namespace AcorexCorpus {

struct DataSet {
	bool isTimeAnalysis;
	bool isReduction;
	int currentDimensionCount;
	int currentPointCount;

	std::vector<std::string> dimensionNames; // [dimension]
	std::vector<std::string> fileList; // [file]

	std::vector<std::vector<double>> timePointsSamples; // [file][timepoint]
	std::vector<std::vector<double>> timePointsSeconds; // [file][timepoint]

	std::vector<std::vector<std::vector<double>>> tData; // [file][timepoint][dimension]

	StatsData sData;

	AnalysisSettings analysisSettings;
};

struct StatsData {
	std::vector<std::vector<double>> mean;			// [file][dimension]
	std::vector<std::vector<double>> stdDev;		// [file][dimension]
	std::vector<std::vector<double>> skewness;		// [file][dimension]
	std::vector<std::vector<double>> kurtosis;		// [file][dimension]
	std::vector<std::vector<double>> loPercent;		// [file][dimension]
	std::vector<std::vector<double>> midPercent;	// [file][dimension]
	std::vector<std::vector<double>> hiPercent;		// [file][dimension]
};

struct AnalysisSettings {
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