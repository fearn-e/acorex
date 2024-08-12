#include "./RawView.h"
#include <data/TensorTypes.hpp>
#include <ofSoundBuffer.h>
#include <ofSystemUtils.h>
#include <ofLog.h>

using namespace Acorex;

bool Explorer::RawView::LoadCorpus ( )
{
	ofFileDialogResult corpusFile = ofSystemLoadDialog ( "Select corpus file" );
	if ( !corpusFile.bSuccess )
	{
		ofLogError ( "RawView" ) << "Invalid load query";
		return false;
	}
	
	bool success = LoadCorpus ( corpusFile.getPath ( ), corpusFile.getName ( ) );
	
	return success;
}

bool Explorer::RawView::LoadCorpus ( const std::string& path, const std::string& name )
{
	if ( name.find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "RawView" ) << "Invalid file type";
		return false;
	}
	if ( !ofFile::doesFileExist ( path ) )
	{
		ofLogError ( "RawView" ) << "File does not exist";
		return false;
	}

	bool success = mJSON.Read ( path, mDataset );

	if ( !success ) { return success; }

	mCorpusName = name.substr ( 0, name.size ( ) - 5 );

	success = LoadAudioSet ( mDataset );
	
	return success;
}

bool Explorer::RawView::LoadAudioSet ( Utils::DataSet& dataset )
{
	dataset.audio.loaded.clear ( );
	dataset.audio.raw.clear ( );
	dataset.audio.originalSampleRates.clear ( );

	for ( int fileIndex = 0; fileIndex < dataset.fileList.size ( ); fileIndex++ )
	{
		fluid::RealVector fileData;
		double sampleRate;

		if ( !mAudioLoader.ReadAudioFile ( dataset.fileList[fileIndex], fileData, sampleRate ) )
		{
			ofLogError ( "RawView" ) << "Failed to load audio file: " << dataset.fileList[fileIndex];
			dataset.audio.loaded.push_back ( false );
			dataset.audio.originalSampleRates.push_back ( 0 );
			dataset.audio.raw.push_back ( ofSoundBuffer ( ) );
			continue;
		}

		dataset.audio.originalSampleRates.push_back ( sampleRate );

		ofSoundBuffer audioData;
		audioData.copyFrom ( std::vector<float> ( fileData.begin ( ), fileData.end ( ) ), 1, sampleRate );

		if ( sampleRate != globalSampleRate )
		{
			audioData.resample ( ( sampleRate / globalSampleRate ), ofSoundBuffer::Hermite );
			audioData.setSampleRate ( globalSampleRate );
		}

		dataset.audio.raw.push_back ( audioData );
		dataset.audio.loaded.push_back ( true );
	}
	
	bool failedToLoad = true;
	for ( int i = 0; i < dataset.audio.loaded.size ( ); i++ )
	{
		if ( dataset.audio.loaded[i] )
		{
			failedToLoad = false;
			break;
		}
	}

	if ( failedToLoad )
	{
		ofLogError ( "RawView" ) << "Failed to load any audio files";
		return false;
	}

	return true;
}

bool Explorer::RawView::IsTimeAnalysis ( ) const
{
	return mDataset.analysisSettings.bTime;
}

bool Explorer::RawView::IsReduction ( ) const
{
	return mDataset.analysisSettings.hasBeenReduced;
}

std::vector<std::string> Explorer::RawView::GetDimensions ( ) const
{
	return mDataset.dimensionNames;
}

std::vector<std::string> Explorer::RawView::GetStatistics ( ) const
{
	return mDataset.statisticNames;
}

std::string Explorer::RawView::GetCorpusName ( ) const
{
	return mCorpusName;
}

Utils::AudioData* Explorer::RawView::GetAudioData ( )
{
	return &mDataset.audio;
}

Utils::TimeData* Explorer::RawView::GetTimeData ( )
{
	return &mDataset.time;
}

Utils::StatsData* Explorer::RawView::GetStatsData ( )
{
	return &mDataset.stats;
}

Utils::DataSet* Explorer::RawView::GetDataset ( )
{
	return &mDataset;
}