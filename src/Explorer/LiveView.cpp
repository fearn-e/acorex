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

#include "./LiveView.h"
#include <ofLog.h>
#include <of3dGraphics.h>
#include <ofGraphics.h>
#include <of3dUtils.h>
#include <ofEvents.h>
#include <random>

using namespace Acorex;

void Explorer::LiveView::Initialise ( )
{
	mAudioPlayback.SetFlagReset ( );

	if ( !bPointersShared )
	{
		mPointPicker = std::make_shared<Explorer::PointPicker> ( );
		mAudioPlayback.SetPointPicker ( mPointPicker );
		bPointersShared = true;
	}

	glPointSize ( 3.0f );

	mStatsCorpus.clear ( );
	mTimeCorpus.clear ( );

	b3D = true;
	bColorFullSpectrum = false;
	bLoopAudio = false;

	for ( auto& each : mKeyboardMoveState ) { each = false; }

	mDisabledAxis = Utils::Axis::NONE;
	xLabel = "X"; yLabel = "Y"; zLabel = "Z";
	colorDimension = -1;

	if ( mPlayingFiles.size ( ) > 0 ) { mPlayingFiles.clear ( ); }
	if ( mPlayingTimeHeads.size ( ) > 0 ) { mPlayingTimeHeads.clear ( ); }

	mCamera = std::make_shared<ofCamera> ( );
	Init3DCam ( );

	mDimensionBounds.CalculateBounds ( *mRawView->GetDataset ( ) );

	mPointPicker->SetCamera ( mCamera );
	mPointPicker->Initialise ( *mRawView->GetDataset ( ), mDimensionBounds );

	if ( mSoundPlayers.size ( ) > 0 ) { mSoundPlayers.clear ( ); }
	if ( mPlayingFiles.size ( ) > 0 ) { mPlayingFiles.clear ( ); }
	if ( mPlayingTimeHeads.size ( ) > 0 ) { mPlayingTimeHeads.clear ( ); }

	mAudioPlayback.Initialise ( );
	mAudioPlayback.SetRawView ( mRawView );

	if ( !listenersAdded )
	{
		ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
		ofAddListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
		ofAddListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
		ofAddListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
		ofAddListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
		ofAddListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
		ofAddListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
		listenersAdded = true;
	}
}

void Explorer::LiveView::ChangeAudioSettings ( size_t bufferSize, ofSoundDevice outDevice )
{
	mAudioPlayback.RestartAudio ( mRawView->GetDataset ( )->analysisSettings.sampleRate, bufferSize, outDevice );
}

void Explorer::LiveView::KillAudio ( )
{
	mAudioPlayback.SetFlagReset ( );
	mAudioPlayback.WaitForResetConfirm ( );
}

void Explorer::LiveView::Exit ( )
{
	RemoveListeners ( );
	mPointPicker->Exit ( );
}

void Explorer::LiveView::RemoveListeners ( )
{
	if ( !listenersAdded ) { return; }
	ofRemoveListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
	ofRemoveListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
	listenersAdded = false;
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
	std::vector<Utils::VisualPlayhead> playheadUpdates = mAudioPlayback.GetPlayheadInfo ( );

	// remove playheads that aren't in the update list
	for ( int i = 0; i < mPlayheads.size ( ); i++ )
	{
		if ( std::find_if ( playheadUpdates.begin ( ), playheadUpdates.end ( ), [this, i]( Utils::VisualPlayhead& playhead ) { return playhead.playheadID == mPlayheads[i].playheadID; } ) == playheadUpdates.end ( ) )
		{
			ofLogNotice ( "LiveView" ) << "Playhead " << mPlayheads[i].playheadID << " deleted";

			// if not the final one, move all after it back to the left
			int j = mPlayheads.size ( ) - 1; int end = i;
			while ( j > end )
			{
				mPlayheads[j].panelRect = mPlayheads[j - 1].panelRect;
				j--;
			}

			mPlayheads.erase ( mPlayheads.begin ( ) + i );
			i--;
		}
	}

	// go through playheadUpdates and update the respective playheads or add new ones if the ID isn't found
	for ( int i = 0; i < playheadUpdates.size ( ); i++ )
	{
		auto it = std::find_if ( mPlayheads.begin ( ), mPlayheads.end ( ), [this, i, &playheadUpdates] ( Utils::VisualPlayhead& playhead ) { return playhead.playheadID == playheadUpdates[i].playheadID; } );
		if ( it != mPlayheads.end ( ) )
		{
			ofColor color = it->color;
			ofRectangle rect = it->panelRect;
			bool highlight = it->highlight;
			*it = playheadUpdates[i];
			it->color = color;
			it->panelRect = rect;
			it->highlight = highlight;
		}
		else
		{
			ofLogNotice ( "LiveView" ) << "Playhead " << playheadUpdates[i].playheadID << " added";

			mPlayheads.push_back ( playheadUpdates[i] );
			int rectWidth = ofGetWidth ( ) / 10; int rectSpacing = ofGetWidth ( ) / 100; int rectHeight = ofGetHeight ( ) / 10;
			std::random_device rd;
			std::mt19937 gen ( rd ( ) );
			std::uniform_int_distribution<> dis ( 0, 255 );
			mPlayheads.back ( ).color = ofColor::fromHsb ( dis ( gen ), 255, 255 );
			mPlayheads.back ( ).panelRect = ofRectangle (	rectSpacing * mPlayheads.size ( ) + rectWidth * ( mPlayheads.size ( ) - 1 ),
															ofGetHeight ( ) - rectHeight - 5,
															rectWidth,
															rectHeight );
		}
	}

	// calculate the 3D positions of the playheads
	for ( auto& playhead : mPlayheads )
	{
		size_t timeIndex = playhead.sampleIndex / ( mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction );
		playhead.position[0] = mTimeCorpus[playhead.fileIndex].getVertex ( timeIndex ).x;
		playhead.position[1] = mTimeCorpus[playhead.fileIndex].getVertex ( timeIndex ).y;
		playhead.position[2] = mTimeCorpus[playhead.fileIndex].getVertex ( timeIndex ).z;
	}
}

void Explorer::LiveView::OLD_UpdateAudioPlayers ( )
{
	if ( mPlayingFiles.size ( ) == 0 ) { return; }

	if ( mRawView->IsTimeAnalysis ( )  )
	{
		Utils::TimeData* time = mRawView->GetTimeData ( );

		for ( int i = 0; i < mPlayingFiles.size ( ); i++ )
		{
			float timePlayed = mSoundPlayers[i].getPositionMS ( ) / 1000.0;
			if ( !mSoundPlayers[i].getIsPlaying ( ) || timePlayed < mPlayingLastPositionMS[i] || ( mPlayingTimeHeads[i] + 1 ) >= time->raw[mPlayingFiles[i]].size ( ) )
			{
				if ( !bLoopAudio )
				{
					RefreshFileColors ( mPlayingFiles[i] );
					mPlayingFiles.erase ( mPlayingFiles.begin ( ) + i );
					mPlayingTimeHeads.erase ( mPlayingTimeHeads.begin ( ) + i );
					mPlayingLastPositionMS.erase ( mPlayingLastPositionMS.begin ( ) + i );
					mPlayingLastColor.erase ( mPlayingLastColor.begin ( ) + i );
					mSoundPlayers[i].stop ( );
					mSoundPlayers.erase ( mSoundPlayers.begin ( ) + i );
					i--;
					continue;
				}
				else
				{
					mTimeCorpus[mPlayingFiles[i]].setColor ( mPlayingTimeHeads[i], mPlayingLastColor[i] );

					mPlayingTimeHeads[i] = 0;

					mPlayingLastColor[i] = mTimeCorpus[mPlayingFiles[i]].getColor ( mPlayingTimeHeads[i] );
					mTimeCorpus[mPlayingFiles[i]].setColor ( mPlayingTimeHeads[i], ofColor { 255, 255, 255, 255 } );
				}
			}
			else
			{
				float nextTimeStep = time->raw[mPlayingFiles[i]][mPlayingTimeHeads[i]][0];

				if ( timePlayed >= nextTimeStep )
				{
					mTimeCorpus[mPlayingFiles[i]].setColor ( mPlayingTimeHeads[i], mPlayingLastColor[i] );

					mPlayingTimeHeads[i]++;

					mPlayingLastColor[i] = mTimeCorpus[mPlayingFiles[i]].getColor ( mPlayingTimeHeads[i] );
					mTimeCorpus[mPlayingFiles[i]].setColor ( mPlayingTimeHeads[i], ofColor { 255, 255, 255, 255 } );
				}
			}

			mPlayingLastPositionMS[i] = timePlayed;
		}
	}
	else // stats
	{
		Utils::StatsData* stats = mRawView->GetStatsData ( );

		for ( int i = 0; i < mPlayingFiles.size ( ); i++ )
		{
			if ( mSoundPlayers[i].getIsPlaying ( ) ) { continue; }

			RefreshFileColors ( mPlayingFiles[i] );
			mPlayingFiles.erase ( mPlayingFiles.begin ( ) + i );
			mSoundPlayers[i].stop ( );
			mSoundPlayers.erase ( mSoundPlayers.begin ( ) + i );
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
	ofSetColor ( 255, 255, 255 );

	if ( mDisabledAxis != Utils::Axis::X ) { ofDrawLine ( { SpaceDefs::mSpaceMin, 0, 0 }, { SpaceDefs::mSpaceMax, 0, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Y ) { ofDrawLine ( { 0, SpaceDefs::mSpaceMin, 0 }, { 0, SpaceDefs::mSpaceMax, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Z ) { ofDrawLine ( { 0, 0, SpaceDefs::mSpaceMin }, { 0, 0, SpaceDefs::mSpaceMax } ); }

	if ( mDisabledAxis != Utils::Axis::X ) { ofDrawBitmapString ( xLabel , { SpaceDefs::mSpaceMax, 0, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Y ) { ofDrawBitmapString ( yLabel , { 0, SpaceDefs::mSpaceMax, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Z ) { ofDrawBitmapString ( zLabel , { 0, 0, SpaceDefs::mSpaceMax } ); }

	// Draw points ------------------------------
	if ( mRawView->IsTimeAnalysis ( ) ) // Time
	{
		if ( mPointPicker->GetNearestMousePointFile ( ) == -1 )
		{
			for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
			{
				mTimeCorpus[file].enableColors ( );
				mTimeCorpus[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
				mTimeCorpus[file].draw ( );
				mTimeCorpus[file].setMode ( OF_PRIMITIVE_POINTS );
				mTimeCorpus[file].draw ( );
			}
		}
		else
		{
			for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
			{
				if ( file == mPointPicker->GetNearestMousePointFile ( ) ) { continue; }
				mTimeCorpus[file].disableColors ( );
				ofSetColor ( 255, 255, 255, 25 );
				mTimeCorpus[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
				mTimeCorpus[file].draw ( );
				mTimeCorpus[file].setMode ( OF_PRIMITIVE_POINTS );
				mTimeCorpus[file].draw ( );
			}

			mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].enableColors ( );
			mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].setMode ( OF_PRIMITIVE_LINE_STRIP );
			mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].draw ( );
			mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].setMode ( OF_PRIMITIVE_POINTS );
			mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].draw ( );
		}

		for ( int i = 0; i < mPlayheads.size ( ); i++ )
		{
			ofDisableDepthTest ( );
			ofDisableAlphaBlending ( );

			ofColor color = mPlayheads[i].color; float size = 50;
			if ( mPlayheads[i].highlight ) { color = { 255, 255, 255, 255 }; size = 100; }

			ofSetColor ( color );
			glm::vec3 position = { mPlayheads[i].position[0], mPlayheads[i].position[1], mPlayheads[i].position[2] };
			ofDrawSphere ( position, size );

			ofEnableAlphaBlending ( );
			ofEnableDepthTest ( );
		}
	}
	else // Stats
	{
		if ( mPointPicker->GetNearestMousePointFile ( ) == -1 && mPlayingFiles.size ( ) == 0 )
		{
			mStatsCorpus.enableColors ( );
			mStatsCorpus.setMode ( OF_PRIMITIVE_POINTS );
			mStatsCorpus.draw ( );
		}
		else
		{
			mStatsCorpus.disableColors ( );
			ofSetColor ( 255, 255, 255, 25 );
			mStatsCorpus.setMode ( OF_PRIMITIVE_POINTS );
			mStatsCorpus.draw ( );

			ofDisableDepthTest ( );

			for ( int i = 0; i < mPlayingFiles.size ( ); i++ )
			{
				ofSetColor ( 255, 255, 255, 150 );
				ofDrawSphere ( mStatsCorpus.getVertex ( mPlayingFiles[i] ), 25 );
			}

			if ( mPointPicker->GetNearestMousePointFile ( ) >= 0 )
			{
				ofSetColor ( mStatsCorpus.getColor ( mPointPicker->GetNearestMousePointFile ( ) ) );
				ofDrawSphere ( mStatsCorpus.getVertex ( mPointPicker->GetNearestMousePointFile ( ) ), 25 );
			}

			ofEnableDepthTest ( );
		}
	}

	mCamera->end ( );
	ofDisableAlphaBlending ( );
	ofDisableDepthTest ( );

	// Draw Nearest Point -----------------------
	mPointPicker->Draw ( );
	if ( mPointPicker->GetNearestMousePointFile ( ) != -1 )
	{
		ofDrawBitmapStringHighlight ( "Nearest File: " + mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )], 20, ofGetHeight ( ) - 60 );
		if ( mRawView->IsTimeAnalysis ( ) )
		{
			std::string hopInfoSamps = std::to_string ( mPointPicker->GetNearestMousePointTime ( ) * mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction );
			std::string hopInfoSecs = std::to_string ( mRawView->GetTimeData ( )->raw[mPointPicker->GetNearestMousePointFile ( )][mPointPicker->GetNearestMousePointTime ( )][0] );
			ofDrawBitmapStringHighlight ( "Nearest Timepoint: " + hopInfoSamps + " samples, " + hopInfoSecs + "s", 20, ofGetHeight ( ) - 40 );
		}
	}

	if ( bDebug )
	{
		ofDrawBitmapStringHighlight ( "UPS: " + std::to_string ( 1 / deltaTime ), 20, ofGetHeight ( ) - 220 );
	}
}

// Sound Functions ------------------------------

void Explorer::LiveView::CreatePlayhead ( )
{
	if ( mPointPicker->GetNearestMousePointFile ( ) == -1 ) { return; }

	mAudioPlayback.CreatePlayhead ( mPointPicker->GetNearestMousePointFile ( ), mPointPicker->GetNearestMousePointTime ( ) );

	return;
}

void Explorer::LiveView::KillPlayhead ( size_t playheadID )
{
	mAudioPlayback.KillPlayhead ( playheadID );
}

void Explorer::LiveView::OLD_PlaySound ( )
{
	if ( std::find ( mPlayingFiles.begin ( ), mPlayingFiles.end ( ), mPointPicker->GetNearestMousePointFile ( ) ) != mPlayingFiles.end ( ) )
	{
		std::vector<int>::iterator it = std::find ( mPlayingFiles.begin ( ), mPlayingFiles.end ( ), mPointPicker->GetNearestMousePointFile ( ) );
		int index = std::distance ( mPlayingFiles.begin ( ), it );
		mSoundPlayers[index].setPositionMS ( 0 );
		mSoundPlayers[index].play ( );

		if ( mRawView->IsTimeAnalysis ( ) )
		{
			ofColor color = mTimeCorpus[mPlayingFiles[index]].getColor ( mPlayingTimeHeads[index] );
			color.setBrightness ( 100 );  color.setSaturation ( 100 );
			mTimeCorpus[mPlayingFiles[index]].setColor ( mPlayingTimeHeads[index], color );

			mPlayingTimeHeads[index] = 0;

			color = mTimeCorpus[mPlayingFiles[index]].getColor ( mPlayingTimeHeads[index] );
			color.setBrightness ( 255 );  color.setSaturation ( 0 );
			mTimeCorpus[mPlayingFiles[index]].setColor ( mPlayingTimeHeads[index], color );

			mPlayingLastPositionMS[index] = 0.0f;
		}
		return;
	}

	std::string filePath = mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )];

	ofSoundPlayer soundPlayer;
	bool success = soundPlayer.load ( filePath );
	if ( !success ) { ofLogError ( "Explorer" ) << "Failed to load sound file: " << filePath; return; }

	mPlayingFiles.push_back ( mPointPicker->GetNearestMousePointFile ( ) );
	if ( mRawView->IsTimeAnalysis ( ) )
	{
		mPlayingTimeHeads.push_back ( 0 );
		mPlayingLastPositionMS.push_back ( 0.0f );
		mPlayingLastColor.push_back ( mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].getColor ( 0 ) );
		mTimeCorpus[mPointPicker->GetNearestMousePointFile ( )].setColor ( 0, ofColor { 255, 255, 255, 255 } );
	}
	else
	{ }

	mSoundPlayers.push_back ( soundPlayer );
	mSoundPlayers.back ( ).setLoop ( bLoopAudio );
	mSoundPlayers.back ( ).setVolume ( 0.6 );
	mSoundPlayers.back ( ).play ( );
}

// Filler Functions ----------------------------

void Explorer::LiveView::CreatePoints ( )
{
	if ( mRawView->IsTimeAnalysis ( ) )
	{
		Utils::TimeData* time = mRawView->GetTimeData ( );

		for ( int file = 0; file < time->raw.size ( ); file++ )
		{
			ofMesh mesh;
			for ( int timepoint = 0; timepoint < time->raw[file].size ( ); timepoint++ )
			{
				mesh.addVertex ( { 0, 0, 0 } );
				ofColor color = ofColor::fromHsb ( 35, 255, 255 );
				mesh.addColor ( color );
			}
			mTimeCorpus.push_back ( mesh );
		}

		mAudioPlayback.SetTimeCorpus ( mTimeCorpus );

		bDraw = true;
		return;
	}
	// ------------------------------

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	if ( !mRawView->IsReduction ( ) )
	{
		for ( int file = 0; file < stats->raw.size ( ); file++ )
		{
			for ( int point = 0; point < stats->raw[file].size ( ); point++ )
			{
				mStatsCorpus.addVertex ( { 0, 0, 0 } );
				ofColor color = ofColor::fromHsb ( 35, 255, 255 );
				mStatsCorpus.addColor ( color );
			}
		}

		bDraw = true;
		return;
	}
	// ------------------------------

	{
		for ( int file = 0; file < stats->reduced.size ( ); file++ )
		{
			for ( int point = 0; point < stats->reduced[file].size ( ); point++ )
			{
				mStatsCorpus.addVertex ( { 0, 0, 0 } );
				ofColor color = ofColor::fromHsb ( 35, 255, 255 );
				mStatsCorpus.addColor ( color );
			}
		}

		bDraw = true;
		return;
	}
}

void Explorer::LiveView::FillDimensionTime ( int dimensionIndex, Utils::Axis axis )
{
	std::string dimensionName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Utils::Axis::X ) { xLabel = dimensionName; }
	else if ( axis == Utils::Axis::Y ) { yLabel = dimensionName; }
	else if ( axis == Utils::Axis::Z ) { zLabel = dimensionName; }
	else if ( axis == Utils::Axis::COLOR ) { colorDimension = dimensionIndex; }

	Utils::TimeData* time = mRawView->GetTimeData ( );

	double min = mDimensionBounds.GetMinBound ( dimensionIndex );
	double max = mDimensionBounds.GetMaxBound ( dimensionIndex );

	for ( int file = 0; file < time->raw.size ( ); file++ )
	{
		for ( int timepoint = 0; timepoint < time->raw[file].size ( ); timepoint++ )
		{
			double value = time->raw[file][timepoint][dimensionIndex];

			if ( axis == Utils::Axis::COLOR )
			{
				if ( bColorFullSpectrum ) { value = ofMap ( value, min, max, SpaceDefs::mColorMin, SpaceDefs::mColorMax ); }
				else { value = ofMap ( value, min, max, SpaceDefs::mColorBlue, SpaceDefs::mColorRed ); }
				ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
				currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
				currentColor.a = 150;
				mTimeCorpus[file].setColor ( timepoint, currentColor );
			}
			else
			{
				value = ofMap ( value, min, max, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax );
				glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
				currentPoint[(int)axis] = value;
				mTimeCorpus[file].setVertex ( timepoint, currentPoint );
			}
		}
	}

	mAudioPlayback.SetTimeCorpus ( mTimeCorpus );

	mPointPicker->Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionStats ( int dimensionIndex, Utils::Axis axis )
{
	std::string dimensionName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Utils::Axis::X ) { xLabel = dimensionName; }
	else if ( axis == Utils::Axis::Y ) { yLabel = dimensionName; }
	else if ( axis == Utils::Axis::Z ) { zLabel = dimensionName; }
	else if ( axis == Utils::Axis::COLOR ) { colorDimension = dimensionIndex; }

	int statisticIndex = dimensionIndex % mRawView->GetStatistics ( ).size ( );
	int dividedDimensionIndex = dimensionIndex / mRawView->GetStatistics ( ).size ( );

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	double min = mDimensionBounds.GetMinBound ( dimensionIndex );
	double max = mDimensionBounds.GetMaxBound ( dimensionIndex );

	for ( int file = 0; file < stats->raw.size ( ); file++ )
	{
		double value = stats->raw[file][dividedDimensionIndex][statisticIndex];
		
		if ( axis == Utils::Axis::COLOR )
		{
			if ( bColorFullSpectrum ) { value = ofMap ( value, min, max, SpaceDefs::mColorMin, SpaceDefs::mColorMax ); }
			else { value = ofMap ( value, min, max, SpaceDefs::mColorBlue, SpaceDefs::mColorRed ); }
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
			currentColor.a = 150;
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			value = ofMap (  value, min, max, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax );
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[(int)axis] = value;
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}

	mPointPicker->Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionStatsReduced ( int dimensionIndex, Utils::Axis axis )
{
	std::string dimensioName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Utils::Axis::X ) { xLabel = dimensioName; }
	else if ( axis == Utils::Axis::Y ) { yLabel = dimensioName; }
	else if ( axis == Utils::Axis::Z ) { zLabel = dimensioName; }
	else if ( axis == Utils::Axis::COLOR ) { colorDimension = dimensionIndex; }

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	double min = mDimensionBounds.GetMinBound ( dimensionIndex );
	double max = mDimensionBounds.GetMaxBound ( dimensionIndex );

	for ( int file = 0; file < stats->reduced.size ( ); file++ )
	{
		double value = stats->reduced[file][dimensionIndex];

		if ( axis == Utils::Axis::COLOR )
		{
			if ( bColorFullSpectrum ) { value = ofMap ( value, min, max, SpaceDefs::mColorMin, SpaceDefs::mColorMax ); }
			else { value = ofMap ( value, min, max, SpaceDefs::mColorBlue, SpaceDefs::mColorRed ); }
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
			currentColor.a = 150;
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			value = ofMap ( value, min, max, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax );
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[(int)axis] = value;
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}

	mPointPicker->Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionNone ( Utils::Axis axis )
{
	if ( axis == Utils::Axis::X ) { xLabel = ""; }
	else if ( axis == Utils::Axis::Y ) { yLabel = ""; }
	else if ( axis == Utils::Axis::Z ) { zLabel = ""; }
	else if ( axis == Utils::Axis::COLOR ) { colorDimension = -1; }

	if ( mRawView->IsTimeAnalysis ( ) )
	{
		for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < mTimeCorpus[file].getNumVertices ( ); timepoint++ )
			{
				if ( axis == Utils::Axis::COLOR )
				{
					ofColor currentColor = ofColor::fromHsb ( 35, 255, 255 );
					mTimeCorpus[file].setColor ( timepoint, currentColor );
				}
				else
				{
					glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
					currentPoint[(int)axis] = 0;
					mTimeCorpus[file].setVertex ( timepoint, currentPoint );
				}
			}
		}

		mAudioPlayback.SetTimeCorpus ( mTimeCorpus );
	}
	else
	{
		for ( int file = 0; file < mStatsCorpus.getNumVertices ( ); file++ )
		{
			if ( axis == Utils::Axis::COLOR )
			{
				ofColor currentColor = ofColor::fromHsb ( 35, 255, 255 );
				mStatsCorpus.setColor ( file, currentColor );
			}
			else
			{
				glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
				currentPoint[(int)axis] = 0;
				mStatsCorpus.setVertex ( file, currentPoint );
			}
		}
	}

	mPointPicker->Train ( -1, axis, true );
}

void Explorer::LiveView::RefreshFileColors ( int fileIndex )
{
	if ( bDebug ) { ofLogNotice ( "Explorer" ) << "Refreshing file colors for file: " << mRawView->GetDataset ( )->fileList[fileIndex]; }

	double min = mDimensionBounds.GetMinBound ( colorDimension );
	double max = mDimensionBounds.GetMaxBound ( colorDimension );
	double outputMin = bColorFullSpectrum ? SpaceDefs::mColorMin : SpaceDefs::mColorBlue;
	double outputMax = bColorFullSpectrum ? SpaceDefs::mColorMax : SpaceDefs::mColorRed;

	if ( mRawView->IsTimeAnalysis ( ) )
	{
		Utils::TimeData* time = mRawView->GetTimeData ( );

		for ( int timepoint = 0; timepoint < time->raw[fileIndex].size ( ); timepoint++ )
		{
			ofColor color = ofColor::fromHsb ( ofMap ( time->raw[fileIndex][timepoint][colorDimension], min, max, outputMin, outputMax ), 255, 255, 255 );
			if ( mPointPicker->GetNearestMousePointFile ( ) != fileIndex && mPointPicker->GetNearestMousePointFile ( ) != -1 ) { color.a = 125; }
			mTimeCorpus[fileIndex].setColor ( timepoint, color );
		}
	}
	else
	{
		Utils::StatsData* stats = mRawView->GetStatsData ( );

		if ( !mRawView->IsReduction ( ) )
		{
			int statisticIndex = colorDimension % DATA_NUM_STATS;
			int dimensionIndex = colorDimension / DATA_NUM_STATS;

			ofColor color = ofColor::fromHsb ( ofMap ( stats->raw[fileIndex][dimensionIndex][statisticIndex], min, max, outputMin, outputMax ), 255, 255, 255 );
			if ( mPointPicker->GetNearestMousePointFile ( ) != fileIndex && mPointPicker->GetNearestMousePointFile ( ) != -1 ) { color.a = 125; }
			mStatsCorpus.setColor ( fileIndex, color );
		}
		else
		{
			ofColor color = ofColor::fromHsb ( ofMap ( stats->reduced[fileIndex][colorDimension], min, max, outputMin, outputMax ), 255, 255, 255 );
			if ( mPointPicker->GetNearestMousePointFile ( ) != fileIndex && mPointPicker->GetNearestMousePointFile ( ) != -1 ) { color.a = 125; }
			mStatsCorpus.setColor ( fileIndex, color );
		}
	}
}

// Camera Functions ----------------------------

void Explorer::LiveView::Init3DCam ( )
{ 
	double outsidePoint = SpaceDefs::mSpaceMax * 1.5;
	double midSpacePoint = (SpaceDefs::mSpaceMax + SpaceDefs::mSpaceMin ) / 2;
	mCamera->setPosition ( outsidePoint, outsidePoint, outsidePoint );
	mCamPivot = ofPoint ( midSpacePoint, midSpacePoint, midSpacePoint );
	mCamera->lookAt ( mCamPivot ); 
	mCamera->setNearClip ( 0.01 ); 
	mCamera->setFarClip ( 99999 ); 
	mCamera->disableOrtho ( );
	mCamera->setScale ( 1 );
}

void Explorer::LiveView::Init2DCam ( Utils::Axis disabledAxis )
{ 
	double midSpacePoint = (SpaceDefs::mSpaceMax + SpaceDefs::mSpaceMin ) / 2;
	if ( disabledAxis == Utils::Axis::X ) { mCamera->setPosition ( -midSpacePoint, midSpacePoint, midSpacePoint ); }
	else if ( disabledAxis == Utils::Axis::Y ) { mCamera->setPosition ( midSpacePoint, -midSpacePoint, midSpacePoint ); }
	else { mCamera->setPosition ( midSpacePoint, midSpacePoint, midSpacePoint ); }
	if ( disabledAxis == Utils::Axis::X ) { mCamera->lookAt ( { 0, midSpacePoint, midSpacePoint } ); }
	else if ( disabledAxis == Utils::Axis::Y ) { mCamera->lookAt ( { midSpacePoint, 0, midSpacePoint } ); }
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
	// TODO - get cam direction to pivot point, check after zoom that direction is still the same, if negative, revert zoom
	float scrollDist = y * SpaceDefs::mCamZoomSpeed3D;
	if ( mCamPivot.distance ( mCamera->getPosition ( ) ) > SpaceDefs::mZoomMin3D && scrollDist < 0.0f && scrollDist < mCamPivot.distance ( mCamera->getPosition ( ) ) ) { mCamera->dolly ( scrollDist ); }
	else if ( mCamPivot.distance ( mCamera->getPosition ( ) ) < SpaceDefs::mZoomMax3D && scrollDist > 0.0f ) { mCamera->dolly ( scrollDist ); }
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

	if ( mouse ) { x -= mLastMouseX; y -= mLastMouseY; }

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
		if ( args.key == 'w' || args.key == OF_KEY_UP ) { mKeyboardMoveState[0] = true; }
		else if ( args.key == 'a' || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = true; }
		else if ( args.key == 's' || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = true; }
		else if ( args.key == 'd' || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = true; }
		else if ( args.key == 'r' ) { mKeyboardMoveState[4] = true; }
		else if ( args.key == 'f' ) { mKeyboardMoveState[5] = true; }
		else if ( args.key == 'q' ) { mKeyboardMoveState[6] = true; }
		else if ( args.key == 'e' ) { mKeyboardMoveState[7] = true; }
		else if ( args.key == 'z' ) { mKeyboardMoveState[8] = true; }
		else if ( args.key == 'x' ) { mKeyboardMoveState[9] = true; }
		else if ( args.key == ' ' ) { CreatePlayhead ( ); }
	}
	else if ( args.type == ofKeyEventArgs::Type::Released )
	{
		if ( args.key == 'w' || args.key == OF_KEY_UP ) { mKeyboardMoveState[0] = false; }
		else if ( args.key == 'a' || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = false; }
		else if ( args.key == 's' || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = false; }
		else if ( args.key == 'd' || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = false; }
		else if ( args.key == 'r' ) { mKeyboardMoveState[4] = false; }
		else if ( args.key == 'f' ) { mKeyboardMoveState[5] = false; }
		else if ( args.key == 'q' ) { mKeyboardMoveState[6] = false; }
		else if ( args.key == 'e' ) { mKeyboardMoveState[7] = false; }
		else if ( args.key == 'z' ) { mKeyboardMoveState[8] = false; }
		else if ( args.key == 'x' ) { mKeyboardMoveState[9] = false; }
		else if ( args.key == OF_KEY_F3 ) { bDebug = !bDebug; }
		else if ( args.key == 'l' )
		{
			bLoopAudio = !bLoopAudio;
			for ( auto& each : mSoundPlayers ) { each.setLoop ( bLoopAudio ); }
		}
		else if ( args.key == 'c' )
		{ 
			if ( mPointPicker->GetNearestMousePointFile ( ) != -1 )
			{
				ofSetClipboardString ( mRawView->GetDataset ( )->fileList[mPointPicker->GetNearestMousePointFile ( )] );
			}
		}
		else if ( args.key == OF_KEY_RETURN )
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
		}
	}
}
