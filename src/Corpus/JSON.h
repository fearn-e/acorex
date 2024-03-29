#pragma once

#include "Corpus/Metadata.h"
#include <data/FluidJSON.hpp>
#include <data/FluidDataSet.hpp>

namespace AcorexCorpus {

class JSON {
public:
	JSON ( ) { };
	~JSON ( ) { };

	bool Write ( const std::string& outputFile, fluid::FluidDataSet<std::string, double, 1>& dataset );

	bool Read ( const std::string& inputFile, fluid::FluidDataSet<std::string, double, 1>& dataset );

	bool WriteMeta ( const std::string& outputFile, const AcorexCorpus::MetaSetStruct& metaset );

	bool ReadMeta ( const std::string& inputFile, AcorexCorpus::MetaSetStruct& metaset, bool test );

private:
	bool ReplaceExtensionToMeta ( std::string& path );
};

} // namespace AcorexCorpus