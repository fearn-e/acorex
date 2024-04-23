#include "./UMAP.h"
#include "ofLog.h"

using namespace Acorex;

bool Analyser::UMAP::Fit ( Utils::DataSet& dataset, const Utils::ReductionSettings& settings )
{
    fluid::algorithm::UMAP algorithm;

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

	dataset.analysisSettings.hasBeenReduced = true;

    return true;
}