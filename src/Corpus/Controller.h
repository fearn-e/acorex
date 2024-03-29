#pragma once

#include "Corpus/JSON.h"
#include "Corpus/Analyse.h"
#include "Corpus/UMAP.h"
#include "Corpus/Metadata.h"
#include <data/FluidDataSet.hpp>
#include <vector>
#include <string>

namespace AcorexCorpus {

class Controller {
public:
	Controller ( ) { };
	~Controller ( ) { };

	bool CreateCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::MetaSetStruct& metaset );

	bool ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexCorpus::MetaSetStruct& metaset );

	bool InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, AcorexCorpus::MetaSetStruct& metaset );

private:
	bool SearchDirectory ( const std::string& directory, std::vector<std::string>& files );

	AcorexCorpus::JSON mJSON;
	AcorexCorpus::Analyse mAnalyse;
	AcorexCorpus::UMAP mUMAP;
};

} // namespace AcorexCorpus