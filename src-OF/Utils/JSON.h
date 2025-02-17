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
#include <nlohmann/json.hpp>

namespace Acorex {
namespace Utils {

class JSON {

#define TO_J( x ) {#x, a.x}
#define TO_J_SETTINGS( x ) {#x, a.analysisSettings.x}

#define TO_A( x ) j.at ( #x ).get_to ( a.x )
#define TO_A_SETTINGS( x ) j.at ( #x ).get_to ( a.analysisSettings.x )

public:
	JSON ( ) { };
	~JSON ( ) { };

	bool Write ( const std::string& outputFile, const DataSet& dataset );

	bool Read ( const std::string& inputFile, DataSet& dataset );
	bool Read ( const std::string& inputFile, AnalysisSettings& settings );
};

void to_json ( nlohmann::json& j, const DataSet& a );
void from_json ( const nlohmann::json& j, DataSet& a );

void to_json ( nlohmann::json& j, const AnalysisSettings& a );
void from_json ( const nlohmann::json& j, AnalysisSettings& a );

} // namespace Utils
} // namespace Acorex