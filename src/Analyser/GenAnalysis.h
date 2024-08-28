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
#include "Utils/AudioFileLoader.h"
#include <Eigen/Core>
#include <algorithms/public/DCT.hpp>
#include <algorithms/public/Loudness.hpp>
#include <algorithms/public/MelBands.hpp>
#include <algorithms/public/MultiStats.hpp>
#include <algorithms/public/STFT.hpp>
#include <algorithms/public/SpectralShape.hpp>
#include <algorithms/public/YINFFT.hpp>
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
	fluid::RealVector ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats );

	void Push7Stats ( fluid::RealVector& stats, std::vector<std::vector<double>>& fileData, int numDimensions );

	Utils::AudioFileLoader mAudioLoader;
};

} // namespace Analyser
} // namespace Acorex