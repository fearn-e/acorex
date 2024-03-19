#pragma once

#include "Corpus/JSON.h"
#include <ofLog.h>
#include <nlohmann/json.hpp>

bool corpus::JSON::Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, double, 1>& dataset )
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

bool corpus::JSON::Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, double, 1>& dataset )
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