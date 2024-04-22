#pragma once

#include "Utils/Data.h"
#include "Utils/DatasetConversion.h"
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
    Utils::DatasetConversion mConversion;
};

} // namespace Analyser
} // namespace Acorex