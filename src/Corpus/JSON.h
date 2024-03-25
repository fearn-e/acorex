#pragma once

#include "Corpus/Metadata.h"
#include <data/FluidJSON.hpp>
#include <data/FluidDataSet.hpp>

namespace acorex {
namespace corpus {

class JSON {
public:
	JSON ( ) { };
	~JSON ( ) { };

	bool Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, double, 1>& dataset );

	bool Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, double, 1>& dataset );

	bool WriteMeta ( const std::string& outputFile, std::vector<corpus::Metadata>& metaset );

	bool ReadMeta ( const std::string& inputFile, std::vector<corpus::Metadata>& metaset, bool loadDefaults );

private:
	MetaStrings mMetaStrings;
};

} // namespace corpus
} // namespace acorex