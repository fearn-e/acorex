#pragma once

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

namespace corpus {

class Analyse {
public:
	Analyse ( ) { };
	~Analyse ( ) { };

	int ProcessFiles ( std::vector<std::string>& files, fluid::FluidDataSet<std::string, double, 1>& dataset, bool timeDimension );

private:
	fluid::RealVector ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats );
};

} // namespace corpus