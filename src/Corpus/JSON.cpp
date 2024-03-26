#pragma once

#include "Corpus/JSON.h"
#include <ofLog.h>
#include <nlohmann/json.hpp>
#include <fstream>

bool acorex::corpus::JSON::Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, double, 1>& dataset )
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

bool acorex::corpus::JSON::Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, double, 1>& dataset )
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

bool acorex::corpus::JSON::WriteMeta ( const std::string& outputFile, std::vector<corpus::Metadata>& metaset )
{
	std::string metaPath = outputFile;
	ReplaceExtensionToMeta ( metaPath );
	std::ofstream file ( metaPath );
	nlohmann::json j;

	j = nlohmann::json::array ( );

	for ( auto each : metaset )
	{
		switch ( each.type )
		{
		case corpus::Metadata::MetaType::BOOL:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.boolValue } );
			break;
		case corpus::Metadata::MetaType::INT:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.intValue } );
			break;
		case corpus::Metadata::MetaType::DOUBLE:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.doubleValue } );
			break;
		case corpus::Metadata::MetaType::STRING:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.stringValue } );
			break;
		case corpus::Metadata::MetaType::STR_ARRAY:
			j.push_back ( { each.key, mMetaStrings.getStringFromMeta(each.key), each.stringArray } );
			break;
		}
	}

	file << j.dump ( 4 );
	file.close ( );

	std::vector<corpus::Metadata> writeTestSet;
	ReadMeta ( metaPath, writeTestSet, false );

	if ( metaset.size ( ) != writeTestSet.size ( ) )
	{
		ofLogError ( "JSON" ) << "Corpus metadata write failed";
		return false;
	}

	return true;
}

bool acorex::corpus::JSON::ReadMeta ( const std::string& inputFile, std::vector<corpus::Metadata>& metaset, bool loadDefaults )
{
	std::string metaPath = inputFile;
	ReplaceExtensionToMeta ( metaPath );

	if ( loadDefaults )
	{
		std::string defaultFileName = DEFAULT_META_FILE;
		metaPath = ofFilePath::getCurrentWorkingDirectory ( ) + defaultFileName;
	}

	auto inputJSON = fluid::JSONFile ( metaPath, "r" );
	nlohmann::json j = inputJSON.read ( );

	if ( !inputJSON.ok ( ) )
	{
		if ( loadDefaults )
		{
			ofLogError ( "JSON" ) << "failed to read default metadata from " << metaPath;
		}
		else
		{ 
			ofLogError ( "JSON" ) << "failed to read metadata from " << metaPath;
		}
		return false;
	}

	if ( !j.is_array ( ) )
	{
		if ( loadDefaults )
		{
			ofLogError ( "JSON" ) << "failed to read default metadata";
		}
		else
		{
			ofLogError ( "JSON" ) << "Invalid metadata format";
		}
		return false;
	}

	for ( auto each : j )
	{
		if ( each.at ( 2 ).is_boolean ( ) )
		{
			MetaList key = each.at ( 0 );
			bool value = each.at ( 2 );
			metaset.push_back ( corpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_number_integer ( ) )
		{
			MetaList key = each.at ( 0 );
			int value = each.at ( 2 );
			metaset.push_back ( corpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_number_float ( ) )
		{
			MetaList key = each.at ( 0 );
			double value = each.at ( 2 );
			metaset.push_back ( corpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_string ( ) )
		{
			MetaList key = each.at ( 0 );
			std::string value = each.at ( 2 );
			metaset.push_back ( corpus::Metadata ( key, value ) );
		}
		else if ( each.at ( 2 ).is_array ( ) )
		{
			MetaList key = each.at ( 0 );
			std::vector<std::string> value = each.at ( 2 );
			metaset.push_back ( corpus::Metadata ( key, value ) );
		}
		else
		{
			if ( loadDefaults )
			{
				ofLogError ( "JSON" ) << "failed to read default metadata";
			}
			else
			{
				ofLogError ( "JSON" ) << "Invalid JSON format at line " << each;
			}
			return false;
		}
	}

	return true;
}

void acorex::corpus::JSON::ReplaceExtensionToMeta ( std::string& path )
{
	path.replace ( path.find ( ".json" ), 5, ".meta" );
}
