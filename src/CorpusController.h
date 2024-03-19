#pragma once

#include "Corpus/JSON.h"
#include "Corpus/Analyse.h"
#include "Corpus/UMAP.h"
#include "data/FluidDataSet.hpp"
#include <vector>
#include <string>

class CorpusController {
public:
	CorpusController ( ) { };
	~CorpusController ( ) { };

	bool CreateCorpus ( const std::string& inputPath, const std::string& outputPath, bool timeDimension );

	bool ReduceCorpus ( const std::string& inputPath, const std::string& outputPath );
	
	bool InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, bool replaceDuplicates );
	
	bool InsertIntoReducedCorpus ( const std::string& inputPath, const std::string& outputPath, bool replaceDuplicates );

private:
	bool SearchDirectory ( const std::string& directory, std::vector<std::string>& files );

	corpus::JSON mJSON;
	corpus::Analyse mAnalyse;
	corpus::UMAP mUMAP;
};