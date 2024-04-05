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

namespace AcorexCorpus {

class UMAP {
public:
    UMAP ( ) { };
    ~UMAP ( ) { };

    bool Fit ( AcorexCorpus::DataSet& dataset, const AcorexCorpus::ReductionSettings& settings );

private:
    void CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const AcorexCorpus::DataSet& dataset, std::vector<int>& filePointLength );

    void FluidToCorpus ( AcorexCorpus::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount );
};

} // namespace AcorexCorpus