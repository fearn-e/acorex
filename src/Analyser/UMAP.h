#pragma once

#include "Utils/Data.h"
#include <algorithms/public/UMAP.hpp>
#include <Eigen/Core>
#include <data/FluidDataSet.hpp>
#include <data/FluidIndex.hpp>
#include <data/FluidJSON.hpp>
#include <data/FluidMemory.hpp>
#include <data/TensorTypes.hpp>
#include <vector>

namespace Acorex {
namespace Analyser {

class UMAP {
public:
    UMAP ( ) { };
    ~UMAP ( ) { };

    bool Fit ( Utils::DataSet& dataset, const Utils::ReductionSettings& settings );

private:
    void CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const Utils::DataSet& dataset, std::vector<int>& filePointLength );

    void FluidToCorpus ( Utils::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount );
};

} // namespace Analyser
} // namespace Acorex