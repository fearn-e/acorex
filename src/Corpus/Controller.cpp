#pragma once

#include "Corpus/Controller.h"
#include <ofLog.h>
#include <filesystem>

// Public --------------------------------------------------------------------

bool AcorexCorpus::Controller::CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexCorpus::AnalysisSettings& settings )
{
	bool success;

	AcorexCorpus::DataSet dataset;

	dataset.analysisSettings = settings;

	success = SearchDirectory ( inputPath, dataset.fileList );
	if ( !success ) { return false; }
	
	int filesIn = dataset.fileList.size ( );
	int numAnalysed = mAnalyse.ProcessFiles ( dataset );
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

bool AcorexCorpus::Controller::ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexCorpus::ReductionSettings& settings )
{
	bool success;

	AcorexCorpus::DataSet dataset;

	success = mJSON.Read ( inputPath, dataset );
	if ( !success ) 
	{ 
		ofLogError ( "Controller" ) << "Failed to read dataset from " << inputPath;
		return false; 
	}

	success = mUMAP.Fit ( dataset, settings );
	if ( !success )
	{ 
		ofLogError ( "Controller" ) << "Failed to reduce dataset.";
		return false; 
	}

	GenerateDimensionNames ( dataset.dimensionNames, settings );

	success = mJSON.Write ( outputPath, dataset );
	if ( !success )
	{ 
		ofLogError ( "Controller" ) << "Failed to write reduced dataset to " << outputPath;
		return false;
	}

	return true;
}

bool AcorexCorpus::Controller::InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const bool newReplacesExisting )
{
	bool success;

	AcorexCorpus::DataSet existingDataset;
	success = mJSON.Read ( outputPath, existingDataset );
	if ( !success )
	{ 
		ofLogError ( "Controller" ) << "Failed to read existing dataset from " << outputPath;
		return false;
	}

	std::vector<std::string> newFiles;
	success = SearchDirectory ( inputPath, newFiles );
	if ( !success )
	{
		ofLogError ( "Controller" ) << "Failed to find new files in " << inputPath;
		return false;
	}

	// remove new files that already exist if duplicates are not to be analysed again
	if ( !newReplacesExisting )
	{
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

		ofLogNotice ( "Controller" ) << newFiles.size ( ) << " new files left to process.";
	}

	AcorexCorpus::DataSet newDataset;
	newDataset.fileList = newFiles;
	// copy settings from existing dataset
	{
#ifndef DATA_CHANGE_CHECK_7
#error "check if this is still valid with dataset structure"
#endif
		newDataset.currentDimensionCount = existingDataset.currentDimensionCount;
		newDataset.analysisSettings = existingDataset.analysisSettings;
	}

	int filesIn = newDataset.fileList.size ( );
	int numAnalysed = mAnalyse.ProcessFiles ( newDataset );
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

	MergeDatasets ( existingDataset, newDataset, newReplacesExisting );

	success = mJSON.Write ( outputPath, existingDataset );
	if ( !success )
	{
		ofLogError ( "Controller" ) << "Failed to write updated dataset to " << outputPath;
		return false;
	}

	return true;
}

// Private -------------------------------------------------------------------

bool AcorexCorpus::Controller::MergeDatasets ( AcorexCorpus::DataSet& primaryDataset, const AcorexCorpus::DataSet& additionalDataset, const bool additionalReplacesPrimary )
{
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
		if ( exists && !additionalReplacesPrimary ) { continue; }

		if ( exists && additionalReplacesPrimary )
		{
			int pointCountDiff = 0;
			primaryDataset.fileList[existingIndex] = additionalDataset.fileList[i];

			if ( primaryDataset.analysisSettings.bTime )
			{ // Time
				pointCountDiff = additionalDataset.time.raw[i].size ( ) - primaryDataset.time.raw[existingIndex].size ( ); // TODO - DOUBLE CHECK THIS
				primaryDataset.time.samples[existingIndex] = additionalDataset.time.samples[i];
				primaryDataset.time.seconds[existingIndex] = additionalDataset.time.seconds[i];
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
			int pointCountDiff = 0;
			primaryDataset.fileList.push_back ( additionalDataset.fileList[i] );

			if ( primaryDataset.analysisSettings.bTime )
			{ // Time
				pointCountDiff = additionalDataset.time.raw[i].size ( ); // TODO - DOUBLE CHECK THIS
				primaryDataset.time.samples.push_back ( additionalDataset.time.samples[i] );
				primaryDataset.time.seconds.push_back ( additionalDataset.time.seconds[i] );
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

	return true;
}

bool AcorexCorpus::Controller::SearchDirectory ( const std::string& directory, std::vector<std::string>& files )
{
	files.clear ( );

	using namespace std::filesystem;
	for ( const auto& entry : recursive_directory_iterator ( directory ) )
	{
		if ( is_directory ( entry.path ( ) ) ) { continue; }

		if ( !is_regular_file ( entry.path ( ) ) ) { continue; }

		// TODO - add mp3 and ogg support
		if ( entry.path ( ).extension ( ) != ".wav" &&
			entry.path ( ).extension ( ) != ".aiff" &&
			entry.path ( ).extension ( ) != ".flac" )
		{
			continue;
		}

		files.push_back ( entry.path ( ).string ( ) );
	}

	if ( files.empty ( ) ) { return false; }

	return true;
}

void AcorexCorpus::Controller::GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const AcorexCorpus::AnalysisSettings& settings )
{
	dimensionNames.clear ( );

	if ( settings.bTime )
	{ // Time
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

void AcorexCorpus::Controller::GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const AcorexCorpus::ReductionSettings& settings )
{
	dimensionNames.clear ( );

	for ( int i = 0; i < settings.dimensionReductionTarget; i++ )
	{
		dimensionNames.push_back ( "Dimension " + std::to_string ( i + 1 ) );
	}
}

void AcorexCorpus::Controller::Push7Stats ( std::string masterDimension, std::vector<std::string>& dimensionNames )
{
	dimensionNames.push_back ( masterDimension + " (Mean)" );
	dimensionNames.push_back ( masterDimension + " (Standard Deviation)" );
	dimensionNames.push_back ( masterDimension + " (Skewness)" );
	dimensionNames.push_back ( masterDimension + " (Kurtosis)" );
	dimensionNames.push_back ( masterDimension + " (Low %)" );
	dimensionNames.push_back ( masterDimension + " (Middle %)" );
	dimensionNames.push_back ( masterDimension + " (High %)" );
}