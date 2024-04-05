#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"

namespace AcorexExplore {

class RawView {
public:
	RawView ( ) { }
	~RawView ( ) { }

	bool LoadCorpus ( ); // asks user for file path, calls function below
	bool LoadCorpus ( const std::string& path, const std::string& name ); // load corpus from file path

private:
	AcorexUtils::DataSet mDataset;

	AcorexUtils::JSON mJSON;
};

} // namespace AcorexExplore