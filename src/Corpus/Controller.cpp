#pragma once

#include "Corpus/Controller.h"
#include <ofLog.h>
#include <filesystem>

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