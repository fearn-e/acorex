#pragma once

#include <algorithms/public/DataSetQuery.hpp>
#include <algorithms/public/UMAP.hpp>
#include <Eigen/Core>
#include <data/FluidDataSet.hpp>
#include <data/FluidIndex.hpp>
#include <data/FluidJSON.hpp>
#include <data/FluidMemory.hpp>
#include <data/TensorTypes.hpp>
#include <vector>

namespace acorex {
namespace corpus {

class UMAP {
public:
    UMAP ( ) { };
    ~UMAP ( ) { };

    bool Fit ( fluid::FluidDataSet<std::string, double, 1>& datasetIN, fluid::FluidDataSet<std::string, double, 1>& datasetOUT );

    bool FitOverTime ( fluid::FluidDataSet<std::string, double, 1>& datasetIN, fluid::FluidDataSet<std::string, double, 1>& datasetOUT );

private:
    fluid::algorithm::DataSetQuery mQuery;
};

} // namespace corpus
} // namespace acorex