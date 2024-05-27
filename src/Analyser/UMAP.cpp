#include "./UMAP.h"
#include "ofLog.h"

using namespace Acorex;

bool Analyser::UMAP::Fit ( Utils::DataSet& dataset, const Utils::ReductionSettings& settings )
{
    fluid::algorithm::UMAP algorithm;

	std::vector<double> timeDimension;
	ExtractTimeDimension ( dataset, timeDimension );

	fluid::FluidDataSet<std::string, double, 1> fluidsetIN ( dataset.analysisSettings.currentDimensionCount );
	fluid::FluidDataSet<std::string, double, 1> fluidsetOUT ( settings.dimensionReductionTarget );

	std::vector<int> filePointLength ( dataset.analysisSettings.bTime ? dataset.fileList.size ( ) : 0, 0 );
	mConversion.CorpusToFluid ( fluidsetIN, dataset, filePointLength );

	fluid::index k = 15;

	if ( dataset.currentPointCount < 15 ) // TODO - double check exactly how k in UMAP works
	{
		k = dataset.currentPointCount;
	}

	ofLogNotice ( "UMAP" ) << "Training UMAP with " << dataset.currentPointCount << " points and " << dataset.analysisSettings.currentDimensionCount << " dimensions";

	fluidsetOUT = algorithm.train ( fluidsetIN, k, settings.dimensionReductionTarget, 0.1, settings.maxIterations, 0.1 ); // TODO - check if this can be parallelised

	ofLogNotice ( "UMAP" ) << "UMAP training complete";

	mConversion.FluidToCorpus ( dataset, fluidsetOUT, filePointLength, settings.dimensionReductionTarget );

	InsertTimeDimension ( dataset, timeDimension );

	dataset.analysisSettings.hasBeenReduced = true;

    return true;
}

void Analyser::UMAP::ExtractTimeDimension ( Utils::DataSet& dataset, std::vector<double>& timeDimension )
{
	if ( !dataset.analysisSettings.bTime ) { return; }

	dataset.dimensionNames.erase ( dataset.dimensionNames.begin ( ) );
    dataset.analysisSettings.currentDimensionCount -= 1;
    
	for ( int file = 0; file < dataset.fileList.size ( ); file++ )
	{
		for ( int timepoint = 0; timepoint < dataset.time.raw[file].size ( ); timepoint++ )
		{
			timeDimension.push_back ( dataset.time.raw[file][timepoint][0] );
			dataset.time.raw[file][timepoint].erase ( dataset.time.raw[file][timepoint].begin ( ) );
		}
	}
}

void Analyser::UMAP::InsertTimeDimension ( Utils::DataSet& dataset, const std::vector<double>& timeDimension )
{
	if ( !dataset.analysisSettings.bTime ) { return; }

	dataset.dimensionNames.insert ( dataset.dimensionNames.begin ( ), "Time" );
    dataset.analysisSettings.currentDimensionCount += 1;
    
	unsigned long long int insertIndex = 0;

	for ( int file = 0; file < dataset.fileList.size ( ); file++ )
	{
		for ( int timepoint = 0; timepoint < dataset.time.raw[file].size ( ); timepoint++ )
		{
			dataset.time.raw[file][timepoint].insert ( dataset.time.raw[file][timepoint].begin ( ), timeDimension[insertIndex] );
			insertIndex++;
		}
	}
}