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

namespace AcorexAnalyse {

class UMAP {
public:
    UMAP ( ) { };
    ~UMAP ( ) { };

    bool Fit ( AcorexUtils::DataSet& dataset, const AcorexUtils::ReductionSettings& settings );

private:
    void CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const AcorexUtils::DataSet& dataset, std::vector<int>& filePointLength );

    void FluidToCorpus ( AcorexUtils::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount );
};

} // namespace AcorexAnalyse