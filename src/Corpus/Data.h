#pragma once

#include <string>
#include <vector>

#include <cassert>

namespace AcorexCorpus {

struct TimeDataSet {
	const bool timeDimension = true;

	std::vector<std::string> dimensionNames; // [dimension]
	std::vector<std::string> fileList; // [file]

	std::vector<std::vector<double>> timePointsSamples; // [file][timepoint]
	std::vector<std::vector<double>> timePointsSeconds; // [file][timepoint]
	std::vector<std::vector<std::vector<double>>> data; // [file][timepoint][dimension]
};

struct StatsDataSet {
	const bool timeDimension = false;

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

struct MetaSet {

#define META_SET_SIZE_16
#define META_SET_FIELD_COUNT 16
#define META_SET_MAX_INDEX 15

	enum MetaIndex {
		MINDEX_hasBeenReduced = 0,
		MINDEX_insertionReplacesDuplicates = 1,
		MINDEX_timeDimension = 2,
		MINDEX_analysisPitch = 3,
		MINDEX_analysisLoudness = 4,
		MINDEX_analysisShape = 5,
		MINDEX_analysisMFCC = 6,
		MINDEX_windowFFTSize = 7,
		MINDEX_hopFraction = 8,
		MINDEX_nBands = 9,
		MINDEX_nCoefs = 10,
		MINDEX_minFreq = 11,
		MINDEX_maxFreq = 12,
		MINDEX_currentDimensionCount = 13,
		MINDEX_dimensionReductionTarget = 14,
		MINDEX_maxIterations = 15,
	};

	enum MetaTypes {
		MTYPE_BOOL = 0,
		MTYPE_INT = 1,
	};

	bool hasBeenReduced = false;
	bool insertionReplacesDuplicates = false;
	bool timeDimension = false;
	bool analysisPitch = false;
	bool analysisLoudness = false;
	bool analysisShape = false;
	bool analysisMFCC = false;

	int windowFFTSize = 1024;
	int hopFraction = 2;
	int nBands = 40;
	int nCoefs = 13;
	int minFreq = 20;
	int maxFreq = 5000;
	int currentDimensionCount = 0;
	int dimensionReductionTarget = 3;
	int maxIterations = 200;

	void SetByIndex ( int index, bool value )
	{
		SetAnyByIndex ( index, 0, value, 0, std::vector<std::string> ( ) );
	}

	void SetByIndex ( int index, int value )
	{
		SetAnyByIndex ( index, 1, false, value, std::vector<std::string> ( ) );
	}

private:
	void SetAnyByIndex ( int index, int type, bool bValue, int iValue, const std::vector<std::string> sValue )
	{
		switch ( index )
		{
		case MINDEX_hasBeenReduced:					assert ( type = MTYPE_BOOL ); hasBeenReduced = bValue;				break;
		case MINDEX_insertionReplacesDuplicates:	assert ( type = MTYPE_BOOL ); insertionReplacesDuplicates = bValue;	break;
		case MINDEX_timeDimension:					assert ( type = MTYPE_BOOL ); timeDimension = bValue;				break;
		case MINDEX_analysisPitch:					assert ( type = MTYPE_BOOL ); analysisPitch = bValue;				break;
		case MINDEX_analysisLoudness:				assert ( type = MTYPE_BOOL ); analysisLoudness = bValue;			break;
		case MINDEX_analysisShape:					assert ( type = MTYPE_BOOL ); analysisShape = bValue;				break;
		case MINDEX_analysisMFCC:					assert ( type = MTYPE_BOOL ); analysisMFCC = bValue;				break;
		case MINDEX_windowFFTSize:					assert ( type = MTYPE_INT ); windowFFTSize = iValue;				break;
		case MINDEX_hopFraction:					assert ( type = MTYPE_INT ); hopFraction = iValue;					break;
		case MINDEX_nBands:							assert ( type = MTYPE_INT ); nBands = iValue;						break;
		case MINDEX_nCoefs:							assert ( type = MTYPE_INT ); nCoefs = iValue;						break;
		case MINDEX_minFreq:						assert ( type = MTYPE_INT ); minFreq = iValue;						break;
		case MINDEX_maxFreq:						assert ( type = MTYPE_INT ); maxFreq = iValue;						break;
		case MINDEX_currentDimensionCount:			assert ( type = MTYPE_INT ); currentDimensionCount = iValue;		break;
		case MINDEX_dimensionReductionTarget:		assert ( type = MTYPE_INT ); dimensionReductionTarget = iValue;		break;
		case MINDEX_maxIterations:					assert ( type = MTYPE_INT ); maxIterations = iValue;				break;
		default:									assert ( false );													break;
		}
	}

#ifndef META_SET_SIZE_16
#error "META_SET_SIZE_16 not defined"
#endif
};

} // namespace AcorexCorpus