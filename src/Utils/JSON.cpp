/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "./JSON.h"
#include <fstream>
#include <iostream>

using namespace Acorex;

bool Utils::JSON::Write ( const std::string& outputFile, const DataSet& dataset )
{
	try
	{
		std::ofstream file ( outputFile );
		nlohmann::json j = dataset;

		file << j.dump ( 0 ) << std::endl;
		file.close ( );
	}
	catch ( std::exception& e )
	{
		std::cerr << "failed to write output to " << outputFile << " : " << e.what ( ) << std::endl;
		return false;
	}

	return true;
}

bool Utils::JSON::Read ( const std::string& inputFile, DataSet& dataset )
{
	try
	{
		std::ifstream file ( inputFile );
		nlohmann::json j;

		file >> j;
		file.close ( );

		dataset = j.template get<DataSet> ( );
	}
	catch ( std::exception& e )
	{
		std::cerr << "failed to read input " << inputFile << " : " << e.what ( ) << std::endl;
		return false;
	}

	return true;
}

bool Utils::JSON::Read ( const std::string& inputFile, AnalysisSettings& settings )
{
	try
	{
		std::ifstream file ( inputFile );
		nlohmann::json j;

		file >> j;
		file.close ( );

		settings = j.template get<AnalysisSettings> ( );
	}
	catch ( std::exception& e )
	{
		std::cerr << "failed to read input " << inputFile << " : " << e.what ( ) << std::endl;
		return false;
	}

	return true;
}


#ifndef DATA_CHANGE_CHECK_1
#error "data structure changed, please update json serialization"
#endif

void Utils::to_json ( nlohmann::json& j, const DataSet& a )
{
	j = nlohmann::json {	
		TO_J ( currentPointCount),
		TO_J ( dimensionNames ),
		TO_J ( fileList ),
		TO_J ( time.raw ),
		TO_J ( stats.raw ),
		TO_J ( stats.reduced ),
		TO_J_SETTINGS ( currentDimensionCount ),
		TO_J_SETTINGS ( hasBeenReduced ),
		TO_J_SETTINGS ( bTime ),
		TO_J_SETTINGS ( bPitch ),
		TO_J_SETTINGS ( bLoudness ),
		TO_J_SETTINGS ( bShape ),
		TO_J_SETTINGS ( bMFCC ),
		TO_J_SETTINGS ( sampleRate ),
		TO_J_SETTINGS ( windowFFTSize ),
		TO_J_SETTINGS ( hopFraction ),
		TO_J_SETTINGS ( nBands ),
		TO_J_SETTINGS ( nCoefs ),
		TO_J_SETTINGS ( minFreq ),
		TO_J_SETTINGS ( maxFreq ) };
}

void Utils::from_json ( const nlohmann::json& j, DataSet& a )
{
	TO_A ( currentPointCount );
	TO_A ( dimensionNames );
	TO_A ( fileList );
	TO_A ( time.raw );
	TO_A ( stats.raw );
	TO_A ( stats.reduced );
	TO_A_SETTINGS ( currentDimensionCount );
	TO_A_SETTINGS ( hasBeenReduced );
	TO_A_SETTINGS ( bTime );
	TO_A_SETTINGS ( bPitch );
	TO_A_SETTINGS ( bLoudness );
	TO_A_SETTINGS ( bShape );
	TO_A_SETTINGS ( bMFCC );
	TO_A_SETTINGS ( sampleRate );
	TO_A_SETTINGS ( windowFFTSize );
	TO_A_SETTINGS ( hopFraction );
	TO_A_SETTINGS ( nBands );
	TO_A_SETTINGS ( nCoefs );
	TO_A_SETTINGS ( minFreq );
	TO_A_SETTINGS ( maxFreq );
}

void Utils::to_json ( nlohmann::json& j, const AnalysisSettings& a )
{
	j = nlohmann::json { 
		TO_J ( currentDimensionCount ),
		TO_J ( hasBeenReduced ),
		TO_J ( bTime ),
		TO_J ( bPitch ),
		TO_J ( bLoudness ),
		TO_J ( bShape ),
		TO_J ( bMFCC ),
		TO_J ( sampleRate ),
		TO_J ( windowFFTSize ),
		TO_J ( hopFraction ),
		TO_J ( nBands ),
		TO_J ( nCoefs ),
		TO_J ( minFreq ),
		TO_J ( maxFreq ) };
}

void Utils::from_json ( const nlohmann::json& j, AnalysisSettings& a )
{ 
	TO_A ( currentDimensionCount );
	TO_A ( hasBeenReduced );
	TO_A ( bTime );
	TO_A ( bPitch );
	TO_A ( bLoudness );
	TO_A ( bShape );
	TO_A ( bMFCC );
	TO_A ( sampleRate );
	TO_A ( windowFFTSize );
	TO_A ( hopFraction );
	TO_A ( nBands );
	TO_A ( nCoefs );
	TO_A ( minFreq );
	TO_A ( maxFreq );
}