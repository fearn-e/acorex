/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "./PointPicker.h"
#include "./SpaceDefs.h"

#include <ofGraphics.h>
#include <of3DGraphics.h>

using namespace Acorex;

void Explorer::PointPicker::Initialise ( const Utils::DataSet& dataset, const Utils::DimensionBounds& dimensionBounds )
{
	std::lock_guard<std::mutex> lock ( mPointPickerMutex );

	mFullFluidSet = fluid::FluidDataSet<std::string, double, 1> ( dataset.dimensionNames.size ( ) );
	mLiveFluidSet = fluid::FluidDataSet<std::string, double, 1> ( 3 );

	bPicker = false; b3D = true; bSkipTraining = true; bNearestMouseCheckNeeded = false;

	bDimensionsFilled[0] = false; bDimensionsFilled[1] = false; bDimensionsFilled[2] = false;
	mDimensionsIndices[0] = -1; mDimensionsIndices[1] = -1; mDimensionsIndices[2] = -1;

	mNearestPoint = -1; mNearestDistance = -1; mNearestPointFile = -1; mNearestPointTime = -1;

	if ( mCorpusFileLookUp.size ( ) > 0 ) { mCorpusFileLookUp.clear ( ); }
	if ( mCorpusTimeLookUp.size ( ) > 0 ) { mCorpusTimeLookUp.clear ( ); }

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

    std::vector<int> tempVector;
    
	mDatasetConversion.CorpusToFluid ( mFullFluidSet, scaledDataset, tempVector );

	if ( !bListenersAdded )
	{
		ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::PointPicker::MouseMoved );
		ofAddListener ( ofEvents ( ).keyReleased, this, &Explorer::PointPicker::KeyEvent );
		ofAddListener ( ofEvents ( ).mouseReleased, this, &Explorer::PointPicker::MouseReleased );
		bListenersAdded = true;
	}
}

void Explorer::PointPicker::Train ( int dimensionIndex, Utils::Axis axis, bool none )
{
	std::lock_guard<std::mutex> lock ( mPointPickerMutex );

	if ( axis == Utils::Axis::X ) { bDimensionsFilled[0] = !none; mDimensionsIndices[0] = dimensionIndex; }
	else if ( axis == Utils::Axis::Y ) { bDimensionsFilled[1] = !none; mDimensionsIndices[1] = dimensionIndex; }
	else if ( axis == Utils::Axis::Z ) { bDimensionsFilled[2] = !none; mDimensionsIndices[2] = dimensionIndex; }
	else { return; }

	int dimsFilled = bDimensionsFilled[0] + bDimensionsFilled[1] + bDimensionsFilled[2];
	if ( dimsFilled < 2 ) { bTrained = false; return; }

	if ( axis == Utils::Axis::Z ) { bSkipTraining = false; }
	if ( bSkipTraining ) { return; }

	mLiveFluidSet = fluid::FluidDataSet<std::string, double, 1> ( dimsFilled );

	for ( int point = 0; point < mFullFluidSet.size ( ); point++ )
	{
		fluid::RealVector pointData ( dimsFilled );
		if ( dimsFilled == 3 || bDimensionsFilled[2] == false )
		{
			for ( int dim = 0; dim < dimsFilled; dim++ )
			{
				pointData[dim] = mFullFluidSet.get ( mFullFluidSet.getIds ( )[point] )[mDimensionsIndices[dim]];
			}
		}
		else if ( bDimensionsFilled[1] == false )
		{
			pointData[0] = mFullFluidSet.get ( mFullFluidSet.getIds ( )[point] )[mDimensionsIndices[0]];
			pointData[1] = mFullFluidSet.get ( mFullFluidSet.getIds ( )[point] )[mDimensionsIndices[2]];
		}
		else if ( bDimensionsFilled[0] == false )
		{
			pointData[0] = mFullFluidSet.get ( mFullFluidSet.getIds ( )[point] )[mDimensionsIndices[1]];
			pointData[1] = mFullFluidSet.get ( mFullFluidSet.getIds ( )[point] )[mDimensionsIndices[2]];
		}

		mLiveFluidSet.add ( mFullFluidSet.getIds ( )[point], pointData );
	}

	ofLogNotice ( "PointPicker" ) << "Training KDTree...";
	mKDTree = fluid::algorithm::KDTree ( mLiveFluidSet );
	ofLogNotice ( "PointPicker" ) << "KDTree Trained.";
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
		ofRemoveListener ( ofEvents ( ).keyReleased, this, &Explorer::PointPicker::KeyEvent );
		ofRemoveListener ( ofEvents ( ).mouseReleased, this, &Explorer::PointPicker::MouseReleased );
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

void Explorer::PointPicker::Draw ( )
{
	if ( bDebug )
	{
		if ( mNearestPoint != -1 )
		{
			ofDrawBitmapStringHighlight ( "Nearest Point: " + std::to_string ( mNearestPoint ), 20, ofGetHeight ( ) - 100 );
			ofDrawBitmapStringHighlight ( "Nearest Distance: " + std::to_string ( mNearestDistance ), 20, ofGetHeight ( ) - 80 );
		}

		ofEnableDepthTest ( );
		mCamera->begin ( );

		ofSetColor ( 150, 150, 255, 125 );
		for ( int i = 0; i < testPoints.size ( ); i++ )
		{
			ofDrawSphere ( testPoints[i], testRadii[i] );
		}

		ofSetColor ( 255, 255, 255, 25 );
		for ( int i = 0; i < testPointsOutOfRange.size ( ); i++ )
		{
			ofDrawSphere ( testPointsOutOfRange[i], testRadiiOutOfRange[i] );
		}

		mCamera->end ( );
		ofDisableDepthTest ( );
	}
}

void Explorer::PointPicker::FindNearestToMouse ( )
{
	if ( !bClicked ) { return; }
	bClicked = false;

	if ( !bPicker && !bTrained && !bNearestMouseCheckNeeded ) { return; }
	bNearestMouseCheckNeeded = false;

	std::lock_guard<std::mutex> lock ( mPointPickerMutex );

	mNearestPoint = -1; mNearestPointFile = -1; mNearestPointTime = -1;
	mNearestDistance = std::numeric_limits<double>::max ( );

	int mouseX = ofGetMouseX ( );
	int mouseY = ofGetMouseY ( );

	if ( !b3D )
	{
		// 2D nearest

		glm::vec3 rayPosition = mCamera->screenToWorld ( glm::vec3 ( mouseX, mouseY, 0 ) );
		glm::vec2 rayPosition2D;
		
		if ( !bDimensionsFilled[0] ) { rayPosition2D.x = rayPosition.y; rayPosition2D.y = rayPosition.z; rayPosition.x = 0; }
		if ( !bDimensionsFilled[1] ) { rayPosition2D.x = rayPosition.x; rayPosition2D.y = rayPosition.z; rayPosition.y = 0; }
		if ( !bDimensionsFilled[2] ) { rayPosition2D.x = rayPosition.x; rayPosition2D.y = rayPosition.y; rayPosition.z = 0; }

		fluid::RealVector query ( 2 );

		query[0] = ofMap ( rayPosition2D.x, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[1] = ofMap ( rayPosition2D.y, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );

		double maxAllowedDistance = ofMap ( mCamera->getScale ( ).x, SpaceDefs::mZoomMin2D, SpaceDefs::mZoomMax2D, maxAllowedDistanceNear * 1.5, maxAllowedDistanceFar * 1.5 );

		auto [dist, id] = mKDTree.kNearest ( query, 1, maxAllowedDistance );

		if ( dist.size ( ) == 0 ) { return; }

		if ( dist[0] < mNearestDistance )
		{
			mNearestDistance = dist[0];
			mNearestPoint = std::stoi ( *id[0] );
			mNearestPointFile = mCorpusFileLookUp[mNearestPoint];
			if ( mCorpusTimeLookUp.size ( ) > 0 ) { mNearestPointTime = mCorpusTimeLookUp[mNearestPoint]; }
		}

		return;
	}

	// 3D nearest
	
	double desiredRayLength = 15000.0f;
	double rayLength = 0.0f;
	std::vector<double> rayPointSpacing;
	
	do
	{
		double maxAllowedDistance = ofMap ( rayLength, 0.0f, desiredRayLength, 0.01, 0.05, false );
		rayPointSpacing.push_back ( maxAllowedDistance );
		rayLength += maxAllowedDistance * ( SpaceDefs::mSpaceMax - SpaceDefs::mSpaceMin );
	} while ( rayLength < desiredRayLength );

	glm::vec3 rayDirection = mCamera->screenToWorld ( glm::vec3 ( (float)mouseX, (float)mouseY, 0.0f ) );
	rayDirection = glm::normalize ( rayDirection - mCamera->getPosition ( ) );
	double depth = 0.0f;

	if ( testPoints.size ( ) > 0 ) { testPoints.clear ( ); }
	if ( testRadii.size ( ) > 0 ) { testRadii.clear ( ); }
	if ( testPointsOutOfRange.size ( ) > 0 ) { testPointsOutOfRange.clear ( ); }
	if ( testRadiiOutOfRange.size ( ) > 0 ) { testRadiiOutOfRange.clear ( ); }

	for ( int rayPoint = 1; rayPoint < rayPointSpacing.size ( ); rayPoint++ )
	{
		depth += rayPointSpacing[rayPoint] * ( SpaceDefs::mSpaceMax - SpaceDefs::mSpaceMin );

		glm::vec3 rayPointPosition = mCamera->getPosition ( ) + glm::vec3 ( rayDirection.x * depth,
																			rayDirection.y * depth, 
																			rayDirection.z * depth );

		if ( rayPointPosition.x < ( SpaceDefs::mSpaceMin - 250 ) || rayPointPosition.x > ( SpaceDefs::mSpaceMax + 250 ) ||
			rayPointPosition.y < ( SpaceDefs::mSpaceMin - 250 ) || rayPointPosition.y > ( SpaceDefs::mSpaceMax + 250 ) ||
			rayPointPosition.z < ( SpaceDefs::mSpaceMin - 250 ) || rayPointPosition.z > ( SpaceDefs::mSpaceMax + 250 ) )
		{
			if ( bDebug )
			{
				testPointsOutOfRange.push_back ( rayPointPosition );
				testRadiiOutOfRange.push_back ( rayPointSpacing[rayPoint] * ( SpaceDefs::mSpaceMax - SpaceDefs::mSpaceMin ) );
			}
			continue; 
		}

		if ( bDebug )
		{
			testPoints.push_back ( rayPointPosition );
			testRadii.push_back ( rayPointSpacing[rayPoint] * (SpaceDefs::mSpaceMax - SpaceDefs::mSpaceMin) );
		}

		fluid::RealVector query ( 3 );

		query[0] = ofMap ( rayPointPosition.x, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[1] = ofMap ( rayPointPosition.y, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[2] = ofMap ( rayPointPosition.z, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );

		auto [dist, id] = mKDTree.kNearest ( query, 1, rayPointSpacing[rayPoint] );

		if ( dist.size ( ) == 0 ) { continue; }

		if ( dist[0] < mNearestDistance )
		{
			mNearestDistance = dist[0];
			mNearestPoint = std::stoi ( *id[0] );
			mNearestPointFile = mCorpusFileLookUp[mNearestPoint];
			if ( mCorpusTimeLookUp.size ( ) > 0 ) { mNearestPointTime = mCorpusTimeLookUp[mNearestPoint]; }
		}
	}
}

bool Explorer::PointPicker::FindNearestToPosition ( const glm::vec3& position, Utils::PointFT& nearestPoint, Utils::PointFT currentPoint, 
													int maxAllowedDistanceSpaceX1000, int maxAllowedTargets, bool sameFileAllowed,
													int minTimeDiffSameFile, int remainingSamplesRequired, const Utils::AudioData& audioSet, size_t hopSize )
{
	if ( maxAllowedDistanceSpaceX1000 == 0 ) { return false; }

	if ( mPointPickerMutex.try_lock ( ) )
	{
		std::lock_guard<std::mutex> lock ( mPointPickerMutex, std::adopt_lock );

		double maxAllowedDistanceSpace = (double)maxAllowedDistanceSpaceX1000 / 1000.0;

		if ( !b3D )
		{
			// 2D nearest

			glm::vec2 position2D;

			if ( !bDimensionsFilled[0] ) { position2D.x = position.y; position2D.y = position.z; }
			if ( !bDimensionsFilled[1] ) { position2D.x = position.x; position2D.y = position.z; }
			if ( !bDimensionsFilled[2] ) { position2D.x = position.x; position2D.y = position.y; }

			fluid::RealVector query ( 2 );

			query[0] = ofMap ( position2D.x, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
			query[1] = ofMap ( position2D.y, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );

			auto [dist, id] = mKDTree.kNearest ( query, maxAllowedTargets, maxAllowedDistanceSpace );

			if ( dist.size ( ) == 0 ) { return false; }

			double nearestDistance = std::numeric_limits<double>::max ( );
			bool jumpFound = false;
			
			for ( int i = 0; i < dist.size ( ); i++ )
			{
				if ( dist[i] < nearestDistance )
				{
					int point = std::stoi ( *id[i] );
					if ( !sameFileAllowed && mCorpusFileLookUp[point] == currentPoint.file ) { continue; } // skip if jumping would jump to the same file and the option is not allowed
					size_t timeDiff = mCorpusTimeLookUp[point] > currentPoint.time ? mCorpusTimeLookUp[point] - currentPoint.time : currentPoint.time - mCorpusTimeLookUp[point];
					if ( sameFileAllowed && mCorpusFileLookUp[point] == currentPoint.file && timeDiff < minTimeDiffSameFile ) { continue; } // skip if jumping would jump to the same file and the time difference is too small

					if ( audioSet.raw[mCorpusFileLookUp[point]].getNumFrames ( ) - ( (size_t)mCorpusTimeLookUp[point] * hopSize ) < remainingSamplesRequired ) { continue; } // skip if there's not enough samples left in the file

					nearestDistance = dist[i];
					nearestPoint.file = mCorpusFileLookUp[point];
					nearestPoint.time = mCorpusTimeLookUp[point];
					jumpFound = true;
				}
			}

			return jumpFound;
		}

		// 3D nearest

		fluid::RealVector query ( 3 );

		query[0] = ofMap ( position.x, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[1] = ofMap ( position.y, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );
		query[2] = ofMap ( position.z, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax, 0.0, 1.0, false );

		auto [dist, id] = mKDTree.kNearest ( query, maxAllowedTargets, maxAllowedDistanceSpace );

		if ( dist.size ( ) == 0 ) { return false; }

		double nearestDistance = std::numeric_limits<double>::max ( );
		bool jumpFound = false;

		for ( int i = 0; i < dist.size ( ); i++ )
		{
			if ( dist[i] < nearestDistance )
			{
				int point = std::stoi ( *id[i] );
				if ( !sameFileAllowed && mCorpusFileLookUp[point] == currentPoint.file ) { continue; } // skip if jumping would jump to the same file and the option is not allowed
				size_t timeDiff = mCorpusTimeLookUp[point] > currentPoint.time ? mCorpusTimeLookUp[point] - currentPoint.time : currentPoint.time - mCorpusTimeLookUp[point];
				if ( sameFileAllowed && mCorpusFileLookUp[point] == currentPoint.file && timeDiff < minTimeDiffSameFile ) { continue; } // skip if jumping would jump to the same file and the time difference is too small

				if ( audioSet.raw[mCorpusFileLookUp[point]].getNumFrames ( ) - ((size_t)mCorpusTimeLookUp[point] * hopSize) < remainingSamplesRequired ) { continue; } // skip if there's not enough samples left in the file

				nearestDistance = dist[i];
				nearestPoint.file = mCorpusFileLookUp[point];
				nearestPoint.time = mCorpusTimeLookUp[point];
				jumpFound = true;
			}

			return jumpFound;
		}
	}

    return false;
}

void Explorer::PointPicker::KeyEvent ( ofKeyEventArgs& args )
{
	if ( args.type == ofKeyEventArgs::Type::Released )
	{
		if ( args.key == OF_KEY_F3 ) { bDebug = !bDebug; }
		else if ( args.key == OF_KEY_TAB ) { bPicker = !bPicker; }
	}
}

void Explorer::PointPicker::MouseReleased ( ofMouseEventArgs& args )
{
	if ( args.button == 2 ) { bClicked = true; }
}
