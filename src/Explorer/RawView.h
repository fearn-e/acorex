#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"
#include "utils/AudioFileLoader.h"

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

	double globalSampleRate;
};

} // namespace Explorer
} // namespace Acorex