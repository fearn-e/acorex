#include "./PointPicker.h"

using namespace Acorex;

void Explorer::PointPicker::Initialise ( const Utils::DataSet& dataset, const Utils::DimensionBounds& dimensionBounds )
{
	mFullFluidSet.resize ( dataset.analysisSettings.currentDimensionCount );
	mLiveFluidSet.resize ( 3 );

	mDatasetConversion.CorpusToFluid ( mFullFluidSet, dataset, std::vector<int> ( ) );
}
void Explorer::PointPicker::ScaleDataset ( Utils::DataSet& scaledDataset, const Utils::DimensionBounds& dimensionBounds )
{
}

void Explorer::PointPicker::Train ( int dimensionIndex, Utils::Axis axis, bool none )
{
	if		( axis == Utils::Axis::X ) { mDimensionsFilled[0] = none ? false : true; mDimensionsIndices[0] = none ? -1 : dimensionIndex; }
	else if ( axis == Utils::Axis::Y ) { mDimensionsFilled[1] = none ? false : true; mDimensionsIndices[1] = none ? -1 : dimensionIndex; }
	else if ( axis == Utils::Axis::Z ) { mDimensionsFilled[2] = none ? false : true; mDimensionsIndices[2] = none ? -1 : dimensionIndex; }
	else { return; }

	int dimsFilled = mDimensionsFilled[0] + mDimensionsFilled[1] + mDimensionsFilled[2];
	if ( dimsFilled < 2 ) { bTrained = false; return; }

	if ( axis == Utils::Axis::Z ) { bSkipTraining = false; }
	if ( bSkipTraining ) { return; }

	mFluidSetQuery.clear ( );
	if ( mDimensionsFilled[0] ) { mFluidSetQuery.addRange ( mDimensionsIndices[0], 1 ); }
	if ( mDimensionsFilled[1] ) { mFluidSetQuery.addRange ( mDimensionsIndices[1], 1 ); }
	if ( mDimensionsFilled[2] ) { mFluidSetQuery.addRange ( mDimensionsIndices[2], 1 ); }
	mFluidSetQuery.process ( mFullFluidSet, fluid::FluidDataSet<std::string, double, 1> ( ), mLiveFluidSet );

	mKDTree = fluid::algorithm::KDTree( mLiveFluidSet );
	bTrained = true;
}