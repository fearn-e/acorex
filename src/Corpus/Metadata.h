#pragma once

#include <string>

namespace acorex {
namespace corpus {

#define META_LIST_SIZE 14

enum MetaList {
	META_HAS_BEEN_REDUCED,
	META_TIME_DIMENSION,
	META_ANALYSIS_PITCH,
	META_ANALYSIS_LOUDNESS,
	META_ANALYSIS_SHAPE,
	META_ANALYSIS_MFCC,
	META_WINDOW_FFT_SIZE,
	META_HOP_FRACTION,
	META_N_BANDS,
	META_N_COEFS,
	META_MIN_FREQ,
	META_MAX_FREQ,
	META_REDUCED_DIMENSIONS,
	META_MAX_ITERATIONS,
	META_INSERTION_REPLACES_DUPLICATES
};

class MetaStrings {
public:
	std::string getStringFromMeta ( MetaList meta )
	{
		switch ( meta )
		{
		case META_HAS_BEEN_REDUCED: return "hasBeenReduced";
		case META_TIME_DIMENSION: return "timeDimension";
		case META_ANALYSIS_PITCH: return "analysisPitch";
		case META_ANALYSIS_LOUDNESS: return "analysisLoudness";
		case META_ANALYSIS_SHAPE: return "analysisShape";
		case META_ANALYSIS_MFCC: return "analysisMFCC";
		case META_WINDOW_FFT_SIZE: return "windowFFTSize";
		case META_HOP_FRACTION: return "hopFraction";
		case META_N_BANDS: return "nBands";
		case META_N_COEFS: return "nCoefs";
		case META_MIN_FREQ: return "minFreq";
		case META_MAX_FREQ: return "maxFreq";
		case META_REDUCED_DIMENSIONS: return "reducedDimensions";
		case META_MAX_ITERATIONS: return "maxIterations";
		case META_INSERTION_REPLACES_DUPLICATES: return "insertionReplacesDuplicates";
		default: return "";
		}
	}
};

struct Metadata {
	enum class MetaType { BOOL, INT, DOUBLE, STRING };

	Metadata ( MetaList key, bool value ) :					key ( key ), type ( MetaType::BOOL ), boolValue ( value ),
															intValue ( 0 ), doubleValue ( 0.0f ), stringValue ( "" ) { }
	Metadata ( MetaList key, int value ) :					key ( key ), type ( MetaType::INT ), intValue ( value ),
															boolValue ( false ), doubleValue ( 0.0f ), stringValue ( "" ) { }
	Metadata ( MetaList key, double value ) :				key ( key ), type ( MetaType::DOUBLE ), doubleValue ( value ),
															boolValue ( false ), intValue ( 0 ), stringValue ( "" ) { }
	Metadata ( MetaList key, std::string value ) :			key ( key ), type ( MetaType::STRING ), stringValue ( value ),
															boolValue ( false ), intValue ( 0 ), doubleValue ( 0.0f ) { }

	MetaList key;
	MetaType type;

	bool boolValue;
	int intValue;
	double doubleValue;
	std::string stringValue;
};

} // namespace corpus
} // namespace acorex