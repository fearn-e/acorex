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

#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"
#include "Utils/AudioFileLoader.h"

namespace Acorex {
namespace Explorer {

class RawView {
public:
	RawView ( ) { }
	~RawView ( ) { }

	bool LoadCorpus ( ); // asks user for file path, calls function below
	bool LoadCorpus ( const std::string& path, const std::string& name ); // load corpus from file path

	bool IsTimeAnalysis ( ) const; // check if dataset is time analysis
	bool IsReduction ( ) const; // check if dataset is a reduced corpus
	std::vector<std::string> GetDimensions ( ) const; // get dimensions from dataset
	std::vector<std::string> GetStatistics ( ) const; // get statistics from dataset
	std::string GetCorpusName ( ) const; // get corpus name
	Utils::AudioData* GetAudioData ( ); // get audio data from dataset
	Utils::TimeData* GetTimeData ( ); // get time data from dataset
	Utils::StatsData* GetStatsData ( ); // get stats data from dataset
	Utils::DataSet* GetDataset ( ); // get dataset

private:
	bool LoadAudioSet ( Utils::DataSet& dataset ); // load all audio files in dataset into memory

	std::string mCorpusName;
	Utils::DataSet mDataset;

	Utils::JSON mJSON;
	Utils::AudioFileLoader mAudioLoader;
};

} // namespace Explorer
} // namespace Acorex
