#pragma once

#include "algorithms/public/UMAP.hpp"
#include <Eigen/Core>
#include <data/FluidDataSet.hpp>
#include <data/FluidIndex.hpp>
#include <data/FluidJSON.hpp>
#include <data/FluidMemory.hpp>
#include <data/TensorTypes.hpp>
#include <vector>

class UMAP {
public:

	void Fit (std::string inputFile, std::string outputFile )
	{
        using namespace fluid;
        using namespace fluid::algorithm;
        using fluid::index;

        FluidDataSet<std::string, double, 1> datasetIN ( 1 );
        FluidDataSet<std::string, double, 1> datasetOUT ( 1 );

        const char* inFile = inputFile.c_str ( );
        const char* outFile = outputFile.c_str ( );

        auto inputJSON = JSONFile ( inFile, "r" );
        nlohmann::json j = inputJSON.read ( );

        if ( !inputJSON.ok ( ) )
        {
            ofLogError ( "UMAP" ) << "failed to read input " << inFile;
        }

        if ( !check_json ( j, datasetIN ) )
        {
            ofLogError ( "UMAP" ) << "Invalid JSON format";
        }

        datasetIN = j.get<FluidDataSet<std::string, double, 1>> ( );

        algorithm::UMAP algorithm;

        datasetOUT = algorithm.train ( datasetIN, 15, 3, 0.1, 200, 0.1 );

        auto outputJSON = JSONFile ( outFile, "w" );
        outputJSON.write ( datasetOUT );

        if ( !outputJSON.ok ( ) )
        {
            ofLogError ( "UMAP" ) << "failed to write output to " << outFile;
        }
	}

};