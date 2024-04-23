#include "./LiveView.h"
#include <ofLog.h>
#include "ofGraphics.h"
#include <of3dUtils.h>
#include <ofEvents.h>

using namespace Acorex;

void Explorer::LiveView::Initialise ( )
{
	mStatsCorpus.clear ( );
	mTimeCorpus.clear ( );

	b3D = true;
	Init3DCam ( );

	mDimensionBounds.CalculateBounds ( *mRawView->GetDataset ( ) );

	mPointPicker.Initialise ( *mRawView->GetDataset ( ), mDimensionBounds );

	ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
	ofAddListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
}

void Explorer::LiveView::Exit ( )
{
	RemoveListeners ( );
}

void Explorer::LiveView::RemoveListeners ( )
{
	ofRemoveListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
	ofRemoveListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
	ofRemoveListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
}

// Process Functions ---------------------------

void Explorer::LiveView::Update ( )
{
	deltaTime = ofGetElapsedTimef ( ) - lastUpdateTime;
	lastUpdateTime = ofGetElapsedTimef ( );
	if ( !bDraw ) { return; }

	float keyboardSpeedDelta = mKeyboardSpeedMulti * deltaTime;

	if ( b3D )
	{
		if ( mKeyboardMoveState[0] || mKeyboardMoveState[1] || mKeyboardMoveState[2] || mKeyboardMoveState[3] )
		{
			Pan3DCam (	( mKeyboardMoveState[1] - mKeyboardMoveState[3] ) * keyboardSpeedDelta,
						( mKeyboardMoveState[0] - mKeyboardMoveState[2] ) * keyboardSpeedDelta,
						false );
		}
	}
	else
	{
		if ( mKeyboardMoveState[0] || mKeyboardMoveState[1] || mKeyboardMoveState[2] || mKeyboardMoveState[3] )
		{
			float adjustedSpeed = mCamMoveSpeedScaleAdjusted * keyboardSpeedDelta;
			mCamera.boom ( (mKeyboardMoveState[0] - mKeyboardMoveState[2]) * adjustedSpeed );
			mCamera.truck ( (mKeyboardMoveState[3] - mKeyboardMoveState[1]) * adjustedSpeed );
		}
	}
}

void Explorer::LiveView::Draw ( )
{
	if ( !bDraw ) { return; }

	ofEnableDepthTest ( );
	mCamera.begin ( );

	// Draw Axis ------------------------------
	ofSetColor ( 255, 255, 255 );

	if ( mDisabledAxis != Utils::Axis::X ) { ofDrawLine ( { mSpaceMin, 0, 0 }, { mSpaceMax, 0, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Y ) { ofDrawLine ( { 0, mSpaceMin, 0 }, { 0, mSpaceMax, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Z ) { ofDrawLine ( { 0, 0, mSpaceMin }, { 0, 0, mSpaceMax } ); }

	if ( mDisabledAxis != Utils::Axis::X ) { ofDrawBitmapString ( xLabel , { mSpaceMax, 0, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Y ) { ofDrawBitmapString ( yLabel , { 0, mSpaceMax, 0 } ); }
	if ( mDisabledAxis != Utils::Axis::Z ) { ofDrawBitmapString ( zLabel , { 0, 0, mSpaceMax } ); }

	// Draw points ------------------------------
	if ( mRawView->IsTimeAnalysis ( ) ) // Time
	{
		for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
		{
			mTimeCorpus[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
			mTimeCorpus[file].draw ( );
			mTimeCorpus[file].setMode ( OF_PRIMITIVE_POINTS );
			mTimeCorpus[file].draw ( );
		}
	}
	else // Stats
	{
		mStatsCorpus.setMode ( OF_PRIMITIVE_POINTS );
		mStatsCorpus.draw ( );
	}

	mCamera.end ( );
	ofDisableDepthTest ( );
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
				value = ofMap ( value, min, max, mColorMin, mColorMax );
				ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
				currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
				mTimeCorpus[file].setColor ( timepoint, currentColor );
			}
			else
			{
				value = ofMap ( value, min, max, mSpaceMin, mSpaceMax );
				glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
				currentPoint[(int)axis] = value;
				mTimeCorpus[file].setVertex ( timepoint, currentPoint );
			}
		}
	}

	mPointPicker.Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionStats ( int dimensionIndex, Utils::Axis axis )
{
	std::string dimensionName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Utils::Axis::X ) { xLabel = dimensionName; }
	else if ( axis == Utils::Axis::Y ) { yLabel = dimensionName; }
	else if ( axis == Utils::Axis::Z ) { zLabel = dimensionName; }

	int statisticIndex = dimensionIndex % mRawView->GetStatistics ( ).size ( );
	int dividedDimensionIndex = dimensionIndex / mRawView->GetStatistics ( ).size ( );

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	for ( int file = 0; file < stats->raw.size ( ); file++ )
	{
		if ( axis == Utils::Axis::COLOR )
		{
			double value = ofMap (	stats->raw[file][dividedDimensionIndex][statisticIndex], 
									mDimensionBounds.GetMinBound ( dimensionIndex ), 
									mDimensionBounds.GetMaxBound ( dimensionIndex ), 
									mColorMin, mColorMax );
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			double value = ofMap (	stats->raw[file][dividedDimensionIndex][statisticIndex], 
									mDimensionBounds.GetMinBound ( dimensionIndex ), 
									mDimensionBounds.GetMaxBound ( dimensionIndex ), 
									mSpaceMin, mSpaceMax );
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[(int)axis] = value;
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}

	mPointPicker.Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionStatsReduced ( int dimensionIndex, Utils::Axis axis )
{
	std::string dimensioName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Utils::Axis::X ) { xLabel = dimensioName; }
	else if ( axis == Utils::Axis::Y ) { yLabel = dimensioName; }
	else if ( axis == Utils::Axis::Z ) { zLabel = dimensioName; }

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	for ( int file = 0; file < stats->reduced.size ( ); file++ )
	{
		if ( axis == Utils::Axis::COLOR )
		{
			double value = ofMap (	stats->reduced[file][dimensionIndex], 
									mDimensionBounds.GetMinBound ( dimensionIndex ), 
									mDimensionBounds.GetMaxBound ( dimensionIndex ), 
									mColorMin, mColorMax );
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			double value = ofMap (	stats->reduced[file][dimensionIndex], 
									mDimensionBounds.GetMinBound ( dimensionIndex ), 
									mDimensionBounds.GetMaxBound ( dimensionIndex ),
									mSpaceMin, mSpaceMax );
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[(int)axis] = value;
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}

	mPointPicker.Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionNone ( Utils::Axis axis )
{
	if ( axis == Utils::Axis::X ) { xLabel = ""; }
	else if ( axis == Utils::Axis::Y ) { yLabel = ""; }
	else if ( axis == Utils::Axis::Z ) { zLabel = ""; }

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

	mPointPicker.Train ( -1, axis, true );
}

// Camera Functions ----------------------------

void Explorer::LiveView::Init3DCam ( )
{ 
	double outsidePoint = mSpaceMax * 1.5;
	double midSpacePoint = ( mSpaceMax + mSpaceMin ) / 2;
	mCamera.setPosition ( outsidePoint, outsidePoint, outsidePoint );
	mCamPivot = ofPoint ( midSpacePoint, midSpacePoint, midSpacePoint );
	mCamera.lookAt ( mCamPivot ); 
	mCamera.setNearClip ( 0.01 ); 
	mCamera.setFarClip ( 99999 ); 
	mCamera.disableOrtho ( );
	mCamera.setScale ( 1 );
}

void Explorer::LiveView::Init2DCam ( Utils::Axis disabledAxis )
{ 
	double midSpacePoint = ( mSpaceMax + mSpaceMin ) / 2;
	mCamera.setPosition ( midSpacePoint, midSpacePoint, midSpacePoint );
	if ( disabledAxis == Utils::Axis::X ) { mCamera.lookAt ( { 0, midSpacePoint, midSpacePoint } ); }
	else if ( disabledAxis == Utils::Axis::Y ) { mCamera.lookAt ( { midSpacePoint, 0, midSpacePoint } ); }
	else { mCamera.lookAt ( { midSpacePoint, midSpacePoint, 0 } ); }
	mCamera.setNearClip ( 0.01 ); 
	mCamera.setFarClip ( 99999 );
	mCamera.enableOrtho ( );
	mCamera.setScale ( 1 );
	mCamMoveSpeedScaleAdjusted = mCamMoveSpeed * mCamera.getScale ( ).x;
}

void Explorer::LiveView::Zoom3DCam ( int y )
{
	float scrollDist = y * mCamZoomSpeed3D;
	if ( mCamPivot.distance ( mCamera.getPosition ( ) ) > mZoomMin3D && scrollDist < 0 ) { mCamera.dolly ( scrollDist ); }
	else if ( mCamPivot.distance ( mCamera.getPosition ( ) ) < mZoomMax3D && scrollDist > 0 ) { mCamera.dolly ( scrollDist ); }
}

void Explorer::LiveView::Rotate3DCam ( int x, int y )
{
	// get vectors
	glm::vec3 upNormalized = glm::normalize ( mCamera.getUpDir ( ) );
	glm::vec3 rightNormalized = glm::normalize ( mCamera.getSideDir ( ) );
	glm::vec3 focus = mCamera.getGlobalPosition ( ) - mCamPivot;
	glm::vec3 focusNormalized = glm::normalize ( focus );

	// calculate rotation angles
	float yawAngle = (x - mLastMouseX) * mCamRotateSpeed;
	float pitchAngle = (y - mLastMouseY) * mCamRotateSpeed;

	// calculate quaternions
	glm::quat yaw = glm::angleAxis ( yawAngle, upNormalized );
	glm::quat pitch = glm::angleAxis ( pitchAngle, rightNormalized );

	// check if we're not going over the top or under the bottom, if not, cross focus with pitch
	if ( focusNormalized.y < 0.90 && pitchAngle > 0 ) { focus = glm::cross ( focus, pitch ); }
	else if ( focusNormalized.y > -0.90 && pitchAngle < 0 ) { focus = glm::cross ( focus, pitch ); }

	// cross focus with yaw
	focus = glm::cross ( focus, yaw );

	// set new camera position and look at pivot point
	mCamera.setPosition ( mCamPivot + focus );
	mCamera.lookAt ( mCamPivot );
}

void Explorer::LiveView::Pan3DCam ( int x, int y, bool mouse )
{
	glm::vec3 upNormalized = glm::normalize ( mCamera.getUpDir ( ) );
	glm::vec3 rightNormalized = glm::normalize ( mCamera.getSideDir ( ) );

	if ( mouse ) { x -= mLastMouseX; y -= mLastMouseY; }

	float moveX = x * mCamMoveSpeedScaleAdjusted * -1;
	float moveY = y * mCamMoveSpeedScaleAdjusted;

	mCamera.move ( rightNormalized * moveX );
	mCamera.move ( upNormalized * moveY );
	mCamPivot += rightNormalized * moveX;
	mCamPivot += upNormalized * moveY;
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
			Zoom3DCam ( args.scrollY );
		}
		else if ( args.type == 3 && args.button == 0 ) // left click drag - rotate
		{
			Rotate3DCam ( args.x, args.y );
		}
		else if ( args.type == 3 && args.button == 1 ) // middle click drag - pan
		{
			Pan3DCam ( args.x, args.y, true );
		}
	}
	else // 2D
	{
		if ( args.type == 4 ) // scroll - zoom
		{
			mCamera.setScale ( mCamera.getScale ( ) + args.scrollY * mCamZoomSpeed2D );
			if ( mCamera.getScale ( ).x < mZoomMin2D ) { mCamera.setScale ( 0.1 ); }
			else if ( mCamera.getScale ( ).x > mZoomMax2D ) { mCamera.setScale ( 20.0 ); }
			mCamMoveSpeedScaleAdjusted = mCamMoveSpeed * mCamera.getScale ( ).x;
		}
		else if ( (args.type == 3 && args.button == 0) || (args.type == 3 && args.button == 1) ) // left/middle button drag - pan
		{
			mCamera.boom ( (args.y - mLastMouseY) * mCamMoveSpeedScaleAdjusted );
			mCamera.truck ( (args.x - mLastMouseX) * mCamMoveSpeedScaleAdjusted * -1 );
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

	if ( args.type == 0 )
	{
		if ( args.key == 'w' || args.key == OF_KEY_UP ) { mKeyboardMoveState[0] = true; }
		if ( args.key == 'a' || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = true; }
		if ( args.key == 's' || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = true; }
		if ( args.key == 'd' || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = true; }
	}
	else if ( args.type == 1 )
	{
		if ( args.key == 'w' || args.key == OF_KEY_UP ) { mKeyboardMoveState[0] = false; }
		if ( args.key == 'a' || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = false; }
		if ( args.key == 's' || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = false; }
		if ( args.key == 'd' || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = false; }
	}
}