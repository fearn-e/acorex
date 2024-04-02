#pragma once

#include "UMAP.h"
#include "ofLog.h"

bool AcorexCorpus::UMAP::Fit ( AcorexCorpus::DataSet& dataset, const AcorexCorpus::ReductionSettings& settings )
{
    fluid::algorithm::UMAP algorithm;

	fluid::FluidDataSet<std::string, double, 1> fluidsetIN ( dataset.analysisSettings.currentDimensionCount );
	fluid::FluidDataSet<std::string, double, 1> fluidsetOUT ( settings.dimensionReductionTarget );

	std::vector<int> filePointLength ( dataset.analysisSettings.bTime ? dataset.currentPointCount : 0, false );

	CorpusToFluid ( fluidsetIN, dataset, filePointLength );

	fluidsetOUT = algorithm.train ( fluidsetIN, 15, settings.dimensionReductionTarget, 0.1, settings.maxIterations, 0.1 );

	FluidToCorpus ( dataset, fluidsetOUT, filePointLength, settings.dimensionReductionTarget );

	dataset.analysisSettings.hasBeenReduced = true;

    return true;
}

void AcorexCorpus::UMAP::CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const AcorexCorpus::DataSet& dataset, std::vector<int>& filePointLength )
{
	if ( dataset.analysisSettings.bTime )
	{	
		int fileMarker = 0;

		for ( int file = 0; file < dataset.time.raw.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < dataset.time.raw[file].size ( ); timepoint++ )
			{
				fluid::RealVector point ( dataset.time.raw[file][timepoint].size ( ) );

				for ( int dimension = 0; dimension < dataset.time.raw[file][timepoint].size ( ); dimension++ )
				{
					point[dimension] = dataset.time.raw[file][timepoint][dimension];
				}

				fluidset.add ( std::to_string ( fileMarker + timepoint ), point );
			}

			filePointLength[file] = dataset.time.raw[file].size ( );
			fileMarker += dataset.time.raw[file].size ( );
		}
	}
	else
	{
		for ( int file = 0; file < dataset.stats.raw.size ( ); file++ )
		{
			fluid::RealVector point ( dataset.analysisSettings.currentDimensionCount );
			
			for ( int dimension = 0; dimension < dataset.stats.raw[file].size ( ); dimension++ )
			{
				for ( int statistic = 0; statistic < dataset.stats.raw[file][dimension].size ( ); statistic++ )
				{
					point[( dimension * DATA_NUM_STATS ) + statistic] = dataset.stats.raw[file][dimension][statistic];
				}
			}

			fluidset.add ( std::to_string ( file ), point );
		}
	}
}

void AcorexCorpus::UMAP::FluidToCorpus ( AcorexCorpus::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount )
{
	if ( dataset.analysisSettings.bTime )
	{
		dataset.time.raw.clear ( );

		int fileMarker = 0;

		for ( int filePoint = 0; filePoint < filePointLength.size ( ); filePoint++ )
		{
			dataset.time.raw.push_back ( std::vector<std::vector<double>> ( ) );

			for ( int timePoint = 0; timePoint < filePointLength[filePoint]; timePoint++ )
			{
				dataset.time.raw[filePoint].push_back ( std::vector<double> ( reducedDimensionCount, 0.0 ) );

				fluid::RealVector pointVals ( reducedDimensionCount );
				fluidset.get ( std::to_string ( fileMarker + timePoint ), pointVals );

				for ( int dimension = 0; dimension < reducedDimensionCount; dimension++ )
				{
					dataset.time.raw[filePoint][timePoint][dimension] = pointVals[dimension];
				}
			}

			fileMarker += filePointLength[filePoint];
		}
	}
	else
	{
		dataset.stats.raw.clear ( );
		dataset.stats.reduced.clear ( );

		for ( int filePoint = 0; filePoint < fluidset.size ( ); filePoint++ )
		{
			dataset.stats.reduced.push_back ( std::vector<double> ( reducedDimensionCount, 0.0 ) );

			fluid::RealVector pointVals ( reducedDimensionCount );
			fluidset.get ( std::to_string ( filePoint ), pointVals );

			for ( int dimension = 0; dimension < reducedDimensionCount; dimension++ )
			{
				dataset.stats.reduced[filePoint][dimension] = pointVals[dimension];
			}
		}
	}

}
