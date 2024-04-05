#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"
#include "Analyse/GenAnalysis.h"
#include "Analyse/UMAP.h"
#include <vector>
#include <string>

namespace AcorexAnalyse {

class Controller {
public:
	Controller ( ) { };
	~Controller ( ) { };

	bool CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexUtils::AnalysisSettings& settings );

	bool ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const AcorexUtils::ReductionSettings& settings );

	bool InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const bool newReplacesExisting );

private:
	std::vector<int> MergeDatasets ( AcorexUtils::DataSet& newDataset, const AcorexUtils::DataSet& existingDataset, const bool newReplacesExisting );

	bool SearchDirectory ( const std::string& directory, std::vector<std::string>& files );

	void GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const AcorexUtils::AnalysisSettings& settings );
	void GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const AcorexUtils::ReductionSettings& settings );
	void Push7Stats ( std::string masterDimension, std::vector<std::string>& dimensionNames );

	AcorexUtils::JSON mJSON;
	AcorexAnalyse::GenAnalysis mGenAnalysis;
	AcorexAnalyse::UMAP mUMAP;
};

} // namespace AcorexAnalyse