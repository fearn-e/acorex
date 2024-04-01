#pragma once

#include "UMAP.h"
#include "ofLog.h"

bool AcorexCorpus::UMAP::Fit ( AcorexCorpus::DataSet& dataset, const AcorexCorpus::ReductionSettings& settings )
{
    fluid::algorithm::UMAP algorithm;

	fluid::FluidDataSet<std::string, double, 1> fluidsetIN ( dataset.currentDimensionCount );
	fluid::FluidDataSet<std::string, double, 1> fluidsetOUT ( settings.dimensionReductionTarget );

	std::vector<int> filePointLength ( dataset.isTimeAnalysis ? dataset.currentPointCount : 0, false );

	CorpusToFluid ( fluidsetIN, dataset, filePointLength );

	fluidsetOUT = algorithm.train ( fluidsetIN, 15, settings.dimensionReductionTarget, 0.1, settings.maxIterations, 0.1 );

	FluidToCorpus ( dataset, fluidsetOUT, filePointLength, settings.dimensionReductionTarget );

	dataset.isReduction = true;

    return true;
}

void AcorexCorpus::UMAP::CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const AcorexCorpus::DataSet& dataset, std::vector<int>& filePointLength )
{
	if ( dataset.isTimeAnalysis )
	{	
		for ( int file = 0; file < dataset.tData.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < dataset.tData[file].size ( ); timepoint++ )
			{
				fluid::RealVector point ( dataset.tData[file][timepoint].size ( ) );

				for ( int dimension = 0; dimension < dataset.tData[file][timepoint].size ( ); dimension++ )
				{
					point[dimension] = dataset.tData[file][timepoint][dimension];
				}

				fluidset.add ( std::to_string ( file ), point );
			}

			filePointLength.push_back ( dataset.tData[file].size ( ) );
		}
	}
	else
	{
		for ( int file = 0; file < dataset.sData.size ( ); file++ )
		{
			fluid::RealVector point ( dataset.currentDimensionCount );
			
			for ( int dimension = 0; dimension < dataset.sData[file].size ( ); dimension++ )
			{
				for ( int statistic = 0; statistic < dataset.sData[file][dimension].size ( ); statistic++ )
				{
					point[( dimension * DATA_NUM_STATS ) + statistic] = dataset.sData[file][dimension][statistic];
				}
			}

			fluidset.add ( std::to_string ( file ), point );
		}
	}
}

void AcorexCorpus::UMAP::FluidToCorpus ( AcorexCorpus::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount )
{
	if ( dataset.isTimeAnalysis )
	{
		dataset.tData.clear ( );

		int fileMarker = 0;

		for ( int filePoint = 0; filePoint < filePointLength.size ( ); filePoint++ )
		{
			dataset.tData.push_back ( std::vector<std::vector<double>> ( ) );

			for ( int timePoint = 0; timePoint < filePointLength[filePoint]; timePoint++ )
			{
				dataset.tData[filePoint].push_back ( std::vector<double> ( reducedDimensionCount, 0.0 ) );

				fluid::RealVector pointVals ( reducedDimensionCount );
				fluidset.get ( std::to_string ( fileMarker + timePoint ), pointVals );

				for ( int dimension = 0; dimension < reducedDimensionCount; dimension++ )
				{
					dataset.tData[filePoint][timePoint][dimension] = pointVals[dimension];
				}
			}

			fileMarker += filePointLength[filePoint];
		}
	}
	else
	{
		dataset.sData.clear ( );
		dataset.sDataReduced.clear ( );

		for ( int filePoint = 0; filePoint < fluidset.size ( ); filePoint++ )
		{
			dataset.sDataReduced.push_back ( std::vector<double> ( reducedDimensionCount, 0.0 ) );

			fluid::RealVector pointVals ( reducedDimensionCount );
			fluidset.get ( std::to_string ( filePoint ), pointVals );

			for ( int dimension = 0; dimension < reducedDimensionCount; dimension++ )
			{
				dataset.sDataReduced[filePoint][dimension] = pointVals[dimension];
			}
		}
	}

}
