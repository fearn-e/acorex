#pragma once

#include "Corpus/Controller.h"
#include <ofLog.h>
#include <filesystem>

bool AcorexCorpus::Controller::CreateCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::DataSet& dataset )
{
	bool success;

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
		dataset.currentDimensionCount = 0;
		ofLogError ( "Controller" ) << "Failed to process any files.";
		return false;
	}

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }
	
	return true;
}

bool AcorexCorpus::Controller::ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::DataSet& dataset, const AcorexCorpus::ReductionSettings& settings )
{
	bool success;

	success = mJSON.Read ( inputPath, dataset );
	if ( !success ) { return false; }

	mUMAP.Fit ( dataset, settings );

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }

	return true;
}

bool AcorexCorpus::Controller::InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::DataSet& dataset )
{
	bool success;

	std::vector<std::string> newFiles;
	success = SearchDirectory ( inputPath, newFiles );
	if ( !success ) { return false; }

	if ( !metaset.insertionReplacesDuplicates )
	{
		std::vector<std::string> newFilesTreated;

		for ( auto each : newFiles )
		{
			if ( std::find ( metaset.fileList.begin ( ), metaset.fileList.end ( ), each ) != metaset.fileList.end ( ) )
			{
				ofLogNotice ( "Controller" ) << "File " << each << " already exists in the corpus, removing it from analysis queue.";
				continue;
			}

			newFilesTreated.push_back ( each );
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

	fluid::FluidDataSet<std::string, double, 1> dataset ( 1 );
	int numAnalysed = mAnalyse.ProcessFiles ( dataset );
	if ( numAnalysed > 0 )
	{
		ofLogNotice ( "Controller" ) << "Processed " << dataset.fileList.size ( ) << " files into " << dataset.currentPointCount
			<< " points, with " << dataset.fileList.size ( ) - numAnalysed << " files failed.";
	}
	else
	{
		ofLogError ( "Controller" ) << "Failed to process any files.";
		return false;
	}

	fluid::FluidDataSet<std::string, double, 1> existingDataset ( 1 );
	success = mJSON.Read ( outputPath, existingDataset );
	if ( !success ) { return false; }

	fluid::FluidTensorView existingFileNames = existingDataset.getIds ( );

	for ( int i = 0; i < dataset.size ( ); i++ )
	{
		// TODO - check if the file already exists in the dataset
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

	if ( files.empty ( ) )
	{
		ofLogError ( "Controller" ) << "No audio files found in " << directory;
		return false;
	}

	return true;
}