#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

#define DATA_CHANGE_CHECK_3

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
	std::vector<std::vector<std::vector<double>>> data; // [file][timepoint][dimension]

	std::vector<std::vector<double>> meanData;				// [file][dimension]
	std::vector<std::vector<double>> standardDeviationData; // [file][dimension]
	std::vector<std::vector<double>> skewnessData;			// [file][dimension]
	std::vector<std::vector<double>> kurtosisData;			// [file][dimension]
	std::vector<std::vector<double>> lowerQuartileData;		// [file][dimension]
	std::vector<std::vector<double>> medianData;			// [file][dimension]
	std::vector<std::vector<double>> upperQuartileData;		// [file][dimension]

	AnalysisSettings analysisSettings;
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