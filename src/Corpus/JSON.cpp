#pragma once

#include "Corpus/JSON.h"
#include <ofLog.h>
#include <nlohmann/json.hpp>
#include <fstream>

bool AcorexCorpus::JSON::Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, double, 1>& dataset )
{
	auto outputJSON = fluid::JSONFile ( outputFile, "w" );
	outputJSON.write ( dataset );

	if ( !outputJSON.ok ( ) )
	{
		ofLogError ( "JSON" ) << "failed to write output to " << outputFile;
		return false;
	}

	return true;
}

bool AcorexCorpus::JSON::Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, double, 1>& dataset )
{
	auto inputJSON = fluid::JSONFile ( inputFile, "r" );
	nlohmann::json j = inputJSON.read ( );

	if ( !inputJSON.ok ( ) )
	{
		ofLogError ( "JSON" ) << "failed to read input " << inputFile;
		return false;
	}

	if ( !check_json ( j, dataset ) )
	{
		ofLogError ( "JSON" ) << "Invalid JSON format";
		return false;
	}

	dataset = j.get<fluid::FluidDataSet<std::string, double, 1>> ( );

	return true;
}

bool AcorexCorpus::JSON::WriteMeta ( const std::string& outputFile, const AcorexCorpus::MetaSetStruct& metaset )
{
	std::string metaPath = outputFile;
	bool success = ReplaceExtensionToMeta ( metaPath );

	if ( !success )
	{
		ofLogError ( "JSON" ) << "Failed to replace extension";
		return false;
	}

	std::ofstream file ( metaPath );
	nlohmann::json j;

	j = nlohmann::json::array ( );

#ifndef META_SET_SIZE_18
#error "incorrect number of metadata entries"
#endif

#define STRINGIFY( x ) #x, x
	j.push_back ( { 0, STRINGIFY ( metaset.isReduction ) } );
	j.push_back ( { 1, STRINGIFY ( metaset.insertionReplacesDuplicates ) } );
	j.push_back ( { 2, STRINGIFY ( metaset.timeDimension ) } );
	j.push_back ( { 3, STRINGIFY ( metaset.analysisPitch ) } );
	j.push_back ( { 4, STRINGIFY ( metaset.analysisLoudness ) } );
	j.push_back ( { 5, STRINGIFY ( metaset.analysisShape ) } );
	j.push_back ( { 6, STRINGIFY ( metaset.analysisMFCC ) } );
	j.push_back ( { 7, STRINGIFY ( metaset.windowFFTSize ) } );
	j.push_back ( { 8, STRINGIFY ( metaset.hopFraction ) } );
	j.push_back ( { 9, STRINGIFY ( metaset.nBands ) } );
	j.push_back ( { 10, STRINGIFY ( metaset.nCoefs ) } );
	j.push_back ( { 11, STRINGIFY ( metaset.minFreq ) } );
	j.push_back ( { 12, STRINGIFY ( metaset.maxFreq ) } );
	j.push_back ( { 13, STRINGIFY ( metaset.currentDimensionCount ) } );
	j.push_back ( { 14, STRINGIFY ( metaset.dimensionReductionTarget ) } );
	j.push_back ( { 15, STRINGIFY ( metaset.maxIterations ) } );
	j.push_back ( { 16, STRINGIFY ( metaset.dimensionNames ) } );
	j.push_back ( { 17, STRINGIFY ( metaset.fileList ) } );
#undef STRINGIFY

	file << j.dump ( 4 );
	file.close ( );

	AcorexCorpus::MetaSetStruct writeTestSet;
	bool success = ReadMeta ( outputFile, writeTestSet, true );

	if ( !success )
	{
		ofLogError ( "JSON" ) << "Corpus metadata write failed";
		return false;
	}

	return true;
}

bool AcorexCorpus::JSON::ReadMeta ( const std::string& inputFile, AcorexCorpus::MetaSetStruct& metaset, bool test )
{
	std::string metaPath = inputFile;
	bool success = ReplaceExtensionToMeta ( metaPath );

	if ( !success )
	{
		ofLogError ( "JSON" ) << "Failed to replace extension";
		return false;
	}

	auto inputJSON = fluid::JSONFile ( metaPath, "r" );
	nlohmann::json j = inputJSON.read ( );

	if ( !inputJSON.ok ( ) )
	{
		ofLogError ( "JSON" ) << "failed to read metadata from " << metaPath;
		return false;
	}

	if ( !j.is_array ( ) )
	{
		ofLogError ( "JSON" ) << "Invalid metadata format";
		return false;
	}

	int count = 0;
	for ( auto each : j )
	{
		if ( each.at ( 0 ) > META_SET_MAX_INDEX )
		{
			ofLogWarning ( "JSON" ) << "Invalid metadata index at line " << each;
			continue;
		}

		if ( each.at ( 2 ).is_boolean ( ) )
		{
			metaset.SetByIndex ( each.at ( 0 ), each.at ( 2 ).get<bool> ( ) );
			count++;
		}
		else if ( each.at ( 2 ).is_number_integer ( ) )
		{
			metaset.SetByIndex ( each.at ( 0 ), each.at ( 2 ).get<int> ( ) );
			count++;
		}
		else if ( each.at ( 2 ).is_array ( ) )
		{
			metaset.SetByIndex ( each.at ( 0 ), each.at ( 2 ).get<std::vector<std::string>> ( ) );
			count++;
		}
		else
		{
			ofLogError ( "JSON" ) << "Invalid JSON format at line " << each;
			return false;
		}
	}

	if ( count < META_SET_FIELD_COUNT )
	{
		ofLogWarning ( "JSON" ) << "Metadata count is less than expected";
	}
	else if ( count > META_SET_FIELD_COUNT )
	{
		ofLogWarning ( "JSON" ) << "Metadata count is more than expected";
	}

	if ( test && count != META_SET_FIELD_COUNT )
	{
		return false;
	}

	return true;
}

bool AcorexCorpus::JSON::ReplaceExtensionToMeta ( std::string& path )
{
	if ( path.find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "JSON" ) << "Invalid file extension";
		return false;
	}

	path.replace ( path.find_last_of ( '.' ), path.length ( ), ".meta" );

	return true;
}
