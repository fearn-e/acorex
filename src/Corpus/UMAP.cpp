#pragma once

#include "UMAP.h"
#include "ofLog.h"

bool corpus::UMAP::Fit ( fluid::FluidDataSet<std::string, double, 1>& datasetIN, fluid::FluidDataSet<std::string, double, 1>& datasetOUT )
{
    fluid::algorithm::UMAP algorithm;

    datasetOUT = algorithm.train ( datasetIN, 15, 3, 0.1, 200, 0.1 );

    return true;
}

bool corpus::UMAP::FitOverTime ( fluid::FluidDataSet<std::string, double, 1>& datasetIN, fluid::FluidDataSet<std::string, double, 1>& datasetOUT )
{
	fluid::algorithm::UMAP algorithm;

	fluid::FluidDataSet<std::string, double, 1> empty;
	fluid::FluidDataSet<std::string, double, 1> datasetNoTimeIN ( datasetIN.dims ( ) - 2 );
	fluid::FluidDataSet<std::string, double, 1> datasetNoTimeOUT ( 3 );
	fluid::FluidDataSet<std::string, double, 1> datasetTIME ( 2 );

	mQuery.clear ( );
	mQuery.addRange ( 0, 2 );
	mQuery.process ( datasetIN, empty, datasetTIME );

	mQuery.clear ( );
	mQuery.addRange ( 2, 24 );
	mQuery.process ( datasetIN, empty, datasetNoTimeIN );

	datasetNoTimeOUT = algorithm.train ( datasetNoTimeIN, 15, 3, 0.1, 200, 0.1 );

	mQuery.clear ( );
	mQuery.addRange ( 0, 3 );
	mQuery.process ( datasetNoTimeOUT, datasetTIME, datasetOUT );

	return true;
}