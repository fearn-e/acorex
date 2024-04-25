#include "./PointPicker.h"
#include "./SpaceDefs.h"

#include <ofGraphics.h>
#include <of3DGraphics.h>

using namespace Acorex;

void Explorer::PointPicker::Initialise ( const Utils::DataSet& dataset, const Utils::DimensionBounds& dimensionBounds )
{
	mFullFluidSet.resize ( dataset.analysisSettings.currentDimensionCount );
	mLiveFluidSet.resize ( 3 );

	Utils::DataSet scaledDataset = dataset;
	ScaleDataset ( scaledDataset, dimensionBounds );

	for ( int file = 0; file < dataset.fileList.size ( ); file++ )
	{
		if ( dataset.analysisSettings.bTime )
		{
			for ( int timepoint = 0; timepoint < dataset.time.raw[file].size ( ); timepoint++ )
			{
				mCorpusFileLookUp.push_back ( file );
				mCorpusTimeLookUp.push_back ( timepoint );
			}
		}
		else
		{
			mCorpusFileLookUp.push_back ( file );
		}
	}

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
	mLiveFluidSet = fluid::FluidDataSet<std::string, double, 1> ( 3 );

	if ( bDimensionsFilled[0] ) { mFluidSetQuery.addRange ( mDimensionsIndices[0], 1 ); }
	if ( bDimensionsFilled[1] ) { mFluidSetQuery.addRange ( mDimensionsIndices[1], 1 ); }
	if ( bDimensionsFilled[2] ) { mFluidSetQuery.addRange ( mDimensionsIndices[2], 1 ); }

	mFluidSetQuery.process ( mFullFluidSet, fluid::FluidDataSet < std::string, double, 1> ( ), mLiveFluidSet );

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
	if ( mNearestPoint != -1 )
	{
		ofDrawBitmapStringHighlight ( "Nearest Point: " + std::to_string ( mNearestPoint ), 20, ofGetHeight ( ) - 100 );
		ofDrawBitmapStringHighlight ( "Nearest Distance: " + std::to_string ( mNearestDistance ), 20, ofGetHeight ( ) - 80 );
	}

	ofEnableDepthTest ( );
	mCamera->begin ( );

	for ( auto& each : testDrawPoints )
	{
		ofSetColor ( 255, 0, 0 );
		ofDrawSphere ( each, 3.0f );
	}

	mCamera->end ( );
	ofDisableDepthTest ( );
}

void Explorer::PointPicker::FindNearest ( )
{
	testDrawPoints.clear ( );

	if ( !bTrained ) { return; }
	if ( !bNearestCheckNeeded ) { return; }
	bNearestCheckNeeded = false;

	mNearestPoint = -1; mNearestPointFile = -1; mNearestPointTime = -1;
	mNearestDistance = std::numeric_limits<double>::max ( );

	int mouseX = ofGetMouseX ( );
	int mouseY = ofGetMouseY ( );

	if ( !b3D )
	{
		// 2D nearest

		glm::vec3 rayPosition = mCamera->screenToWorld ( glm::vec3 ( mouseX, mouseY, 0 ) );
		
		if ( !bDimensionsFilled[0] ) { rayPosition.x = 0; }
		if ( !bDimensionsFilled[1] ) { rayPosition.y = 0; }
		if ( !bDimensionsFilled[2] ) { rayPosition.z = 0; }

		fluid::RealVector query ( 2 );

		query[0] = ofMap ( rayPosition.x, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[1] = ofMap ( rayPosition.y, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );

		auto [dist, id] = mKDTree.kNearest ( query, 1, maxAllowedDistance );

		if ( dist.size ( ) == 0 ) { return; }

		if ( dist[0] < mNearestDistance )
		{
			mNearestDistance = dist[0];
			mNearestPoint = std::stoi ( *id[0] );
			mNearestPointFile = mCorpusFileLookUp[mNearestPoint];
			if ( mCorpusTimeLookUp.size ( ) > 0 ) { mNearestPointTime = mCorpusTimeLookUp[mNearestPoint]; }
		}

		testDrawPoints.push_back ( rayPosition );

		return;
	}

	// 3D nearest

	double desiredRayLenght = 1000.0f;
	double rayPointSpacing = ofMap ( maxAllowedDistance, 0.0, 1.0, 0.0, SpaceDefs::mSpaceMax - SpaceDefs::mSpaceMin, false );
	int rayPointAmount = desiredRayLenght / rayPointSpacing;

	for ( int rayPoint = 0; rayPoint < rayPointAmount; rayPoint++ )
	{
		glm::vec3 rayDirection = mCamera->screenToWorld ( glm::vec3 ( mouseX, mouseY, 0 ) );
		rayDirection = glm::normalize ( rayDirection - mCamera->getPosition ( ) );
		double depth = rayPoint * rayPointSpacing;
		glm::vec3 rayPointPosition = mCamera->getPosition ( ) + glm::vec3 ( rayDirection.x * depth, 
																			rayDirection.y * depth, 
																			rayDirection.z * depth );

		fluid::RealVector query ( 3 );

		query[0] = ofMap ( rayPointPosition.x, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[1] = ofMap ( rayPointPosition.y, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[2] = ofMap ( rayPointPosition.z, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );

		auto [dist, id] = mKDTree.kNearest ( query, 1, maxAllowedDistance );

		if ( dist.size ( ) == 0 ) { continue; }

		if ( dist[0] < mNearestDistance )
		{
			mNearestDistance = dist[0];
			mNearestPoint = std::stoi ( *id[0] );
			mNearestPointFile = mCorpusFileLookUp[mNearestPoint];
			if ( mCorpusTimeLookUp.size ( ) > 0 ) { mNearestPointTime = mCorpusTimeLookUp[mNearestPoint]; }
		}

		testDrawPoints.push_back ( rayPointPosition );
	}
}