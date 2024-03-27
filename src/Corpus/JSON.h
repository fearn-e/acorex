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

	bool WriteMeta ( const std::string& outputFile, std::vector<AcorexCorpus::Metadata>& metaset );

	bool ReadMeta ( const std::string& inputFile, std::vector<AcorexCorpus::Metadata>& metaset, bool loadDefaults );

private:
	MetaStrings mMetaStrings;

	bool ReplaceExtensionToMeta ( std::string& path );
};

} // namespace AcorexCorpus