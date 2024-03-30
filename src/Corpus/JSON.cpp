#pragma once

#include "Corpus/JSON.h"
#include <ofLog.h>
#include <fstream>


bool AcorexCorpus::JSON::Write ( const std::string& outputFile, const AcorexCorpus::DataSet& dataset )
{
	try
	{
		std::ofstream file ( outputFile );
		nlohmann::json j = dataset;

		file << j.dump ( 4 ) << std::endl;
		file.close ( );
	}
	catch ( std::exception& e )
	{
		ofLogError ( "JSON" ) << "failed to write output to " << outputFile << " : " << e.what ( );
		return false;
	}

	return true;
}

bool AcorexCorpus::JSON::Read ( const std::string& inputFile, AcorexCorpus::DataSet& dataset )
{
	try
	{
		std::ifstream file ( inputFile );
		nlohmann::json j;

		file >> j;
		file.close ( );

		dataset = j.template get<AcorexCorpus::DataSet> ( );
	}
	catch ( std::exception& e )
	{
		ofLogError ( "JSON" ) << "failed to read input " << inputFile << " : " << e.what ( );
		return false;
	}

	return true;
}

#ifndef DATA_CHANGE_CHECK_3
#error "data structure changed, please update json serialization"
#endif

void AcorexCorpus::to_json ( nlohmann::json& j, const AcorexCorpus::DataSet& a )
{
	j = nlohmann::json {	TO_J ( isTimeAnalysis ), TO_J ( isReduction ), TO_J ( currentDimensionCount ), TO_J ( dimensionNames ),
							TO_J ( fileList ), TO_J ( timePointsSamples ), TO_J ( timePointsSeconds ), TO_J ( data ),
							TO_J ( meanData ), TO_J ( standardDeviationData ), TO_J ( skewnessData ), TO_J ( kurtosisData ),
							TO_J ( lowerQuartileData ), TO_J ( medianData ), TO_J ( upperQuartileData ),
							TO_J ( analysisSettings.bPitch ), TO_J ( analysisSettings.bLoudness ),
							TO_J ( analysisSettings.bShape ), TO_J ( analysisSettings.bMFCC ),
							TO_J ( analysisSettings.windowFFTSize ), TO_J ( analysisSettings.hopFraction ),
							TO_J ( analysisSettings.nBands ), TO_J ( analysisSettings.nCoefs ),
							TO_J ( analysisSettings.minFreq ), TO_J ( analysisSettings.maxFreq ) };
}

void AcorexCorpus::from_json ( const nlohmann::json& j, AcorexCorpus::DataSet& a )
{
	TO_A ( isTimeAnalysis );
	TO_A ( isReduction );
	TO_A ( currentDimensionCount );
	TO_A ( dimensionNames );
	TO_A ( fileList );
	TO_A ( timePointsSamples );
	TO_A ( timePointsSeconds );
	TO_A ( data );
	TO_A ( meanData );
	TO_A ( standardDeviationData );
	TO_A ( skewnessData );
	TO_A ( kurtosisData );
	TO_A ( lowerQuartileData );
	TO_A ( medianData );
	TO_A ( upperQuartileData );
	TO_A ( analysisSettings.bPitch );
	TO_A ( analysisSettings.bLoudness );
	TO_A ( analysisSettings.bShape );
	TO_A ( analysisSettings.bMFCC );
	TO_A ( analysisSettings.windowFFTSize );
	TO_A ( analysisSettings.hopFraction );
	TO_A ( analysisSettings.nBands );
	TO_A ( analysisSettings.nCoefs );
	TO_A ( analysisSettings.minFreq );
	TO_A ( analysisSettings.maxFreq );
}