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

bool Read ( const std::string& inputFile, AcorexCorpus::AnalysisSettings& settings )
{
	try
	{
		std::ifstream file ( inputFile );
		nlohmann::json j;

		file >> j;
		file.close ( );

		settings = j.template get<AcorexCorpus::AnalysisSettings> ( );
	}
	catch ( std::exception& e )
	{
		ofLogError ( "JSON" ) << "failed to read input " << inputFile << " : " << e.what ( );
		return false;
	}

	return true;
}


#ifndef DATA_CHANGE_CHECK_7
#error "data structure changed, please update json serialization"
#endif

void AcorexCorpus::to_json ( nlohmann::json& j, const AcorexCorpus::DataSet& a )
{
	j = nlohmann::json {	TO_J ( currentDimensionCount ), TO_J ( currentPointCount), TO_J ( dimensionNames ), TO_J ( fileList ),
							TO_J ( time.samples ), TO_J ( time.seconds ), TO_J ( time.raw ), TO_J ( stats.raw ), TO_J ( stats.reduced ),
							TO_J ( analysisSettings.hasBeenReduced ), TO_J ( analysisSettings.bTime ),
							TO_J ( analysisSettings.bPitch ), TO_J ( analysisSettings.bLoudness ),
							TO_J ( analysisSettings.bShape ), TO_J ( analysisSettings.bMFCC ),
							TO_J ( analysisSettings.windowFFTSize ), TO_J ( analysisSettings.hopFraction ),
							TO_J ( analysisSettings.nBands ), TO_J ( analysisSettings.nCoefs ),
							TO_J ( analysisSettings.minFreq ), TO_J ( analysisSettings.maxFreq ) };
}

void AcorexCorpus::from_json ( const nlohmann::json& j, AcorexCorpus::DataSet& a )
{
	TO_A ( currentDimensionCount );
	TO_A ( currentPointCount );
	TO_A ( dimensionNames );
	TO_A ( fileList );
	TO_A ( time.samples );
	TO_A ( time.seconds );
	TO_A ( time.raw );
	TO_A ( stats.raw );
	TO_A ( stats.reduced );
	TO_A ( analysisSettings.hasBeenReduced );
	TO_A ( analysisSettings.bTime );
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

void AcorexCorpus::to_json ( nlohmann::json& j, const AcorexCorpus::AnalysisSettings& a )
{
	j = nlohmann::json { TO_J ( hasBeenReduced ), TO_J ( bTime ), TO_J ( bPitch ), TO_J ( bLoudness ), TO_J ( bShape ), TO_J ( bMFCC ),
							TO_J ( windowFFTSize ), TO_J ( hopFraction ), TO_J ( nBands ), TO_J ( nCoefs ), TO_J ( minFreq ), TO_J ( maxFreq ) };
}

void AcorexCorpus::from_json ( const nlohmann::json& j, AcorexCorpus::AnalysisSettings& a )
{ 
	TO_A ( hasBeenReduced );
	TO_A ( bTime );
	TO_A ( bPitch );
	TO_A ( bLoudness );
	TO_A ( bShape );
	TO_A ( bMFCC );
	TO_A ( windowFFTSize );
	TO_A ( hopFraction );
	TO_A ( nBands );
	TO_A ( nCoefs );
	TO_A ( minFreq );
	TO_A ( maxFreq );
}