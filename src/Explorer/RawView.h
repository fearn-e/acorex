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

private:
	Utils::DataSet mDataset;

	Utils::JSON mJSON;
};

} // namespace Explorer
} // namespace Acorex