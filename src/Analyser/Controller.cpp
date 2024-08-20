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

#include "./Controller.h"
#include <ofLog.h>
#include <filesystem>

using namespace Acorex;

// Public --------------------------------------------------------------------

bool Analyser::Controller::CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const Utils::AnalysisSettings& settings )
{
	bool success;

	Utils::DataSet dataset;

	dataset.analysisSettings = settings;

	success = SearchDirectory ( inputPath, dataset.fileList );
	if ( !success ) { return false; }
	
	int filesIn = dataset.fileList.size ( );
	int numAnalysed = mGenAnalysis.ProcessFiles ( dataset );
	if ( numAnalysed > 0 )
	{
		ofLogNotice ( "Controller" ) << "Processed " << filesIn << " files into " << dataset.currentPointCount 
			<< " points, with " << dataset.fileList.size ( ) - numAnalysed << " files failed.";
	}
	else 
	{ 
		ofLogError ( "Controller" ) << "Failed to process any files.";
		return false;
	}

	GenerateDimensionNames ( dataset.dimensionNames, settings );

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }
	
	return true;
}

bool Analyser::Controller::ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const Utils::ReductionSettings& settings )
{
	bool success;

	Utils::DataSet dataset;

	success = mJSON.Read ( inputPath, dataset );
	if ( !success ) { return false; }

	success = mUMAP.Fit ( dataset, settings );
	if ( !success ) { return false; }

	dataset.analysisSettings.currentDimensionCount = settings.dimensionReductionTarget + 1;
	GenerateDimensionNames ( dataset.dimensionNames, settings, dataset.analysisSettings.bTime );

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }

	return true;
}

bool Analyser::Controller::InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const bool newReplacesExisting )
{
	bool success;

	Utils::DataSet existingDataset;
	success = mJSON.Read ( outputPath, existingDataset );
	if ( !success ) { return false; }

	std::vector<std::string> newFiles;
	success = SearchDirectory ( inputPath, newFiles );
	if ( !success ) { return false; }

	// remove new files that already exist if duplicates are not to be analysed again
	if ( !newReplacesExisting )
	{
		int preTreated = newFiles.size ( );

		std::vector<std::string> newFilesTreated;

		for ( auto eachNew : newFiles )
		{
			bool exists = false;
			for ( auto eachExisting : existingDataset.fileList )
			{
				if ( eachNew == eachExisting )
				{
					exists = true;
					break;
				}
			}

			if ( exists ) { continue; }

			newFilesTreated.push_back ( eachNew );
		}

		if ( newFilesTreated.empty ( ) )
		{
			ofLogError ( "Controller" ) << "No new files left to process.";
			return false;
		}

		newFiles.clear ( );
		newFiles = newFilesTreated;

		ofLogNotice ( "Controller" ) << newFiles.size ( ) << " new files left to process, with " << preTreated - newFiles.size ( ) << " duplicates removed.";
	}

	Utils::DataSet newDataset;
	newDataset.fileList = newFiles;
	newDataset.analysisSettings = existingDataset.analysisSettings;
#ifndef DATA_CHANGE_CHECK_1
#error "check if this is still valid with dataset structure"
#endif

	int filesIn = newDataset.fileList.size ( );
	int numAnalysed = mGenAnalysis.ProcessFiles ( newDataset );
	if ( numAnalysed > 0 )
	{
		ofLogNotice ( "Controller" ) << "Processed " << filesIn << " files into " << newDataset.currentPointCount
			<< " points, with " << newDataset.fileList.size ( ) - numAnalysed << " files failed.";
	}
	else
	{
		ofLogError ( "Controller" ) << "Failed to process any files.";
		return false;
	}

	std::vector<int> mergeInfo = MergeDatasets ( existingDataset, newDataset, newReplacesExisting );

	if ( newReplacesExisting )
	{
		ofLogNotice ( "Controller" ) << "Replaced existing dataset with new files, with " << mergeInfo[1] << " not previously existing added and " << mergeInfo[2] << " overwriting existing.";
	}
	else
	{
		ofLogNotice ( "Controller" ) << "Merged new files into dataset, with " << mergeInfo[0] << " already existing skipped and " << mergeInfo[1] << " not previously existing added.";
	}

	success = mJSON.Write ( outputPath, existingDataset );
	if ( !success ) { return false; }

	return true;
}

// Private -------------------------------------------------------------------

std::vector<int> Analyser::Controller::MergeDatasets ( Utils::DataSet& primaryDataset, const Utils::DataSet& additionalDataset, const bool additionalReplacesPrimary )
{
	int filesSkipped = 0;
	int filesAdded = 0;
	int filesOverwritten = 0;

	for ( int i = 0; i < additionalDataset.fileList.size ( ); i++ )
	{
		bool exists = false;
		int existingIndex = -1;
		for ( int j = 0; j < primaryDataset.fileList.size ( ); j++ )
		{
			if ( additionalDataset.fileList[i] == primaryDataset.fileList[j] )
			{
				exists = true;
				existingIndex = j;
				break;
			}
		}

		if ( exists && !additionalReplacesPrimary )
		{ 
			// Skip
			filesSkipped++;
			continue;
		}

		if ( exists && additionalReplacesPrimary )
		{
			// Overwrite
			filesOverwritten++;
			int pointCountDiff = 0;
			primaryDataset.fileList[existingIndex] = additionalDataset.fileList[i];

			if ( primaryDataset.analysisSettings.bTime )
			{ // Time
				pointCountDiff = additionalDataset.time.raw[i].size ( ) - primaryDataset.time.raw[existingIndex].size ( ); // TODO - DOUBLE CHECK THIS
				primaryDataset.time.raw[existingIndex] = additionalDataset.time.raw[i];
			}
			else
			{ // Stats
				pointCountDiff = 0;
				primaryDataset.stats.raw[existingIndex] = additionalDataset.stats.raw[i];
			}

			primaryDataset.currentPointCount += pointCountDiff;

			continue;
		}

		if ( !exists )
		{
			// Add
			filesAdded++;
			int pointCountDiff = 0;
			primaryDataset.fileList.push_back ( additionalDataset.fileList[i] );

			if ( primaryDataset.analysisSettings.bTime )
			{ // Time
				pointCountDiff = additionalDataset.time.raw[i].size ( ); // TODO - DOUBLE CHECK THIS
				primaryDataset.time.raw.push_back ( additionalDataset.time.raw[i] );
			}
			else
			{ // Stats
				pointCountDiff = 1;
				primaryDataset.stats.raw.push_back ( additionalDataset.stats.raw[i] );
			}

			primaryDataset.currentPointCount += pointCountDiff;

			continue;
		}
	}

	return std::vector<int> { filesSkipped, filesAdded, filesOverwritten };
}

bool Analyser::Controller::SearchDirectory ( const std::string& directory, std::vector<std::string>& files )
{
	files.clear ( );

	using namespace std::filesystem;
	for ( const auto& entry : recursive_directory_iterator ( directory ) )
	{
		if ( is_directory ( entry.path ( ) ) ) { continue; }

		if ( !is_regular_file ( entry.path ( ) ) ) { continue; }

		if ( entry.path ( ).extension ( ) != ".wav" &&
			entry.path ( ).extension ( ) != ".flac" &&
			entry.path ( ).extension ( ) != ".mp3" &&
			entry.path ( ).extension ( ) != ".ogg" )
		{
			continue;
		}

		files.push_back ( entry.path ( ).string ( ) );
	}

	if ( files.empty ( ) )
	{
		ofLogError ( "Controller" ) << "No files found in " << directory;
		return false;
	}

	return true;
}

void Analyser::Controller::GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const Utils::AnalysisSettings& settings )
{
	dimensionNames.clear ( );

	if ( settings.bTime )
	{ // Time
		dimensionNames.push_back ( "Time" );

		if ( settings.bPitch )
		{
			dimensionNames.push_back ( "Pitch" );
			dimensionNames.push_back ( "Pitch Confidence" );
		}

		if ( settings.bLoudness )
		{
			dimensionNames.push_back ( "Loudness" );
			dimensionNames.push_back ( "True Peak" );
		}

		if ( settings.bShape )
		{
			dimensionNames.push_back ( "Spectral Centroid" );
			dimensionNames.push_back ( "Spectral Spread" );
			dimensionNames.push_back ( "Spectral Skewness" );
			dimensionNames.push_back ( "Spectral Kurtosis" );
			dimensionNames.push_back ( "Spectral Rolloff" );
			dimensionNames.push_back ( "Spectral Flatness" );
			dimensionNames.push_back ( "Spectral Crest" );
		}

		if ( settings.bMFCC )
		{
			for ( int i = 0; i < settings.nCoefs; i++ )
			{
				dimensionNames.push_back ( "MFCC " + std::to_string ( i + 1 ) );
			}
		}
	}
	else
	{ // Stats
		if ( settings.bPitch )
		{
			Push7Stats ( "Pitch", dimensionNames );
			Push7Stats ( "Pitch Confidence", dimensionNames );
		}

		if ( settings.bLoudness )
		{
			Push7Stats ( "Loudness", dimensionNames );
			Push7Stats ( "True Peak", dimensionNames );
		}

		if ( settings.bShape )
		{
			Push7Stats ( "Spectral Centroid", dimensionNames );
			Push7Stats ( "Spectral Spread", dimensionNames );
			Push7Stats ( "Spectral Skewness", dimensionNames );
			Push7Stats ( "Spectral Kurtosis", dimensionNames );
			Push7Stats ( "Spectral Rolloff", dimensionNames );
			Push7Stats ( "Spectral Flatness", dimensionNames );
			Push7Stats ( "Spectral Crest", dimensionNames );
		}

		if ( settings.bMFCC )
		{
			for ( int i = 0; i < settings.nCoefs; i++ )
			{
				Push7Stats ( "MFCC " + std::to_string ( i + 1 ), dimensionNames );
			}
		}
	}
}

void Analyser::Controller::GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const Utils::ReductionSettings& settings, bool time )
{
	dimensionNames.clear ( );

	if ( time ) { dimensionNames.push_back ( "Time" ); }

	for ( int i = 0; i < settings.dimensionReductionTarget; i++ )
	{
		dimensionNames.push_back ( "Dimension " + std::to_string ( i + 1 ) );
	}
}

void Analyser::Controller::Push7Stats ( std::string masterDimension, std::vector<std::string>& dimensionNames )
{
	Utils::DataSet temp;
	for ( int i = 0; i < temp.statisticNames.size ( ); i++ )
	{
		dimensionNames.push_back ( masterDimension + " (" + temp.statisticNames[i] + ")" );
	}
}