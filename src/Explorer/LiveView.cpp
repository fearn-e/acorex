#include "./LiveView.h"
#include <ofLog.h>
#include "ofGraphics.h"
#include <of3dUtils.h>
#include <ofEvents.h>

using namespace Acorex;

void Explorer::LiveView::Initialise ( )
{
	glPointSize ( 3.0f );

	mStatsCorpus.clear ( );
	mTimeCorpus.clear ( );

	b3D = true;
	bColorFullSpectrum = false;

	for ( auto& each : mKeyboardMoveState ) { each = false; }

	mDisabledAxis = Utils::Axis::NONE;
	xLabel = "X"; yLabel = "Y"; zLabel = "Z";

	mCamera = std::make_shared<ofCamera> ( );
	Init3DCam ( );

	mDimensionBounds.CalculateBounds ( *mRawView->GetDataset ( ) );

	mPointPicker.SetCamera ( mCamera );
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
	mPointPicker.Exit ( );
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
			mPointPicker.SetNearestCheckNeeded ( );
		}
		else if ( mKeyboardMoveState[6] || mKeyboardMoveState[7] )
		{
			Rotate3DCam ( ( mKeyboardMoveState[6] - mKeyboardMoveState[7] ) * keyboardRotateDelta, 0, false );
			mPointPicker.SetNearestCheckNeeded ( );
		}
		else if ( mKeyboardMoveState[8] || mKeyboardMoveState[9] )
		{
			Zoom3DCam ( ( mKeyboardMoveState[8] - mKeyboardMoveState[9] ) * keyboardZoomDelta, false );
			mPointPicker.SetNearestCheckNeeded ( );
		}
	}
	else
	{
		if ( mKeyboardMoveState[0] || mKeyboardMoveState[1] || mKeyboardMoveState[2] || mKeyboardMoveState[3] )
		{
			float adjustedSpeed = mCamMoveSpeedScaleAdjusted * keyboardMoveDelta;
			mCamera->boom ( (mKeyboardMoveState[0] - mKeyboardMoveState[2]) * adjustedSpeed );
			mCamera->truck ( (mKeyboardMoveState[3] - mKeyboardMoveState[1]) * adjustedSpeed );
			mPointPicker.SetNearestCheckNeeded ( );
		}
		else if ( mKeyboardMoveState[8] || mKeyboardMoveState[9] )
		{
			Zoom2DCam ( ( mKeyboardMoveState[8] - mKeyboardMoveState[9] ) * keyboardZoomDelta, false );
			mPointPicker.SetNearestCheckNeeded ( );
	}
}
}

void Explorer::LiveView::SlowUpdate ( )
{
	mPointPicker.SlowUpdate ( );
}

void Explorer::LiveView::Draw ( )
{
	if ( !bDraw ) { return; }

	ofEnableDepthTest ( );
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
		if ( mPointPicker.GetNearestPointFile ( ) == -1 )
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
				if ( file != mPointPicker.GetNearestPointFile ( ) )
				{
					mTimeCorpus[file].disableColors ( );
					ofSetColor ( 255, 255, 255, 25 );
					mTimeCorpus[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
					mTimeCorpus[file].draw ( );
					mTimeCorpus[file].setMode ( OF_PRIMITIVE_POINTS );
					mTimeCorpus[file].draw ( );
				}
			}

			ofDisableDepthTest ( );
			mTimeCorpus[mPointPicker.GetNearestPointFile ( )].enableColors ( );
			mTimeCorpus[mPointPicker.GetNearestPointFile ( )].setMode ( OF_PRIMITIVE_LINE_STRIP );
			mTimeCorpus[mPointPicker.GetNearestPointFile ( )].draw ( );
			mTimeCorpus[mPointPicker.GetNearestPointFile ( )].setMode ( OF_PRIMITIVE_POINTS );
			mTimeCorpus[mPointPicker.GetNearestPointFile ( )].draw ( );
			ofEnableDepthTest ( );
		}
	}
	else // Stats
	{
		mStatsCorpus.setMode ( OF_PRIMITIVE_POINTS );
		mStatsCorpus.draw ( );
	}

	mCamera->end ( );
	ofDisableDepthTest ( );

	// Draw Nearest Point -----------------------
	mPointPicker.Draw ( );
	if ( mPointPicker.GetNearestPointFile ( ) != -1 )
	{
		ofDrawBitmapStringHighlight ( "Nearest File: " + mRawView->GetDataset ( )->fileList[mPointPicker.GetNearestPointFile ( )], 20, ofGetHeight ( ) - 60 );
		if ( mRawView->IsTimeAnalysis ( ) )
		{
			std::string hopInfoSamps = std::to_string ( mPointPicker.GetNearestPointTime ( ) * mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction );
			std::string hopInfoSecs = std::to_string ( mRawView->GetTimeData ( )->raw[mPointPicker.GetNearestPointFile ( )][mPointPicker.GetNearestPointTime ( )][0] );
			ofDrawBitmapStringHighlight ( "Nearest Timepoint: " + hopInfoSamps + " samples, " + hopInfoSecs + "s", 20, ofGetHeight ( ) - 40 );
		}
	}
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
				if ( bColorFullSpectrum ) { value = ofMap ( value, min, max, SpaceDefs::mColorMin, SpaceDefs::mColorMax ); }
				else { value = ofMap ( value, min, max, SpaceDefs::mColorBlue, SpaceDefs::mColorRed ); }
				ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
				currentColor.setHsb ( value, currentColor.getSaturation ( ), currentColor.getBrightness ( ) );
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

	mPointPicker.Train ( dimensionIndex, axis, false );
}

void Explorer::LiveView::FillDimensionStatsReduced ( int dimensionIndex, Utils::Axis axis )
{
	std::string dimensioName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Utils::Axis::X ) { xLabel = dimensioName; }
	else if ( axis == Utils::Axis::Y ) { yLabel = dimensioName; }
	else if ( axis == Utils::Axis::Z ) { zLabel = dimensioName; }

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
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			double value = ofMap ( value, min, max, SpaceDefs::mSpaceMin, SpaceDefs::mSpaceMax );
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
	mPointPicker.SetNearestCheckNeeded ( );
}

void Explorer::LiveView::Zoom3DCam ( float y, bool mouse )
{
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
			mPointPicker.SetNearestCheckNeeded ( );
		}
		else if ( args.type == 3 && args.button == 0 ) // left click drag - rotate
		{
			Rotate3DCam ( args.x, args.y, true );
			mPointPicker.SetNearestCheckNeeded ( );
		}
		else if ( args.type == 3 && args.button == 1 ) // middle click drag - pan
		{
			Pan3DCam ( args.x, args.y, 0, true );
			mPointPicker.SetNearestCheckNeeded ( );
		}
	}
	else // 2D
	{
		if ( args.type == 4 ) // scroll - zoom
		{
			Zoom2DCam ( args.scrollY, true );
			mPointPicker.SetNearestCheckNeeded ( );
		}
		else if ( (args.type == 3 && args.button == 0) || (args.type == 3 && args.button == 1) ) // left/middle button drag - pan
		{
			mCamera->boom ( (args.y - mLastMouseY) * mCamMoveSpeedScaleAdjusted );
			mCamera->truck ( (args.x - mLastMouseX) * mCamMoveSpeedScaleAdjusted * -1 );
			mPointPicker.SetNearestCheckNeeded ( );
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
		else if ( args.key == 'a' || args.key == OF_KEY_LEFT ) { mKeyboardMoveState[1] = true; }
		else if ( args.key == 's' || args.key == OF_KEY_DOWN ) { mKeyboardMoveState[2] = true; }
		else if ( args.key == 'd' || args.key == OF_KEY_RIGHT ) { mKeyboardMoveState[3] = true; }
		else if ( args.key == 'r' ) { mKeyboardMoveState[4] = true; }
		else if ( args.key == 'f' ) { mKeyboardMoveState[5] = true; }
		else if ( args.key == 'q' ) { mKeyboardMoveState[6] = true; }
		else if ( args.key == 'e' ) { mKeyboardMoveState[7] = true; }
		else if ( args.key == 'z' ) { mKeyboardMoveState[8] = true; }
		else if ( args.key == 'x' ) { mKeyboardMoveState[9] = true; }
	}
	else if ( args.type == 1 )
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
	}
}