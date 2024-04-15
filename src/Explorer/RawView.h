#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"

namespace Acorex {
namespace Explorer {

class RawView {
public:
	RawView ( ) { }
	~RawView ( ) { }

	bool LoadCorpus ( ); // asks user for file path, calls function below
	bool LoadCorpus ( const std::string& path, const std::string& name ); // load corpus from file path

	std::vector<std::string> GetDimensions ( ); // get dimensions from dataset
	std::string GetCorpusName ( ); // get corpus name

private:
	std::string mCorpusName;
	Utils::DataSet mDataset;

	Utils::JSON mJSON;
};

} // namespace Explorer
} // namespace Acorex