#pragma once

#include "Utils/Data.h"
#include <data/FluidDataSet.hpp>

namespace Acorex {
namespace Utils {

class DatasetConversion {
public:
	DatasetConversion ( ) { }
	~DatasetConversion ( ) { }

	void CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const Utils::DataSet& dataset, std::vector<int>& filePointLength );

	void FluidToCorpus ( Utils::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount );
};

} // namespace Utils
} // namespace Acorex;