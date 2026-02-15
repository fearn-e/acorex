/*
The MIT License (MIT)

Copyright (c) 2024-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Utilities/Data.h"
#include "Utilities/AudioFileLoader.h"

#include <Eigen/Core>
#include <flucoma/algorithms/public/DCT.hpp>
#include <flucoma/algorithms/public/Loudness.hpp>
#include <flucoma/algorithms/public/MelBands.hpp>
#include <flucoma/algorithms/public/MultiStats.hpp>
#include <flucoma/algorithms/public/STFT.hpp>
#include <flucoma/algorithms/public/SpectralShape.hpp>
#include <flucoma/algorithms/public/YINFFT.hpp>
#include <flucoma/data/FluidDataSet.hpp>
#include <flucoma/data/FluidIndex.hpp>
#include <flucoma/data/FluidJSON.hpp>
#include <flucoma/data/FluidMemory.hpp>
#include <flucoma/data/TensorTypes.hpp>
#include <ofxAudioFile.h>
#include <vector>
#include <string>

namespace Acorex {
namespace Analyser {

class GenAnalysis {
public:
    GenAnalysis ( ) { };
    ~GenAnalysis ( ) { };

    int ProcessFiles ( Utilities::DataSet& dataset );

private:
    Utilities::AudioFileLoader mAudioLoader;
};

} // namespace Analyser
} // namespace Acorex