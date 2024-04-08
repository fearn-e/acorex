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
#include <vector>
#include <string>

namespace AcorexAnalyser {

class GenAnalysis {
public:
	GenAnalysis ( ) { };
	~GenAnalysis ( ) { };

	int ProcessFiles ( AcorexUtils::DataSet& dataset );

private:
	void MixDownToMono ( fluid::RealVector& output, htl::in_audio_file& file );

	fluid::RealVector ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats );

	void Push7Stats ( fluid::RealVector& stats, std::vector<std::vector<double>> fileData, int numDimensions );
};

} // namespace AcorexAnalyser