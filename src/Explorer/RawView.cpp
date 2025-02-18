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
#include <iostream>

using namespace Acorex;

bool Explorer::RawView::LoadCorpus ( )
{
	ofFileDialogResult corpusFile = ofSystemLoadDialog ( "Select corpus file" );
	if ( !corpusFile.bSuccess )
	{
		std::cerr << "Invalid load query" << std::endl;
		return false;
	}
	
	bool success = LoadCorpus ( corpusFile.getPath ( ), corpusFile.getName ( ) );
	
	return success;
}

bool Explorer::RawView::LoadCorpus ( const std::string& path, const std::string& name )
{
	if ( name.find ( ".json" ) == std::string::npos )
	{
		std::cerr << "Invalid file type" << std::endl;
		return false;
	}
	if ( !ofFile::doesFileExist ( path ) )
	{
		std::cerr << "File does not exist" << std::endl;
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
	dataset.audio.mono.clear ( );
	dataset.audio.size.clear ( );
	int loadedCount = 0;

	for ( int fileIndex = 0; fileIndex < dataset.fileList.size ( ); fileIndex++ )
	{
		std::vector<float> fileData;

		if ( !mAudioLoader.ReadAudioFile ( dataset.fileList[fileIndex], fileData, dataset.analysisSettings.sampleRate ) )
		{
			std::cerr << "Failed to load audio file: " << dataset.fileList[fileIndex] << std::endl;
			dataset.audio.loaded.push_back ( false );
			dataset.audio.mono.push_back ( std::vector<float> ( ) );
			dataset.audio.size.push_back ( 0 );
			continue;
		}

		dataset.audio.loaded.push_back ( true );
		dataset.audio.mono.push_back ( fileData );
		dataset.audio.size.push_back ( fileData.size ( ) );
		loadedCount++;
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
		std::cerr << "Failed to load any audio files" << std::endl;
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