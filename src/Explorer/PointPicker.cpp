#include "./PointPicker.h"

using namespace Acorex;

void Explorer::PointPicker::Initialise ( const Utils::DataSet& dataset, const Utils::DimensionBounds& dimensionBounds )
{
	mFullFluidSet.resize ( dataset.analysisSettings.currentDimensionCount );
	mLiveFluidSet.resize ( 3 );

	Utils::DataSet scaledDataset = dataset;
	ScaleDataset ( scaledDataset, dimensionBounds );

	mDatasetConversion.CorpusToFluid ( mFullFluidSet, scaledDataset, std::vector<int> ( ) );

	if ( !bListenersAdded )
	{
		ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::PointPicker::MouseMoved );
		bListenersAdded = true;
	}
}

void Explorer::PointPicker::Train ( int dimensionIndex, Utils::Axis axis, bool none )
{
	if ( axis == Utils::Axis::X ) { bDimensionsFilled[0] = none ? false : true; mDimensionsIndices[0] = dimensionIndex; }
	else if ( axis == Utils::Axis::Y ) { bDimensionsFilled[1] = none ? false : true; mDimensionsIndices[1] = dimensionIndex; }
	else if ( axis == Utils::Axis::Z ) { bDimensionsFilled[2] = none ? false : true; mDimensionsIndices[2] = dimensionIndex; }
	else { return; }

	int dimsFilled = bDimensionsFilled[0] + bDimensionsFilled[1] + bDimensionsFilled[2];
	if ( dimsFilled < 2 ) { bTrained = false; return; }

	if ( axis == Utils::Axis::Z ) { bSkipTraining = false; }
	if ( bSkipTraining ) { return; }

	mFluidSetQuery.clear ( );

	if ( bDimensionsFilled[0] ) { mFluidSetQuery.addRange ( mDimensionsIndices[0], 1 ); }
	if ( bDimensionsFilled[1] ) { mFluidSetQuery.addRange ( mDimensionsIndices[1], 1 ); }
	if ( bDimensionsFilled[2] ) { mFluidSetQuery.addRange ( mDimensionsIndices[2], 1 ); }

	mFluidSetQuery.process ( mFullFluidSet, fluid::FluidDataSet<std::string, double, 1> ( ), mLiveFluidSet );

	mKDTree = fluid::algorithm::KDTree ( mLiveFluidSet );
	bTrained = true;

	if ( dimsFilled == 2 ) { b3D = false; }
	if ( dimsFilled == 3 ) { b3D = true; }
}

void Explorer::PointPicker::Exit ( )
{
	RemoveListeners ( );
}

void Explorer::PointPicker::RemoveListeners ( )
{
	if ( bListenersAdded )
	{
		ofRemoveListener ( ofEvents ( ).mouseMoved, this, &Explorer::PointPicker::MouseMoved );
		bListenersAdded = false;
	}
}

void Explorer::PointPicker::ScaleDataset ( Utils::DataSet& scaledDataset, const Utils::DimensionBounds& dimensionBounds )
{
	if ( scaledDataset.analysisSettings.bTime )
	{
		for ( int file = 0; file < scaledDataset.time.raw.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < scaledDataset.time.raw[file].size ( ); timepoint++ )
			{
				for ( int dimension = 0; dimension < scaledDataset.dimensionNames.size ( ); dimension++ )
				{
					scaledDataset.time.raw[file][timepoint][dimension] = ofMap ( 
						scaledDataset.time.raw[file][timepoint][dimension],
						dimensionBounds.GetMinBound ( dimension ),
						dimensionBounds.GetMaxBound ( dimension ),
						0.0, 1.0, false );
				}
			}
		}
	}
	else if ( scaledDataset.analysisSettings.hasBeenReduced )
	{
		for ( int file = 0; file < scaledDataset.stats.reduced.size ( ); file++ )
		{
			for ( int dimension = 0; dimension < scaledDataset.dimensionNames.size ( ); dimension++ )
			{
				scaledDataset.stats.reduced[file][dimension] = ofMap (
					scaledDataset.stats.reduced[file][dimension],
					dimensionBounds.GetMinBound ( dimension ),
					dimensionBounds.GetMaxBound ( dimension ),
					0.0, 1.0, false );
			}
		}
	}
	else
	{
		for ( int file = 0; file < scaledDataset.stats.raw.size ( ); file++ )
		{
			for ( int dimension = 0; dimension < scaledDataset.stats.raw[file].size ( ); dimension++ )
			{
				int statistic = dimension % DATA_NUM_STATS;
				int dividedDimension = dimension / DATA_NUM_STATS;
				scaledDataset.stats.raw[file][dividedDimension][statistic] = ofMap (
					scaledDataset.stats.raw[file][dividedDimension][statistic],
					dimensionBounds.GetMinBound ( dimension ),
					dimensionBounds.GetMaxBound ( dimension ),
					0.0, 1.0, false );
			}
		}
	}
}

void Explorer::PointPicker::SlowUpdate ( )
{
	FindNearest ( );
}

void Explorer::PointPicker::Draw ( )
{
	//
}

void Explorer::PointPicker::FindNearest ( )
{
	if ( !bTrained ) { return; }
	if ( !bNearestCheckNeeded ) { return; }


}