#pragma once

#include "Corpus/Controller.h"
#include <ofLog.h>
#include <filesystem>

bool AcorexCorpus::Controller::CreateCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::MetaSetStruct& metaset )
{
	bool success;

	success = SearchDirectory ( inputPath, metaset.fileList );
	if ( !success ) { return false; }
	
	fluid::FluidDataSet<std::string, double, 1> dataset ( 1 );
	int numFailed = mAnalyse.ProcessFiles ( dataset, metaset, metaset.fileList );
	if ( numFailed < metaset.fileList.size() && dataset.size() != 0 ) { ofLogNotice ( "Controller" ) << "Processed " << metaset.fileList.size ( ) << " files into " << dataset.size ( ) << " points, with " << numFailed << " files failed."; }
	else 
	{ 
		ofLogError ( "Controller" ) << "Failed to process any files.";
		return false;
	}

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }
	
	return true;
}

bool AcorexCorpus::Controller::ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexCorpus::MetaSetStruct& metaset )
{
	bool success;

	int numReducedDimensions = metaset.dimensionReductionTarget + ( metaset.timeDimension ? 2 : 0 );

	fluid::FluidDataSet<std::string, double, 1> dataset ( metaset.currentDimensionCount );
	fluid::FluidDataSet<std::string, double, 1> reducedDataset ( numReducedDimensions );

	success = mJSON.Read ( inputPath, dataset );
	if ( !success ) { return false; }

	if ( metaset.timeDimension ) { mUMAP.FitOverTime ( dataset, reducedDataset ); }
	else { mUMAP.Fit ( dataset, reducedDataset ); }

	success = mJSON.Write ( outputPath, reducedDataset );
	if ( !success ) { return false; }

	return true;
}

bool AcorexCorpus::Controller::InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::MetaSetStruct& metaset )
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
	int numFailed = mAnalyse.ProcessFiles ( dataset, metaset, newFiles );
	if ( numFailed < newFiles.size ( ) && dataset.size ( ) != 0 ) { ofLogNotice ( "Controller" ) << "Processed " << newFiles.size ( ) << " files into " << dataset.size ( ) << " points, with " << numFailed << " files failed."; }
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

bool InsertionTreatFilesList ( std::vector<std::string>& files ) //add more here
{



	return true;
}

bool AcorexCorpus::Controller::SearchDirectory ( const std::string& directory, std::vector<std::string>& files )
{
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