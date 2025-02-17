/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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