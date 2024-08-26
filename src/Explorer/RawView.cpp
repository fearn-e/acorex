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

	for ( int fileIndex = 0; fileIndex < dataset.fileList.size ( ); fileIndex++ )
	{
		fluid::RealVector fileData;

		if ( !mAudioLoader.ReadAudioFile ( dataset.fileList[fileIndex], fileData, dataset.analysisSettings.sampleRate ) )
		{
			ofLogError ( "RawView" ) << "Failed to load audio file: " << dataset.fileList[fileIndex];
			dataset.audio.loaded.push_back ( false );
			dataset.audio.raw.push_back ( ofSoundBuffer ( ) );
			continue;
		}

		ofSoundBuffer audioData;
		audioData.copyFrom ( std::vector<float> ( fileData.begin ( ), fileData.end ( ) ), 1, dataset.analysisSettings.sampleRate );

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