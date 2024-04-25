#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"
#include "Analyser/GenAnalysis.h"
#include "Analyser/UMAP.h"
#include <vector>
#include <string>

namespace Acorex {
namespace Analyser {

class Controller {
public:
	Controller ( ) { };
	~Controller ( ) { };

	bool CreateCorpus ( const std::string& inputPath, const std::string& outputPath, const Utils::AnalysisSettings& settings );

	bool ReduceCorpus ( const std::string& inputPath, const std::string& outputPath, const Utils::ReductionSettings& settings );

	bool InsertIntoCorpus ( const std::string& inputPath, const std::string& outputPath, const bool newReplacesExisting );

private:
	std::vector<int> MergeDatasets ( Utils::DataSet& newDataset, const Utils::DataSet& existingDataset, const bool newReplacesExisting );

	bool SearchDirectory ( const std::string& directory, std::vector<std::string>& files );

	void GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const Utils::AnalysisSettings& settings );
	void GenerateDimensionNames ( std::vector<std::string>& dimensionNames, const Utils::ReductionSettings& settings, bool time );
	void Push7Stats ( std::string masterDimension, std::vector<std::string>& dimensionNames );

	Utils::JSON mJSON;
	Analyser::GenAnalysis mGenAnalysis;
	Analyser::UMAP mUMAP;
};

} // namespace Analyser
} // namespace Acorex