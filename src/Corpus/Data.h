#pragma once

#include <string>
#include <vector>

namespace AcorexCorpus {

struct TimeDataSet {
	const bool timeDimension = true;
	bool hasBeenReduced = false;
	int currentDimensionCount = 0;

	std::vector<std::string> dimensionNames; // [dimension]
	std::vector<std::string> fileList; // [file]

	std::vector<std::vector<double>> timePointsSamples; // [file][timepoint]
	std::vector<std::vector<double>> timePointsSeconds; // [file][timepoint]
	std::vector<std::vector<std::vector<double>>> data; // [file][timepoint][dimension]
};

struct StatsDataSet {
	const bool timeDimension = false;
	bool hasBeenReduced = false;
	int currentDimensionCount = 0;

	std::vector<std::string> dimensionNames; // [dimension]
	std::vector<std::string> fileList; // [file]

	std::vector<std::vector<double>> meanData;				// [file][dimension]
	std::vector<std::vector<double>> standardDeviationData; // [file][dimension]
	std::vector<std::vector<double>> skewnessData;			// [file][dimension]
	std::vector<std::vector<double>> kurtosisData;			// [file][dimension]
	std::vector<std::vector<double>> lowerQuartileData;		// [file][dimension]
	std::vector<std::vector<double>> medianData;			// [file][dimension]
	std::vector<std::vector<double>> upperQuartileData;		// [file][dimension]


};

#define DATA_ANALYSIS_SETTINGS_COUNT 10
#define DATA_REDUCTION_SETTINGS_COUNT 2

struct AnalysisSettings {
	bool analysisToggles[4] = { false, false, false, false }; // [pitch, loudness, shape, mfcc]
	int analysisParams[6] = { 1024, 2, 40, 13, 20, 5000 }; // [windowFFTSize, hopFraction, nBands, nCoefs, minFreq, maxFreq]
};

struct ReductionSettings {
	int reductionParams[2] = { 3, 200 }; // [dimensionReductionTarget, maxIterations]
};

} // namespace AcorexCorpus