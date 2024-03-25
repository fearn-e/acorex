#pragma once

#include "Corpus/JSON.h"
#include "Corpus/Analyse.h"
#include "Corpus/UMAP.h"
#include "Corpus/Metadata.h"
#include <data/FluidDataSet.hpp>
#include <vector>
#include <string>

namespace acorex {
namespace corpus {

class Controller {
public:
	Controller ( ) { };
	~Controller ( ) { };

	bool CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const std::vector<corpus::Metadata>& metaset );

	bool ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const std::vector<corpus::Metadata>& metaset );

	bool InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const std::vector<corpus::Metadata>& metaset );

private:
	bool SearchDirectory ( const std::string& directory, std::vector<std::string>& files );

	bool WriteMeta ( const std::string& outputFile, std::vector<corpus::Metadata>& metaset );

	corpus::JSON mJSON;
	corpus::Analyse mAnalyse;
	corpus::UMAP mUMAP;
};

} // namespace corpus
} // namespace acorex