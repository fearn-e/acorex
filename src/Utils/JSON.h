#pragma once

#include "Utils/Data.h"
#include <nlohmann/json.hpp>

namespace AcorexCorpus {

class JSON {

#define TO_J( x ) {#x, a.x}
#define TO_J_SETTINGS( x ) {#x, a.analysisSettings.x}

#define TO_A( x ) j.at ( #x ).get_to ( a.x )
#define TO_A_SETTINGS( x ) j.at ( #x ).get_to ( a.analysisSettings.x )

public:
	JSON ( ) { };
	~JSON ( ) { };

	bool Write ( const std::string& outputFile, const AcorexCorpus::DataSet& dataset );

	bool Read ( const std::string& inputFile, AcorexCorpus::DataSet& dataset );
	bool Read ( const std::string& inputFile, AcorexCorpus::AnalysisSettings& settings );
};

void to_json ( nlohmann::json& j, const AcorexCorpus::DataSet& a );
void from_json ( const nlohmann::json& j, AcorexCorpus::DataSet& a );

void to_json ( nlohmann::json& j, const AcorexCorpus::AnalysisSettings& a );
void from_json ( const nlohmann::json& j, AcorexCorpus::AnalysisSettings& a );

} // namespace AcorexCorpus