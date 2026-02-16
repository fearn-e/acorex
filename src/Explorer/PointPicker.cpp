/*
The MIT License (MIT)

Copyright (c) 2024-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Explorer/PointPicker.h"

#include "Explorer/SpaceDefs.h"
#include "Utilities/TemporaryKeybinds.h"

#include <ofGraphics.h>
#include <of3DGraphics.h>

using namespace Acorex;

Explorer::PointPicker::PointPicker ( )
    :   bListenersAdded ( false ), bDebug (false ),
        bTrained ( false ), bSkipTraining ( true ),
        b3D ( true ), bPicker ( false ), bClicked ( false ), bNearestMouseCheckNeeded ( false ),
        bDimensionsFilled { false, false, false }, mDimensionsIndices { -1, -1, -1 },
        mNearestPoint ( -1 ), mNearestDistance ( -1 ),
        maxAllowedDistanceFar ( 0.05 ), maxAllowedDistanceNear ( 0.01 ),
        mNearestPointFile ( -1 ), mNearestPointTime ( -1 )
{
    mRandomGen = std::mt19937 ( std::random_device ( ) () );
}

void Explorer::PointPicker::Initialise ( const Utilities::DataSet& dataset, const Utilities::DimensionBounds& dimensionBounds )
{
    Clear ( );

    std::lock_guard<std::mutex> lock ( mPointPickerMutex );

    mFullFluidSet = fluid::FluidDataSet<std::string, double, 1> ( dataset.dimensionNames.size ( ) );
    mLiveFluidSet = fluid::FluidDataSet<std::string, double, 1> ( 3 );

    Utilities::DataSet scaledDataset = dataset;
    ScaleDataset ( scaledDataset, dimensionBounds );

    for ( int file = 0; file < dataset.fileList.size ( ); file++ )
    {
        for ( int timepoint = 0; timepoint < dataset.trails.raw[file].size ( ); timepoint++ )
        {
            mCorpusFileLookUp.push_back ( file );
            mCorpusTimeLookUp.push_back ( timepoint );
        }
    }

    std::vector<int> tempVector;
    
    mDatasetConversion.CorpusToFluid ( mFullFluidSet, scaledDataset, tempVector );

    AddListeners ( );
}

void Explorer::PointPicker::Clear ( )
{
    std::lock_guard<std::mutex> lock ( mPointPickerMutex );

    mFullFluidSet = fluid::FluidDataSet<std::string, double, 1> ( 0 );
    mLiveFluidSet = fluid::FluidDataSet<std::string, double, 1> ( 0 );

    bTrained = false; bSkipTraining = true;
    b3D = true; bPicker = false; bClicked = false; bNearestMouseCheckNeeded = false;

    bDimensionsFilled[0] = false; bDimensionsFilled[1] = false; bDimensionsFilled[2] = false;
    mDimensionsIndices[0] = -1; mDimensionsIndices[1] = -1; mDimensionsIndices[2] = -1;

    mNearestPoint = -1; mNearestDistance = -1; mNearestPointFile = -1; mNearestPointTime = -1;

    mCorpusFileLookUp.clear ( );
    mCorpusTimeLookUp.clear ( );

    RemoveListeners ( );
}

void Explorer::PointPicker::Train ( int dimensionIndex, Utilities::Axis axis, bool none )
{
    std::lock_guard<std::mutex> lock ( mPointPickerMutex );

    if ( axis == Utilities::Axis::X ) { bDimensionsFilled[0] = !none; mDimensionsIndices[0] = dimensionIndex; }
    else if ( axis == Utilities::Axis::Y ) { bDimensionsFilled[1] = !none; mDimensionsIndices[1] = dimensionIndex; }
    else if ( axis == Utilities::Axis::Z ) { bDimensionsFilled[2] = !none; mDimensionsIndices[2] = dimensionIndex; }
    else { return; }

    int dimsFilled = bDimensionsFilled[0] + bDimensionsFilled[1] + bDimensionsFilled[2];
    if ( dimsFilled < 2 ) { bTrained = false; return; }

    // TODO - take a closer look at this later.
    // it seems like this makes bSkipTraining get set to false once when a corpus is first opened, as it gets 3 Train calls, for the X, Y, and Z dimensions.
    // but this means it wouldn't work for a corpus that had only 2 dimensions to load to begin with?
    // and if i ever accidentally set bSkipTraining back to true anywhere other than when loading a new corpus, this could just break training
    // split this function into Train and SetDimension? - call SetDimension when first loading, then Train only once and then at runtime when changes are applied to the corpus?
    if ( axis == Utilities::Axis::Z ) { bSkipTraining = false; }
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
    ofLogVerbose ( "PointPicker" ) << "KDTree Trained.";
    bTrained = true;

    if ( dimsFilled == 2 ) { b3D = false; }
    if ( dimsFilled == 3 ) { b3D = true; }
}

void Explorer::PointPicker::Exit ( )
{
    RemoveListeners ( );
}

void Explorer::PointPicker::AddListeners ( )
{
    if ( bListenersAdded ) { return; }
    ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::PointPicker::MouseMoved );
    ofAddListener ( ofEvents ( ).keyReleased, this, &Explorer::PointPicker::KeyEvent );
    ofAddListener ( ofEvents ( ).mouseReleased, this, &Explorer::PointPicker::MouseReleased );
    bListenersAdded = true;
}

void Explorer::PointPicker::RemoveListeners ( )
{
    if ( !bListenersAdded ) { return; }
    ofRemoveListener ( ofEvents ( ).mouseMoved, this, &Explorer::PointPicker::MouseMoved );
    ofRemoveListener ( ofEvents ( ).keyReleased, this, &Explorer::PointPicker::KeyEvent );
    ofRemoveListener ( ofEvents ( ).mouseReleased, this, &Explorer::PointPicker::MouseReleased );
    bListenersAdded = false;
}

void Explorer::PointPicker::ScaleDataset ( Utilities::DataSet& scaledDataset, const Utilities::DimensionBounds& dimensionBounds )
{
    for ( int file = 0; file < scaledDataset.trails.raw.size ( ); file++ )
    {
        for ( int timepoint = 0; timepoint < scaledDataset.trails.raw[file].size ( ); timepoint++ )
        {
            for ( int dimension = 0; dimension < scaledDataset.dimensionNames.size ( ); dimension++ )
            {
                scaledDataset.trails.raw[file][timepoint][dimension] = ofMap (
                    scaledDataset.trails.raw[file][timepoint][dimension],
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
            mNearestPointTime = mCorpusTimeLookUp[mNearestPoint];
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

    testPoints.clear ( );
    testRadii.clear ( );
    testPointsOutOfRange.clear ( );
    testRadiiOutOfRange.clear ( );

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
            mNearestPointTime = mCorpusTimeLookUp[mNearestPoint];
        }
    }
}

// TODO - revisit this function for any performance improvements
bool Explorer::PointPicker::FindNearestToPosition ( const glm::vec3& position, Utilities::PointFT& nearestPoint, Utilities::PointFT currentPoint, 
                                                    int maxAllowedDistanceSpaceX1000, int maxAllowedTargets, bool sameFileAllowed,
                                                    int minTimeDiffSameFile, int remainingSamplesRequired, const Utilities::AudioData& audioSet, size_t hopSize )
{
    if ( !bTrained ) { return false; }

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

void Explorer::PointPicker::FindRandom ( )
{
    if ( !bTrained ) { return; }

    std::uniform_int_distribution<int> dist ( 0, (int)mCorpusFileLookUp.size ( ) - 1 );
    int randomPoint = dist ( mRandomGen );

    mNearestPoint = randomPoint;
    mNearestDistance = 0.0;

    mNearestPointFile = mCorpusFileLookUp[randomPoint];
    mNearestPointTime = mCorpusTimeLookUp[randomPoint];
}

void Explorer::PointPicker::KeyEvent ( ofKeyEventArgs& args )
{
    if ( args.type == ofKeyEventArgs::Type::Released )
    {
        if ( args.key == ACOREX_KEYBIND_TOGGLE_DEBUG_VIEW ) { bDebug = !bDebug; }
        else if ( args.key == ACOREX_KEYBIND_TOGGLE_POINT_PICKER ) { bPicker = !bPicker; }
        else if ( args.key == ACOREX_KEYBIND_PICK_RANDOM_POINT ) { FindRandom ( ); }
    }
}

void Explorer::PointPicker::MouseReleased ( ofMouseEventArgs& args )
{
    if ( args.button == OF_MOUSE_BUTTON_RIGHT ) { bClicked = true; }
}
