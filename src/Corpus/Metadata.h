#pragma once

#include <string>
#include <vector>

#include <cassert>

namespace AcorexCorpus {

struct MetaSetStruct {

#define META_SET_SIZE_18
#define META_SET_FIELD_COUNT 18
#define META_SET_MAX_INDEX 17

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
		MINDEX_dimensionNames = 16,
		MINDEX_fileList = 17
	};

	enum MetaTypes {
		MTYPE_BOOL = 0,
		MTYPE_INT = 1,
		MTYPE_STRING_LIST = 2
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

	std::vector<std::string> dimensionNames = { };
	std::vector<std::string> fileList = { };


	void SetByIndex ( int index, bool value )
	{
		SetAnyByIndex ( index, 0, value, 0, std::vector<std::string> ( ) );
	}

	void SetByIndex ( int index, int value )
	{
		SetAnyByIndex ( index, 1, false, value, std::vector<std::string> ( ) );
	}

	void SetByIndex ( int index, const std::vector<std::string>& value )
	{
		SetAnyByIndex ( index, 2, false, 0, value );
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
		case MINDEX_dimensionNames:					assert ( type = MTYPE_STRING_LIST ); dimensionNames = sValue;		break;
		case MINDEX_fileList:						assert ( type = MTYPE_STRING_LIST ); fileList = sValue;				break;
		default:									assert ( false );													break;
		}
	}

#ifndef META_SET_SIZE_18
#error "META_SET_SIZE_18 not defined"
#endif
};

} // namespace AcorexCorpus