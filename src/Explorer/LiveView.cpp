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

#include "Explorer/LiveView.h"

#include "Utilities/TemporaryKeybinds.h"

#include <ofLog.h>
#include <of3dGraphics.h>
#include <ofGraphics.h>
#include <of3dUtils.h>
#include <ofEvents.h>
#include <random>

#define TEMPORARY_ACOREX_VISUAL_TRAIL_FADE_UPDATE_INTERVAL 4
#define TEMPORARY_ACOREX_VISUAL_TRAIL_MAX_LENGTH 20

using namespace Acorex;

Explorer::LiveView::LiveView ( )
    : bListenersAdded ( false ),
    bDebug ( false ), bUserPaused ( false ), bDraw ( false ), bDrawAxes ( false ), bDrawCloud ( true ), bDrawCloudDark ( true ),
    b3D ( true ), bColorFullSpectrum ( false ),
    mKeyboardMoveState { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, // W, A, S, D, R, F, Q, E, Z, X
    mCamMoveSpeedScaleAdjusted ( SpaceDefs::mCamMoveSpeed ),
    deltaTime ( 0.1f ), lastUpdateTime ( 0 ),
    mDisabledAxis ( Utilities::Axis::NONE ), xLabel ( "X" ), yLabel ( "Y" ), zLabel ( "Z" ), colorDimension ( -1 ),
    mCamPivot ( ofPoint ( 0, 0, 0 ) ),
    mLastMouseX ( 0 ), mLastMouseY ( 0 )
{
    mPointPicker = std::make_shared<Explorer::PointPicker> ( );
    mAudioPlayback.SetPointPicker ( mPointPicker );

    mCamera = std::make_shared<ofCamera> ( );
    mPointPicker->SetCamera ( mCamera );

    mRandomGen = std::mt19937 ( std::random_device ( ) () );
}

void Explorer::LiveView::Initialise ( )
{
    Clear ( );

    glPointSize ( 3.0f ); // TODO - might be fine in the constructor instead

    Init3DCam ( );

    mDimensionBounds.CalculateBounds ( *mRawView->GetDataset ( ) );

    mAudioPlayback.SetDimensionBounds ( mDimensionBounds.GetBoundsData ( ) );
    
    mPointPicker->Initialise ( *mRawView->GetDataset ( ), mDimensionBounds );

    AddListeners ( );
}

void Explorer::LiveView::Clear ( )
{
    RemoveListeners ( );

    mAudioPlayback.ClearAndKillAudio ( );

    mPointPicker->Clear ( );

    mDimensionBounds.Clear ( );

    mCorpusMesh.clear ( );

    mPlayheads.clear ( );

    bDraw = false;
    b3D = true;
    bColorFullSpectrum = false;

    for ( auto& each : mKeyboardMoveState ) { each = false; }
    mCamMoveSpeedScaleAdjusted = SpaceDefs::mCamMoveSpeed;

    mDisabledAxis = Utilities::Axis::NONE;
    xLabel = "X"; yLabel = "Y"; zLabel = "Z";
    colorDimension = -1;
}

bool Explorer::LiveView::StartAudio ( std::pair<ofSoundDevice, int> audioSettings )
{
    bool audioOutputValidated = mAudioPlayback.StartRestartAudio ( mRawView->GetDataset ( )->analysisSettings.sampleRate, audioSettings.second, audioSettings.first );
    return audioOutputValidated;
}

void Explorer::LiveView::Exit ( )
{
    RemoveListeners ( );
    mPointPicker->Exit ( );
}

void Explorer::LiveView::AddListeners ( )
{
    if ( bListenersAdded ) { return; }
    ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
    ofAddListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
    ofAddListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
    ofAddListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
    ofAddListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
    ofAddListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
    ofAddListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
    bListenersAdded = true;
}

void Explorer::LiveView::RemoveListeners ( )
{
    if ( !bListenersAdded ) { return; }
    ofRemoveListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
    ofRemoveListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
    ofRemoveListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
    ofRemoveListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
    ofRemoveListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
    ofRemoveListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
    ofRemoveListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
    bListenersAdded = false;
}

// Process Functions ---------------------------

void Explorer::LiveView::Update ( )
{
    deltaTime = ofGetElapsedTimef ( ) - lastUpdateTime;
    lastUpdateTime = ofGetElapsedTimef ( );
    if ( !bDraw ) { return; }

    float keyboardMoveDelta = SpaceDefs::mKeyboardMoveSpeed * deltaTime;
    float keyboardRotateDelta = SpaceDefs::mKeyboardRotateSpeed * deltaTime;
    float keyboardZoomDelta = SpaceDefs::mKeyboardZoomSpeed * deltaTime;

    if ( b3D )
    {
        if ( mKeyboardMoveState[0] || mKeyboardMoveState[1] || mKeyboardMoveState[2] || mKeyboardMoveState[3] || mKeyboardMoveState[4] || mKeyboardMoveState[5] )
        {
            Pan3DCam (	( mKeyboardMoveState[1] - mKeyboardMoveState[3] ) * keyboardMoveDelta,
                        ( mKeyboardMoveState[4] - mKeyboardMoveState[5] ) * keyboardMoveDelta,
                        ( mKeyboardMoveState[2] - mKeyboardMoveState[0] ) * keyboardMoveDelta,
                        false );
            mPointPicker->SetNearestCheckNeeded ( );
        }
        else if ( mKeyboardMoveState[6] || mKeyboardMoveState[7] )
        {
            Rotate3DCam ( ( mKeyboardMoveState[6] - mKeyboardMoveState[7] ) * keyboardRotateDelta, 0, false );
            mPointPicker->SetNearestCheckNeeded ( );
        }
        else if ( mKeyboardMoveState[8] || mKeyboardMoveState[9] )
        {
            Zoom3DCam ( ( mKeyboardMoveState[8] - mKeyboardMoveState[9] ) * keyboardZoomDelta, false );
            mPointPicker->SetNearestCheckNeeded ( );
        }
    }
    else
    {
        if ( mKeyboardMoveState[0] || mKeyboardMoveState[1] || mKeyboardMoveState[2] || mKeyboardMoveState[3] )
        {
            float adjustedSpeed = mCamMoveSpeedScaleAdjusted * keyboardMoveDelta;
            mCamera->boom ( (mKeyboardMoveState[0] - mKeyboardMoveState[2]) * adjustedSpeed );
            mCamera->truck ( (mKeyboardMoveState[3] - mKeyboardMoveState[1]) * adjustedSpeed );
            mPointPicker->SetNearestCheckNeeded ( );
        }
        else if ( mKeyboardMoveState[8] || mKeyboardMoveState[9] )
        {
            Zoom2DCam ( ( mKeyboardMoveState[8] - mKeyboardMoveState[9] ) * keyboardZoomDelta, false );
            mPointPicker->SetNearestCheckNeeded ( );
        }
    }

    UpdatePlayheads ( );
}

void Explorer::LiveView::UpdatePlayheads ( )
{
    std::vector<Utilities::VisualPlayhead> playheadUpdates = mAudioPlayback.GetPlayheadInfo ( );

    // remove playheads that aren't in the update list
    for ( int i = 0; i < mPlayheads.size ( ); i++ )
    {
        if ( std::find_if ( playheadUpdates.begin ( ), playheadUpdates.end ( ), [this, i]( Utilities::VisualPlayhead& playhead ) { return playhead.playheadID == mPlayheads[i].playheadID; } ) == playheadUpdates.end ( ) )
        {
            ofLogVerbose ( "LiveView" ) << "Playhead " << mPlayheads[i].playheadID << " deleted";

            // if not the final one, move all after it back to the left
            int j = mPlayheads.size ( ) - 1; int end = i;
            while ( j > end )
            {
                mPlayheads[j].panelRect = mPlayheads[j - 1].panelRect;
                mPlayheads[j].playheadColorRect = mPlayheads[j - 1].playheadColorRect;
                mPlayheads[j].killButtonRect = mPlayheads[j - 1].killButtonRect;
                j--;
            }

            std::find_if ( mPlayheadTrails.begin ( ), mPlayheadTrails.end ( ), [ this, i ] ( Utilities::VisualPlayheadTrail& trail ) { return trail.playheadID == mPlayheads[i].playheadID; } )->Kill ( );

            mPlayheads.erase ( mPlayheads.begin ( ) + i );
            i--;
        }
    }

    // go through playheadUpdates and update the respective playheads or add new ones if the ID isn't found
    for ( int i = 0; i < playheadUpdates.size ( ); i++ )
    {
        auto it = std::find_if ( mPlayheads.begin ( ), mPlayheads.end ( ), [this, i, &playheadUpdates] ( Utilities::VisualPlayhead& playhead ) { return playhead.playheadID == playheadUpdates[i].playheadID; } );
        if ( it != mPlayheads.end ( ) )
        {
            ofColor color = it->color;
            ofRectangle panelRect = it->panelRect;
            ofRectangle playheadColorRect = it->playheadColorRect;
            ofRectangle killButtonRect = it->killButtonRect;
            bool highlight = it->highlight;

            *it = playheadUpdates[i];

            it->color = color;
            it->panelRect = panelRect;
            it->playheadColorRect = playheadColorRect;
            it->killButtonRect = killButtonRect;
            it->highlight = highlight;
        }
        else
        {
            ofLogVerbose ( "LiveView" ) << "Playhead " << playheadUpdates[i].playheadID << " added";

            mPlayheads.push_back ( playheadUpdates[i] );
            std::random_device rd;
            std::mt19937 gen ( rd ( ) );
            std::uniform_int_distribution<> dis ( 0, 255 );
            ofColor randomPlayheadColor = ofColor::fromHsb ( dis ( gen ), 255, 255 );

            mPlayheadTrails.push_back ( Utilities::VisualPlayheadTrail ( playheadUpdates[i].playheadID, randomPlayheadColor, TEMPORARY_ACOREX_VISUAL_TRAIL_MAX_LENGTH, TEMPORARY_ACOREX_VISUAL_TRAIL_FADE_UPDATE_INTERVAL ) );

            mPlayheads.back ( ).color = randomPlayheadColor;
            mPlayheads.back ( ).ResizeBox ( mPlayheads.size ( ) - 1, mLayout->getTopBarHeight ( ), ofGetHeight ( ), ofGetWidth ( ) );
        }
    }

    // calculate the 3D positions of the playheads
    for ( auto& playhead : mPlayheads )
    {
        size_t timeIndex = playhead.sampleIndex / mRawView->GetHopSize ( );
        playhead.position[0] = mCorpusMesh[playhead.fileIndex].getVertex ( timeIndex ).x;
        playhead.position[1] = mCorpusMesh[playhead.fileIndex].getVertex ( timeIndex ).y;
        playhead.position[2] = mCorpusMesh[playhead.fileIndex].getVertex ( timeIndex ).z;
    }

    // add new points to the playhead trails
    for ( auto& trail : mPlayheadTrails )
    {
        auto it = std::find_if ( mPlayheads.begin ( ), mPlayheads.end ( ), [ &trail ] ( Utilities::VisualPlayhead& playhead ) { return playhead.playheadID == trail.playheadID; } );
        if ( it != mPlayheads.end ( ) )
        {
            ofColor newColor = mCorpusMesh[it->fileIndex].getColor ( it->sampleIndex / mRawView->GetHopSize ( ) );
            trail.AddTrailPoint ( it->fileIndex, it->sampleIndex / mRawView->GetHopSize ( ), glm::vec3 ( it->position[0], it->position[1], it->position[2] ), newColor );
        }
    }

    // update playhead trails and remove any that have finished fading
    int currentTime = ofGetElapsedTimeMillis ( );
    for ( int i = 0; i < mPlayheadTrails.size ( ); i++ )
    {
        if ( mPlayheadTrails[i].Update ( currentTime ) )
        {
            mPlayheadTrails.erase ( mPlayheadTrails.begin ( ) + i );
            i--;
        }
    }
}

void Explorer::LiveView::SlowUpdate ( )
{
    mPointPicker->FindNearestToMouse ( );
}

void Explorer::LiveView::Draw ( )
{
    if ( !bDraw ) { return; }

    ofEnableDepthTest ( );
    ofEnableAlphaBlending ( );
    mCamera->begin ( );

    // Draw Axis ------------------------------
    if ( bDrawAxes )
    {
        ofSetColor ( 255, 255, 255 );
        if ( mDisabledAxis != Utilities::Axis::X ) { ofDrawLine ( { SpaceDefs::mSpaceMin, 0, 0 }, { SpaceDefs::mSpaceMax, 0, 0 } ); }
        if ( mDisabledAxis != Utilities::Axis::Y ) { ofDrawLine ( { 0, SpaceDefs::mSpaceMin, 0 }, { 0, SpaceDefs::mSpaceMax, 0 } ); }
        if ( mDisabledAxis != Utilities::Axis::Z ) { ofDrawLine ( { 0, 0, SpaceDefs::mSpaceMin }, { 0, 0, SpaceDefs::mSpaceMax } ); }

        if ( mDisabledAxis != Utilities::Axis::X ) { ofDrawBitmapString ( xLabel, { SpaceDefs::mSpaceMax, 0, 0 } ); }
        if ( mDisabledAxis != Utilities::Axis::Y ) { ofDrawBitmapString ( yLabel, { 0, SpaceDefs::mSpaceMax, 0 } ); }
        if ( mDisabledAxis != Utilities::Axis::Z ) { ofDrawBitmapString ( zLabel, { 0, 0, SpaceDefs::mSpaceMax } ); }
    }

    // Draw points, tails, playheads ------------------------------
    {
        ofEnableDepthTest ( );
        ofEnableAlphaBlending ( );

        if ( bDrawCloud && !bDrawCloudDark )
        {
            for ( int file = 0; file < mCorpusMesh.size ( ); file++ )
            {
                mCorpusMesh[file].enableColors ( );
                mCorpusMesh[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
                mCorpusMesh[file].draw ( );
                mCorpusMesh[file].setMode ( OF_PRIMITIVE_POINTS );
                mCorpusMesh[file].draw ( );
            }

            ofDisableDepthTest ( );
        }
        if ( bDrawCloudDark )
        {
            for ( int file = 0; file < mCorpusMesh.size ( ); file++ )
            {
                mCorpusMesh[file].disableColors ( );
                ofSetColor ( 255, 255, 255, 1 );
                mCorpusMesh[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
                mCorpusMesh[file].draw ( );
                mCorpusMesh[file].setMode ( OF_PRIMITIVE_POINTS );
                mCorpusMesh[file].draw ( );
            }

            ofDisableDepthTest ( );
        }

        for ( auto& trail : mPlayheadTrails )
        {
            trail.Draw ( );
        }

        for ( int i = 0; i < mPlayheads.size ( ); i++ )
        {
            ofColor color = mPlayheads[i].color; float size = 50;
            if ( mPlayheads[i].highlight ) { color = { 255, 255, 255, 255 }; size = 100; }

            ofSetColor ( color );
            glm::vec3 position = { mPlayheads[i].position[0], mPlayheads[i].position[1], mPlayheads[i].position[2] };
            ofDrawSphere ( position, size );
        }

        if ( mPointPicker->GetNearestMousePointFile ( ) != -1 )
        {
            ofSetColor ( 255, 255, 255 );
            glm::vec3 nearestPoint = mCorpusMesh[mPointPicker->GetNearestMousePointFile ( )].getVertex ( mPointPicker->GetNearestMousePointTime ( ) );
            ofDrawSphere ( nearestPoint, 25 );
        }

        //ofEnableAlphaBlending ( );
        //ofEnableDepthTest ( );
        //{
        //    if ( mPointPicker->GetNearestMousePointFile ( ) == -1 )
        //    {
        //        for ( int file = 0; file < mCorpusMesh.size ( ); file++ )
        //        {
        //            mCorpusMesh[file].enableColors ( );
        //            mCorpusMesh[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
        //            mCorpusMesh[file].draw ( );
        //            mCorpusMesh[file].setMode ( OF_PRIMITIVE_POINTS );
        //            mCorpusMesh[file].draw ( );
        //        }
        //    }
        //    else
        //    {
        //        for ( int file = 0; file < mCorpusMesh.size ( ); file++ )
        //        {
        //            if ( file == mPointPicker->GetNearestMousePointFile ( ) ) { continue; }
        //            mCorpusMesh[file].disableColors ( );
        //            ofSetColor ( 255, 255, 255, 25 );
        //            mCorpusMesh[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
        //            mCorpusMesh[file].draw ( );
        //            mCorpusMesh[file].setMode ( OF_PRIMITIVE_POINTS );
        //            mCorpusMesh[file].draw ( );
        //        }
        //
        //        mCorpusMesh[mPointPicker->GetNearestMousePointFile ( )].enableColors ( );
        //        mCorpusMesh[mPointPicker->GetNearestMousePointFile ( )].setMode ( OF_PRIMITIVE_LINE_STRIP );
        //        mCorpusMesh[mPointPicker->GetNearestMousePointFile ( )].draw ( );
        //        mCorpusMesh[mPointPicker->GetNearestMousePointFile ( )].setMode ( OF_PRIMITIVE_POINTS );
        //        mCorpusMesh[mPointPicker->GetNearestMousePointFile ( )].draw ( );
        //    }
        //
        //    for ( int i = 0; i < mPlayheads.size ( ); i++ )
        //    {
        //        ofDisableDepthTest ( );
        //        ofDisableAlphaBlending ( );
        //
        //        ofColor color = mPlayheads[i].color; float size = 50;
        //        if ( mPlayheads[i].highlight ) { color = { 255, 255, 255, 255 }; size = 100; }
        //
        //        ofSetColor ( color );
        //        glm::vec3 position = { mPlayheads[i].position[0], mPlayheads[i].position[1], mPlayheads[i].position[2] };
        //        ofDrawSphere ( position, size );
        //
        //        ofEnableAlphaBlending ( );
        //        ofEnableDepthTest ( );
        //    }
        //}
    }

    mCamera->end ( );
    ofDisableAlphaBlending ( );
    ofDisableDepthTest ( );

    // Draw Nearest Point -----------------------
    mPointPicker->Draw ( );
    if ( mPointPicker->GetNearestMousePointFile ( ) != -1 )
    {
        ofDrawBitmapStringHighlight ( "Point picked: " + std::to_string ( mPointPicker->GetNearestMousePointFile ( ) ) + ", " + std::to_string ( mPointPicker->GetNearestMousePointTime ( ) ), ofGetWidth ( ) - 200, ofGetHeight ( ) - 80 );        
        //ofDrawBitmapStringHighlight ( "Nearest File: " + mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )], 20, ofGetHeight ( ) - 60 );
        //std::string hopInfoSamps = std::to_string ( mPointPicker->GetNearestMousePointTime ( ) * mRawView->GetHopSize ( ) );
        //std::string hopInfoSecs = std::to_string ( mRawView->GetTrailData ( )->raw[mPointPicker->GetNearestMousePointFile ( )][mPointPicker->GetNearestMousePointTime ( )][0] );
        //ofDrawBitmapStringHighlight ( "Nearest Timepoint: " + hopInfoSamps + " samples, " + hopInfoSecs + "s", 20, ofGetHeight ( ) - 40 );
    }

    // Paused overlay ---------------------------
    if ( bUserPaused )
    {
        ofEnableAlphaBlending ( );

        //ofColor color = ofColor::fromHsb ( fmod ( ofGetElapsedTimef ( ) * 5, 255 ), 60, 120 );
        //ofSetColor ( color.r, color.g, color.b, 50 );
        ofSetColor ( 0, 0, 0, 75 );
        ofDrawRectangle ( 0, 0, ofGetWidth ( ), ofGetHeight ( ) );

        ofSetColor ( 255, 0, 0, 150 );
        ofDrawBitmapStringHighlight ( "PAUSED", ofGetWidth ( ) - 100, ofGetHeight ( ) - 40 );

        ofDisableAlphaBlending ( );
    }
}

// Sound Functions ------------------------------

void Explorer::LiveView::CreatePlayhead ( )
{
    if ( mPointPicker->GetNearestMousePointFile ( ) == -1 ) { return; }

    CreatePlayhead ( mPointPicker->GetNearestMousePointFile ( ), mPointPicker->GetNearestMousePointTime ( ) );

    return;
}

void Explorer::LiveView::CreatePlayhead ( size_t fileIndex, size_t timePointIndex )
{
    mAudioPlayback.CreatePlayhead ( fileIndex, timePointIndex );
}

void Explorer::LiveView::CreatePlayheadRandom ( )
{
    std::uniform_int_distribution<> disFile ( 0, mRawView->GetDataset ( )->fileList.size ( ) - 1 );
    size_t randomFile = disFile ( mRandomGen );
    std::uniform_int_distribution<> disTime ( 0, mRawView->GetTrailData ( )->raw[randomFile].size ( ) - 1 );
    size_t randomTime = disTime ( mRandomGen );
    CreatePlayhead ( randomFile, randomTime );
}

void Explorer::LiveView::PickRandomPoint ( )
{
    mPointPicker->FindRandom ( );
}

void Explorer::LiveView::KillPlayhead ( size_t playheadID )
{
    mAudioPlayback.KillPlayhead ( playheadID );
}

// Filler Functions ----------------------------

void Explorer::LiveView::CreatePoints ( )
{
    Utilities::TrailData* trails = mRawView->GetTrailData ( );

    for ( int file = 0; file < trails->raw.size ( ); file++ )
    {
        ofMesh mesh;
        for ( int timepoint = 0; timepoint < trails->raw[file].size ( ); timepoint++ )
        {
            mesh.addVertex ( { 0, 0, 0 } );
            ofColor color = ofColor::fromHsb ( 35, 255, 255 );
            mesh.addColor ( color );
        }
        mCorpusMesh.push_back ( mesh );
    }

    mAudioPlayback.SetCorpusMesh ( mCorpusMesh );

    bDraw = true;
}

void Explorer::LiveView::FillDimension ( int dimensionIndex, Utilities::Axis axis )
{
    std::string dimensionName = mRawView->GetDimensions ( )[dimensionIndex];
    if ( axis == Utilities::Axis::X ) { xLabel = dimensionName; }
    else if ( axis == Utilities::Axis::Y ) { yLabel = dimensionName; }
    else if ( axis == Utilities::Axis::Z ) { zLabel = dimensionName; }
    else if ( axis == Utilities::Axis::COLOR ) { colorDimension = dimensionIndex; }

    Utilities::TrailData* trails = mRawView->GetTrailData ( );

    double min = mDimensionBounds.GetMinBound ( dimensionIndex );
    double max = mDimensionBounds.GetMaxBound ( dimensionIndex );

    for ( int file = 0; file < trails->raw.size ( ); file++ )
    {
        for ( int timepoint = 0; timepoint < trails->raw[file].size ( ); timepoint++ )
        {
            double value = trails->raw[file][timepoint][dimensionIndex];

            //colors
            if ( axis == Utilities::Axis::COLOR )
            {
                if ( bColorFullSpectrum ) { value = ofMap ( value, min, max, SpaceDefs::mColorMin, SpaceDefs::mColorMax ); }
                else { value = ofMap ( value, min, max, SpaceDefs::mColorBlue, SpaceDefs::mColorRed ); }
                ofColor currentColor = mCorpusMesh[file].getColor ( timepoint );
                currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
                currentColor.a = 150;
                mCorpusMesh[file].setColor ( timepoint, currentColor );
            }
            //positions
            else
            {
                value = ofMap ( value, min, max, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax );
                glm::vec3 currentPoint = mCorpusMesh[file].getVertex ( timepoint );
                currentPoint[(int)axis] = value;
                mCorpusMesh[file].setVertex ( timepoint, currentPoint );
            }
        }
    }

    mAudioPlayback.SetCorpusMesh ( mCorpusMesh );

    mPointPicker->Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::ClearDimension ( Utilities::Axis axis )
{
    if ( axis == Utilities::Axis::X ) { xLabel = ""; }
    else if ( axis == Utilities::Axis::Y ) { yLabel = ""; }
    else if ( axis == Utilities::Axis::Z ) { zLabel = ""; }
    else if ( axis == Utilities::Axis::COLOR ) { colorDimension = -1; }

    for ( int file = 0; file < mCorpusMesh.size ( ); file++ )
    {
        for ( int timepoint = 0; timepoint < mCorpusMesh[file].getNumVertices ( ); timepoint++ )
        {
            //colors
            if ( axis == Utilities::Axis::COLOR )
            {
                ofColor currentColor = ofColor::fromHsb ( 35, 255, 255 );
                mCorpusMesh[file].setColor ( timepoint, currentColor );
            }
            //positions
            else
            {
                glm::vec3 currentPoint = mCorpusMesh[file].getVertex ( timepoint );
                currentPoint[(int)axis] = 0;
                mCorpusMesh[file].setVertex ( timepoint, currentPoint );
            }
        }
    }

    mAudioPlayback.SetCorpusMesh ( mCorpusMesh );

    mPointPicker->Train ( -1, axis, true );
}

void Explorer::LiveView::RefreshFileColors ( int fileIndex )
{
    ofLogVerbose ( "Explorer" ) << "Refreshing file colors for file: " << mRawView->GetDataset ( )->fileList[fileIndex];

    double min = mDimensionBounds.GetMinBound ( colorDimension );
    double max = mDimensionBounds.GetMaxBound ( colorDimension );
    double outputMin = bColorFullSpectrum ? SpaceDefs::mColorMin : SpaceDefs::mColorBlue;
    double outputMax = bColorFullSpectrum ? SpaceDefs::mColorMax : SpaceDefs::mColorRed;

    Utilities::TrailData* trails = mRawView->GetTrailData ( );

    for ( int timepoint = 0; timepoint < trails->raw[fileIndex].size ( ); timepoint++ )
    {
        ofColor color = ofColor::fromHsb ( ofMap ( trails->raw[fileIndex][timepoint][colorDimension], min, max, outputMin, outputMax ), 255, 255, 255 );
        if ( mPointPicker->GetNearestMousePointFile ( ) != fileIndex && mPointPicker->GetNearestMousePointFile ( ) != -1 ) { color.a = 125; }
        mCorpusMesh[fileIndex].setColor ( timepoint, color );
    }
}

// Camera Functions ----------------------------

void Explorer::LiveView::Init3DCam ( )
{ 
    double outsidePoint = SpaceDefs::mSpaceMax * 1.5;
    double midSpacePoint = (SpaceDefs::mSpaceMax + SpaceDefs::mSpaceMin ) / 2;
    mCamera->setPosition ( outsidePoint, midSpacePoint + 200, midSpacePoint );
    mCamPivot = ofPoint ( midSpacePoint, midSpacePoint, midSpacePoint );
    mCamera->lookAt ( mCamPivot ); 
    mCamera->setNearClip ( 0.01 ); 
    mCamera->setFarClip ( 99999 ); 
    mCamera->disableOrtho ( );
    mCamera->setScale ( 1 );
}

void Explorer::LiveView::Init2DCam ( Utilities::Axis disabledAxis )
{ 
    double midSpacePoint = (SpaceDefs::mSpaceMax + SpaceDefs::mSpaceMin ) / 2;
    if ( disabledAxis == Utilities::Axis::X ) { mCamera->setPosition ( -midSpacePoint, midSpacePoint, midSpacePoint ); }
    else if ( disabledAxis == Utilities::Axis::Y ) { mCamera->setPosition ( midSpacePoint, -midSpacePoint, midSpacePoint ); }
    else { mCamera->setPosition ( midSpacePoint, midSpacePoint, midSpacePoint ); }
    if ( disabledAxis == Utilities::Axis::X ) { mCamera->lookAt ( { 0, midSpacePoint, midSpacePoint } ); }
    else if ( disabledAxis == Utilities::Axis::Y ) { mCamera->lookAt ( { midSpacePoint, 0, midSpacePoint } ); }
    else { mCamera->lookAt ( { midSpacePoint, midSpacePoint, 0 } ); }
    mCamera->setNearClip ( 0.01 ); 
    mCamera->setFarClip ( 99999 );
    mCamera->enableOrtho ( );
    mCamera->setScale ( 1 );
    mCamMoveSpeedScaleAdjusted = SpaceDefs::mCamMoveSpeed * mCamera->getScale ( ).x;
}

void Explorer::LiveView::Zoom2DCam ( float y, bool mouse )
{
    mCamera->setScale ( mCamera->getScale ( ) + y * SpaceDefs::mCamZoomSpeed2D );
    float zoom = mCamera->getScale ( ).x + y * SpaceDefs::mCamZoomSpeed2D;
    if ( mCamera->getScale ( ).x > SpaceDefs::mZoomMin2D && y < 0.0f ) { mCamera->setScale ( zoom ); }
    else if ( mCamera->getScale ( ).x < SpaceDefs::mZoomMax2D && y > 0.0f ) { mCamera->setScale ( zoom ); }
    mCamMoveSpeedScaleAdjusted = SpaceDefs::mCamMoveSpeed * mCamera->getScale ( ).x;
    if ( mCamera->getScale ( ).x < SpaceDefs::mZoomMin2D ) { mCamera->setScale ( SpaceDefs::mZoomMin2D ); }
    mPointPicker->SetNearestCheckNeeded ( );
}

void Explorer::LiveView::Zoom3DCam ( float y, bool mouse )
{
    float scrollDist = y * SpaceDefs::mCamZoomSpeed3D;
    float camPivotDist = mCamPivot.distance ( mCamera->getPosition ( ) );

    if ( scrollDist < 0.0f )
    {
        scrollDist *= -1;
        if ( camPivotDist < (SpaceDefs::mZoomMin3D * 1.02) ) { return; }
        if ( scrollDist > (camPivotDist - SpaceDefs::mZoomMin3D) ) { scrollDist = camPivotDist - SpaceDefs::mZoomMin3D; }
        mCamera->dolly ( scrollDist * -1 );
    }
    else if ( scrollDist > 0.0f )
    {
        if ( camPivotDist > (SpaceDefs::mZoomMax3D * 0.98) ) { return; }
        if ( scrollDist > (SpaceDefs::mZoomMax3D - camPivotDist) ) { scrollDist = SpaceDefs::mZoomMax3D - camPivotDist; }
        mCamera->dolly ( scrollDist );
    }
}

void Explorer::LiveView::Rotate3DCam ( float x, float y, bool mouse )
{
    // get vectors
    glm::vec3 upNormalized = glm::normalize ( mCamera->getUpDir ( ) );
    glm::vec3 rightNormalized = glm::normalize ( mCamera->getSideDir ( ) );
    glm::vec3 focus = mCamera->getGlobalPosition ( ) - mCamPivot;
    glm::vec3 focusNormalized = glm::normalize ( focus );

    // calculate rotation angles
    if ( mouse ) { x -= mLastMouseX; y -= mLastMouseY; }

    float yawAngle = x * SpaceDefs::mCamRotateSpeed;
    float pitchAngle = y * SpaceDefs::mCamRotateSpeed;

    // calculate quaternions
    glm::quat yaw = glm::angleAxis ( yawAngle, upNormalized );
    glm::quat pitch = glm::angleAxis ( pitchAngle, rightNormalized );

    // check if we're not going over the top or under the bottom, if not, cross focus with pitch
    if ( focusNormalized.y < 0.90 && pitchAngle > 0 ) { focus = glm::cross ( focus, pitch ); }
    else if ( focusNormalized.y > -0.90 && pitchAngle < 0 ) { focus = glm::cross ( focus, pitch ); }

    // cross focus with yaw
    focus = glm::cross ( focus, yaw );

    // set new camera position and look at pivot point
    mCamera->setPosition ( mCamPivot + focus );
    mCamera->lookAt ( mCamPivot );
}

void Explorer::LiveView::Pan3DCam ( float x, float y, float z, bool mouse )
{
    glm::vec3 upNormalized = mCamera->getUpDir ( );
    glm::vec3 rightNormalized = mCamera->getSideDir ( );
    glm::vec3 focusNormalized = mCamera->getGlobalPosition ( ) - mCamPivot;

    upNormalized.x = 0;
    upNormalized.z = 0;
    rightNormalized.y = 0;
    focusNormalized.y = 0;

    upNormalized = glm::normalize ( upNormalized );
    rightNormalized = glm::normalize ( rightNormalized );
    focusNormalized = glm::normalize ( focusNormalized );

    if ( mouse )
    {
        x -= mLastMouseX;
        y -= mLastMouseY;
        x *= 2.0;
        y *= 2.0;
    }

    float moveX = x * mCamMoveSpeedScaleAdjusted * -1;
    float moveY = y * mCamMoveSpeedScaleAdjusted;
    float moveZ = z * mCamMoveSpeedScaleAdjusted;

    mCamera->move ( rightNormalized * moveX );
    mCamera->move ( upNormalized * moveY );
    mCamera->move ( focusNormalized * moveZ );
    mCamPivot += rightNormalized * moveX;
    mCamPivot += upNormalized * moveY;
    mCamPivot += focusNormalized * moveZ;
}

// Listener Functions --------------------------

void Explorer::LiveView::MouseEvent ( ofMouseEventArgs& args )
{
    //types: 0-pressed, 1-moved, 2-released, 3-dragged, 4-scrolled
    //buttons: 0-left, 1-middle, 2-right
    //modifiers: 0-none, 1-shift, 2-ctrl, 4-alt (and combinations of them are added together)
    //position: x, y
    //scroll direction: x, y

    if ( b3D )
    {
        if ( args.type == 4 ) // scroll - zoom
        {
            Zoom3DCam ( args.scrollY, true);
            mPointPicker->SetNearestCheckNeeded ( );
        }
        else if ( args.type == 3 && args.button == 0 ) // left click drag - rotate
        {
            Rotate3DCam ( args.x, args.y, true );
            mPointPicker->SetNearestCheckNeeded ( );
        }
        else if ( args.type == 3 && args.button == 1 ) // middle click drag - pan
        {
            Pan3DCam ( args.x, args.y, 0, true );
            mPointPicker->SetNearestCheckNeeded ( );
        }
    }
    else // 2D
    {
        if ( args.type == 4 ) // scroll - zoom
        {
            Zoom2DCam ( args.scrollY, true );
            mPointPicker->SetNearestCheckNeeded ( );
        }
        else if ( (args.type == 3 && args.button == 0) || (args.type == 3 && args.button == 1) ) // left/middle button drag - pan
        {
            mCamera->boom ( (args.y - mLastMouseY) * mCamMoveSpeedScaleAdjusted );
            mCamera->truck ( (args.x - mLastMouseX) * mCamMoveSpeedScaleAdjusted * -1 );
            mPointPicker->SetNearestCheckNeeded ( );
        }
    }

    mLastMouseX = args.x;
    mLastMouseY = args.y;
}

void Explorer::LiveView::KeyEvent ( ofKeyEventArgs& args )
{
    //type: 0-pressed, 1-released
    //key: no modifiers, just the raw key
    //scancode: includes all modifiers

    if ( args.type == ofKeyEventArgs::Type::Pressed )
    {
        if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_FORWARD || args.key == OF_KEY_UP ) { mKeyboardMoveState[0] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_LEFT || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_BACKWARD || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_RIGHT || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_UP ) { mKeyboardMoveState[4] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_DOWN ) { mKeyboardMoveState[5] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ROTATE_LEFT ) { mKeyboardMoveState[6] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ROTATE_RIGHT ) { mKeyboardMoveState[7] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ZOOM_IN ) { mKeyboardMoveState[8] = true; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ZOOM_OUT ) { mKeyboardMoveState[9] = true; }
    }
    else if ( args.type == ofKeyEventArgs::Type::Released )
    {
        if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_FORWARD || args.key == OF_KEY_UP ) { mKeyboardMoveState[0] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_LEFT || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_BACKWARD || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_RIGHT || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_UP ) { mKeyboardMoveState[4] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_MOVE_DOWN ) { mKeyboardMoveState[5] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ROTATE_LEFT ) { mKeyboardMoveState[6] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ROTATE_RIGHT ) { mKeyboardMoveState[7] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ZOOM_IN ) { mKeyboardMoveState[8] = false; }
        else if ( args.key == ACOREX_KEYBIND_CAMERA_ZOOM_OUT ) { mKeyboardMoveState[9] = false; }
        else if ( args.key == ACOREX_KEYBIND_CREATE_PLAYHEAD_ZERO_ZERO ) { mAudioPlayback.CreatePlayhead ( 0, 0 ); }
        else if ( args.key == ACOREX_KEYBIND_CREATE_PLAYHEAD_RANDOM_POINT ) { CreatePlayheadRandom ( ); }
        else if ( args.key == ACOREX_KEYBIND_CREATE_PLAYHEAD_PICKER_POINT ) { CreatePlayhead ( ); }
        else if ( args.key == ACOREX_KEYBIND_AUDIO_PAUSE ) { bUserPaused = !bUserPaused; mAudioPlayback.UserInvokedPause ( bUserPaused ); }
        else if ( args.key == ACOREX_KEYBIND_TOGGLE_DEBUG_VIEW ) { bDebug = !bDebug; }
        else if ( args.key == ACOREX_KEYBIND_TOGGLE_DRAWING_AXES ) { bDrawAxes = !bDrawAxes; }
        else if ( args.key == ACOREX_KEYBIND_TOGGLE_DRAWING_CLOUD )
        {
            if ( bDrawCloud && !bDrawCloudDark )
            {
                bDrawCloudDark = true;
            }
            else if ( bDrawCloud && bDrawCloudDark )
            {
                bDrawCloudDark = false;
                bDrawCloud = false;
            }
            else
            {
                bDrawCloud = true;
            }
        }
        //else if ( args.key == 'c' ) // TODO - might not need this key either just like the ENTER key below, remove also?
        //{ 
        //    if ( mPointPicker->GetNearestMousePointFile ( ) != -1 )
        //    {
        //        ofSetClipboardString ( mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )] );
        //    }
        //}
        /*else if ( args.key == OF_KEY_RETURN ) // TODO - decide whether change key or just remove entirely? enter triggers this even when
        *                                       // typing in file dialogs or settings boxes, so it's annoying
        *                                       // -- but might not even need this functionality at all anymore, i don't really use it
        {
            if ( mPointPicker->GetNearestMousePointFile ( ) != -1 )
            {
#ifdef _WIN32
                ofSystem ( "explorer /select," + mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )] );
#elif __APPLE__ && __MACH__
                ofSystem ( "open -R " + mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )] );
#elif __linux__
                ofSystem ( "xdg-open " + mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )] );
#endif
            }
        }*/
    }
}
