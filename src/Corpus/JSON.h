#pragma once

#include <data/FluidJSON.hpp>
#include <data/FluidDataSet.hpp>

namespace corpus {

class JSON {
public:
	JSON ( ) { };
	~JSON ( ) { };

	bool Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, double, 1>& dataset );
	//bool Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, std::string, 1>& metaset );

	bool Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, double, 1>& dataset );
	//bool Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, std::string, 1>& metaset );
};

} // namespace corpus

