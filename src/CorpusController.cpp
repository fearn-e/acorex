#pragma once

#include "CorpusController.h"
#include <ofLog.h>
#include <filesystem>

bool CorpusController::CreateCorpus ( const std::string& inputPath, const std::string& outputPath, bool timeDimension )
{
	bool success;

	std::vector<std::string> files;
	success = SearchDirectory ( inputPath, files );
	if ( !success ) { return false; }
	
	fluid::FluidDataSet<std::string, double, 1> dataset ( timeDimension ? 26 : 168 );
	int numFailed = mAnalyse.ProcessFiles ( files, dataset, timeDimension );
	if ( numFailed < files.size() && dataset.size() != 0 ) { ofLogNotice ( "CorpusController" ) << "Processed " << files.size ( ) << " files into " << dataset.size ( ) << " points, with " << numFailed << " files failed."; }
	else 
	{ 
		ofLogError ( "CorpusController" ) << "Failed to process any files.";
		return false;
	}

	success = mJSON.Write ( outputPath, dataset );
	if ( !success ) { return false; }

	//fluid::FluidDataSet<std::string, bool, 1> metaset ( 1 );
	//metaset.add ( "timeDimension", timeDimension );
	//success = mJSON.Write ( corpusName + ".meta", metaset );
	//if ( !success ) { return false; }
	
	return true;
}

bool CorpusController::ReduceCorpus ( const std::string& inputPath, const std::string& outputPath )
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

bool CorpusController::InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, bool replaceDuplicates )
{
	bool success;

	std::vector<std::string> files;
	success = SearchDirectory ( inputPath, files );
	if ( !success ) { return false; }

	//call Analyse.AnalysePerFile or Analyse.AnalysePerFrame
	//call JSON.ReadAnalysis
	//insert new data into dataset
	//call JSON.WriteAnalysis

	return true;
}

bool CorpusController::InsertIntoReducedCorpus ( const std::string& inputPath, const std::string& outputPath, bool replaceDuplicates )
{
	bool success;

	std::vector<std::string> files;
	success = SearchDirectory ( inputPath, files );
	if ( !success ) { return false; }

	//call Analysis.AnalysePerFile or Analysis.AnalysePerFrame
	//call JSON.ReadReduced
	//call UMAP.TrainedReducePerFile or UMAP.TrainedReducePerFrame
	//insert new data into dataset
	//call JSON.WriteReduced

	return true;
}

bool CorpusController::SearchDirectory ( const std::string& directory, std::vector<std::string>& files )
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
		ofLogError ( "CorpusController" ) << "No audio files found in " << directory;
		return false;
	}

	return true;
}