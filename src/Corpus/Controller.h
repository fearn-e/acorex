#pragma once

#include "Corpus/Data.h"
#include "Corpus/JSON.h"
#include "Corpus/Analyse.h"
#include "Corpus/UMAP.h"
#include <vector>
#include <string>

namespace AcorexCorpus {

class Controller {
public:
	Controller ( ) { };
	~Controller ( ) { };

	bool CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexCorpus::AnalysisSettings& settings );

	bool ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexCorpus::ReductionSettings& settings );

	bool InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const bool newReplacesExisting );

private:
	std::vector<int> MergeDatasets ( AcorexCorpus::DataSet& newDataset, const AcorexCorpus::DataSet& existingDataset, const bool newReplacesExisting );

	bool SearchDirectory ( const std::string& directory, std::vector<std::string>& files );

	void GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const AcorexCorpus::AnalysisSettings& settings );
	void GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const AcorexCorpus::ReductionSettings& settings );
	void Push7Stats ( std::string masterDimension, std::vector<std::string>& dimensionNames );

	AcorexCorpus::JSON mJSON;
	AcorexCorpus::Analyse mAnalyse;
	AcorexCorpus::UMAP mUMAP;
};

} // namespace AcorexCorpus