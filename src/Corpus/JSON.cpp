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

bool AcorexCorpus::JSON::WriteMeta ( const std::string& outputFile, std::vector<AcorexCorpus::Metadata>& metaset )
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

	for ( auto each : metaset )
	{
		switch ( each.type )
		{
		case AcorexCorpus::Metadata::MetaType::BOOL:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.boolValue } );
			break;
		case AcorexCorpus::Metadata::MetaType::INT:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.intValue } );
			break;
		case AcorexCorpus::Metadata::MetaType::DOUBLE:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.doubleValue } );
			break;
		case AcorexCorpus::Metadata::MetaType::STRING:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.stringValue } );
			break;
		case AcorexCorpus::Metadata::MetaType::STR_ARRAY:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.stringArray } );
			break;
		}
	}

	file << j.dump ( 4 );
	file.close ( );

	std::vector<AcorexCorpus::Metadata> writeTestSet;
	ReadMeta ( metaPath, writeTestSet );

	if ( metaset.size ( ) != writeTestSet.size ( ) )
	{
		ofLogError ( "JSON" ) << "Corpus metadata write failed";
		return false;
	}

	return true;
}

bool AcorexCorpus::JSON::ReadMeta ( const std::string& inputFile, std::vector<AcorexCorpus::Metadata>& metaset )
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

	for ( auto each : j )
	{
		if ( each.at ( 2 ).is_boolean ( ) )
		{
			MetaList key = each.at ( 0 );
			bool value = each.at ( 2 );
			metaset.push_back ( AcorexCorpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_number_integer ( ) )
		{
			MetaList key = each.at ( 0 );
			int value = each.at ( 2 );
			metaset.push_back ( AcorexCorpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_number_float ( ) )
		{
			MetaList key = each.at ( 0 );
			double value = each.at ( 2 );
			metaset.push_back ( AcorexCorpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_string ( ) )
		{
			MetaList key = each.at ( 0 );
			std::string value = each.at ( 2 );
			metaset.push_back ( AcorexCorpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_array ( ) )
		{
			MetaList key = each.at ( 0 );
			std::vector<std::string> value = each.at ( 2 );
			metaset.push_back ( AcorexCorpus::Metadata ( key, value ) );
		}
		else
		{
			ofLogError ( "JSON" ) << "Invalid JSON format at line " << each;
			return false;
		}
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
