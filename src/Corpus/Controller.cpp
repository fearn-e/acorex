#pragma once

#include "Corpus/Controller.h"
#include <ofLog.h>
#include <filesystem>

bool AcorexCorpus::Controller::CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const std::vector<AcorexCorpus::Metadata>& metaset )
{
	bool success;

	std::vector<std::string> files;
	success = SearchDirectory ( inputPath, files );
	if ( !success ) { return false; }
	
	fluid::FluidDataSet<std::string, double, 1> dataset ( 1 );
	int numFailed = mAnalyse.ProcessFiles ( files, dataset, metaset );
	if ( numFailed < files.size() && dataset.size() != 0 ) { ofLogNotice ( "Controller" ) << "Processed " << files.size ( ) << " files into " << dataset.size ( ) << " points, with " << numFailed << " files failed."; }
	else 
	{ 
		ofLogError ( "Controller" ) << "Failed to process any files.";
		return false;
	}

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }

	//std::string metaPath = outputPath;
	//metaPath.replace ( metaPath.find_last_of ( '.' ), metaPath.length ( ), ".meta.json" );
	//success = WriteMeta ( metaPath, metaset );
	//if ( !success ) { return false; }
	
	return true;
}

bool AcorexCorpus::Controller::ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const std::vector<AcorexCorpus::Metadata>& metaset )
{
	bool success;

	fluid::FluidDataSet<std::string, double, 1> dataset ( 168 );
	fluid::FluidDataSet<std::string, double, 1> reducedDataset ( 3 );

	success = mJSON.Read ( inputPath, dataset );
	if ( !success ) { return false; }

	if ( dataset.dims ( ) == 26 )
	{
		reducedDataset.resize ( 5 );
		mUMAP.FitOverTime ( dataset, reducedDataset );
	}
	else
	{
		mUMAP.Fit ( dataset, reducedDataset );
	}

	success = mJSON.Write ( outputPath, reducedDataset );
	if ( !success ) { return false; }

	return true;
}

bool AcorexCorpus::Controller::InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const std::vector<AcorexCorpus::Metadata>& metaset )
{
	bool success;

	std::vector<std::string> files;
	success = SearchDirectory ( inputPath, files );
	if ( !success ) { return false; }

	// CHECK REPLACE DUPLICATES

	//call Analyse.AnalysePerFile or Analyse.AnalysePerFrame
	//call JSON.ReadAnalysis
	//insert new data into dataset
	//call JSON.WriteAnalysis

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

bool AcorexCorpus::Controller::WriteMeta ( const std::string& outputFile, std::vector<AcorexCorpus::Metadata>& metaset )
{
	bool success;

	success = mJSON.WriteMeta ( outputFile, metaset );
	if ( !success ) { return false; }

	return true;
}