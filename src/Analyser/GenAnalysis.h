#pragma once

#include "Utils/Data.h"
#include <Eigen/Core>
#include <algorithms/public/DCT.hpp>
#include <algorithms/public/Loudness.hpp>
#include <algorithms/public/MelBands.hpp>
#include <algorithms/public/MultiStats.hpp>
#include <algorithms/public/STFT.hpp>
#include <algorithms/public/SpectralShape.hpp>
#include <algorithms/public/YINFFT.hpp>
#include <audio_file/in_file.hpp>
#include <data/FluidDataSet.hpp>
#include <data/FluidIndex.hpp>
#include <data/FluidJSON.hpp>
#include <data/FluidMemory.hpp>
#include <data/TensorTypes.hpp>
#include <ofxAudioFile.h>
#include <vector>
#include <string>

namespace Acorex {
namespace Analyser {

class GenAnalysis {
public:
	GenAnalysis ( ) { };
	~GenAnalysis ( ) { };

	int ProcessFiles ( Utils::DataSet& dataset );

private:
	bool ReadFile ( std::string filename, fluid::RealVector& output, double& sampleRate );

	void ReadToMono ( fluid::RealVector& output, htl::in_audio_file& file );
	void ReadToMono ( fluid::RealVector& output, ofxAudioFile& file );

	fluid::RealVector ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats );

	void Push7Stats ( fluid::RealVector& stats, std::vector<std::vector<double>>& fileData, int numDimensions );
};

} // namespace Analyser
} // namespace Acorex