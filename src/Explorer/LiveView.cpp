#include "./LiveView.h"
#include <ofLog.h>
#include "ofGraphics.h"
#include <of3dUtils.h>
#include <ofEvents.h>

using namespace Acorex;

void Explorer::LiveView::SetRawView ( std::shared_ptr<RawView>& rawPointer )
{
	mRawView = rawPointer;
}

void Explorer::LiveView::Initialise ( )
{
	mStatsCorpus.clear ( );
	mTimeCorpus.clear ( );

	b3D = true;
	Init3DCam ( );

	ofAddListener ( ofEvents ( ).mouseMoved, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mouseDragged, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mousePressed, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mouseReleased, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).mouseScrolled, this, &Explorer::LiveView::MouseEvent );
	ofAddListener ( ofEvents ( ).keyPressed, this, &Explorer::LiveView::KeyEvent );
	ofAddListener ( ofEvents ( ).keyReleased, this, &Explorer::LiveView::KeyEvent );
}

void Explorer::LiveView::MouseEvent ( ofMouseEventArgs& args )
{
	//types: 0-pressed, 1-moved, 2-released, 3-dragged, 4-scrolled
	//buttons: 0-left, 1-middle, 2-right
	//modifiers: 0-none, 1-shift, 2-ctrl, 4-alt (and combinations of them are added together)
	//position: x, y
	//scroll direction: x, y

	if ( b3D )
	{
		if ( args.type == 4 )
		{
			mCamera.dolly ( args.scrollY * mCamZoomSpeed3D );
		}
		else if ( args.type == 3 && args.button == 0 )
		{
			// get vectors
			glm::vec3 upNormalized = glm::normalize ( mCamera.getUpDir ( ) );
			glm::vec3 rightNormalized = glm::normalize ( mCamera.getSideDir ( ) );
			glm::vec3 focus = mCamera.getGlobalPosition ( ) - mCamPivot;
			glm::vec3 focusNormalized = glm::normalize ( focus );

			// calculate rotation angles
			float yawAngle = (args.x - mLastMouseX) * mCamRotateSpeed;
			float pitchAngle = (args.y - mLastMouseY) * mCamRotateSpeed;

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
	}
	else // 2D
	{
		if ( args.type == 4 )
		{
			mCamera.setScale ( mCamera.getScale ( ) + args.scrollY * mCamZoomSpeed2D );
			if ( mCamera.getScale ( ).x < 0.1 ) { mCamera.setScale ( 0.1 ); }
			mCamMoveSpeedScaleAdjusted = mCamMoveSpeed * mCamera.getScale ( ).x;
		}
		else if ( args.type == 3 )
		{
			mCamera.boom ( ( args.y - mLastMouseY ) * mCamMoveSpeedScaleAdjusted );
			mCamera.truck ( ( args.x - mLastMouseX ) * mCamMoveSpeedScaleAdjusted * -1 );
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

	if ( b3D )
	{

	}
	else // 2D
	{
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
}

void Explorer::LiveView::Update ( )
{
	deltaTime = ofGetElapsedTimef ( ) - lastUpdateTime;
	lastUpdateTime = ofGetElapsedTimef ( );
	if ( !bDraw ) { return; }

	if ( b3D )
	{

	}
	else
	{
		mCamera.boom ( ( mKeyboardMoveState[0] - mKeyboardMoveState[2] ) * mCamMoveSpeedScaleAdjusted * mKeyboardSpeedMulti * deltaTime );
		mCamera.truck ( ( mKeyboardMoveState[3] - mKeyboardMoveState[1] ) * mCamMoveSpeedScaleAdjusted * mKeyboardSpeedMulti * deltaTime );
	}
}

void Explorer::LiveView::Draw ( )
{
	if ( !bDraw ) { return; }

	ofEnableDepthTest ( );
	mCamera.begin ( );

	ofSetColor ( 255, 255, 255 );

	if ( mDisabledAxis != Axis::X ) { ofDrawLine ( { mSpaceMin, 0, 0 }, { mSpaceMax, 0, 0 } ); }
	if ( mDisabledAxis != Axis::Y ) { ofDrawLine ( { 0, mSpaceMin, 0 }, { 0, mSpaceMax, 0 } ); }
	if ( mDisabledAxis != Axis::Z ) { ofDrawLine ( { 0, 0, mSpaceMin }, { 0, 0, mSpaceMax } ); }

	if ( mDisabledAxis != Axis::X ) { ofDrawBitmapString ( xLabel , { mSpaceMax, 0, 0 } ); }
	if ( mDisabledAxis != Axis::Y ) { ofDrawBitmapString ( yLabel , { 0, mSpaceMax, 0 } ); }
	if ( mDisabledAxis != Axis::Z ) { ofDrawBitmapString ( zLabel , { 0, 0, mSpaceMax } ); }
	

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
				mesh.addColor ( { 255, 255, 255 } );
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
				mStatsCorpus.addColor ( { 255, 255, 255 } );
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
				mStatsCorpus.addColor ( { 255, 255, 255 } );
			}
		}

		bDraw = true;
		return;
	}
}

void Explorer::LiveView::FillDimensionTime ( int dimensionIndex, Axis axis )
{
	std::string dimensionName = dimensionIndex == -1 ? "Time" : mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Axis::X ) { xLabel = dimensionName; }
	else if ( axis == Axis::Y ) { yLabel = dimensionName; }
	else if ( axis == Axis::Z ) { zLabel = dimensionName; }

	Utils::TimeData* time = mRawView->GetTimeData ( );

	double min = 0, max = 0;
	FindScaling ( dimensionIndex, -1, min, max );

	for ( int file = 0; file < time->raw.size ( ); file++ )
	{
		for ( int timepoint = 0; timepoint < time->raw[file].size ( ); timepoint++ )
		{
			double value = 0.0;
			if ( dimensionIndex == -1 ) { value = ( timepoint * time->hopSize ) / time->sampleRates[file]; }
			else { value = time->raw[file][timepoint][dimensionIndex]; }

			if ( axis == Axis::COLOR )
			{
				value = ofMap ( value, min, max, mColorMin, mColorMax );
				ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
				currentColor.setHsb ( value, 255, 255 );
				mTimeCorpus[file].setColor ( timepoint, currentColor );
			}
			else
			{
				value = ofMap ( value, min, max, mSpaceMin, mSpaceMax );
				glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
				currentPoint[axis] = value;
				mTimeCorpus[file].setVertex ( timepoint, currentPoint );
			}
		}
	}
}

void Explorer::LiveView::FillDimensionStats ( int dimensionIndex, Axis axis )
{
	std::string dimensionName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Axis::X ) { xLabel = dimensionName; }
	else if ( axis == Axis::Y ) { yLabel = dimensionName; }
	else if ( axis == Axis::Z ) { zLabel = dimensionName; }

	int statisticIndex = dimensionIndex % mRawView->GetStatistics ( ).size ( );
	dimensionIndex /= mRawView->GetStatistics ( ).size ( );

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	double min = 0, max = 0;
	FindScaling ( dimensionIndex, statisticIndex, min, max );

	for ( int file = 0; file < stats->raw.size ( ); file++ )
	{
		if ( axis == Axis::COLOR )
		{
			double value = ofMap ( stats->raw[file][dimensionIndex][statisticIndex], min, max, mColorMin, mColorMax);
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( value, 255, 255 );
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			double value = ofMap ( stats->raw[file][dimensionIndex][statisticIndex], min, max, mSpaceMin, mSpaceMax );
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[axis] = value;
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}
}

void Explorer::LiveView::FillDimensionStatsReduced ( int dimensionIndex, Axis axis )
{
	std::string dimensioName = mRawView->GetDimensions ( )[dimensionIndex];
	if ( axis == Axis::X ) { xLabel = dimensioName; }
	else if ( axis == Axis::Y ) { yLabel = dimensioName; }
	else if ( axis == Axis::Z ) { zLabel = dimensioName; }

	Utils::StatsData* stats = mRawView->GetStatsData ( );

	double min = 0, max = 0;
	FindScaling ( dimensionIndex, -1, min, max );

	for ( int file = 0; file < stats->reduced.size ( ); file++ )
	{
		if ( axis == Axis::COLOR )
		{
			double value = ofMap ( stats->reduced[file][dimensionIndex], min, max, mColorMin, mColorMax );
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( value, 255, 255 );
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			double value = ofMap ( stats->reduced[file][dimensionIndex], min, max, mSpaceMin, mSpaceMax );
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[axis] = value;
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}
}

void Explorer::LiveView::FillDimensionNone ( Axis axis )
{
	if ( axis == Axis::X ) { xLabel = ""; }
	else if ( axis == Axis::Y ) { yLabel = ""; }
	else if ( axis == Axis::Z ) { zLabel = ""; }

	if ( mRawView->IsTimeAnalysis ( ) )
	{
		for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < mTimeCorpus[file].getNumVertices ( ); timepoint++ )
			{
				if ( axis == Axis::COLOR )
				{
					ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
					currentColor.set ( 255, 255, 255 );
					mTimeCorpus[file].setColor ( timepoint, currentColor );
				}
				else
				{
					glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
					currentPoint[axis] = 0;
					mTimeCorpus[file].setVertex ( timepoint, currentPoint );
				}
			}
		}
	}
	else
	{
		for ( int file = 0; file < mStatsCorpus.getNumVertices ( ); file++ )
		{
			if ( axis == Axis::COLOR )
			{
				ofColor currentColor = mStatsCorpus.getColor ( file );
				currentColor.set ( 255, 255, 255 );
				mStatsCorpus.setColor ( file, currentColor );
			}
			else
			{
				glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
				currentPoint[axis] = 0;
				mStatsCorpus.setVertex ( file, currentPoint );
			}
		}
	}
}

void Explorer::LiveView::FindScaling ( int dimensionIndex, int statisticIndex, double& min, double& max )
{
	if ( mRawView->IsTimeAnalysis ( ) )
	{
		Utils::TimeData* time = mRawView->GetTimeData ( );

		if ( dimensionIndex == -1 ) { min = 0; max = 0; }
		else
		{
			min = std::numeric_limits<double>::max ( );
			max = std::numeric_limits<double>::max ( ) * -1;
		}

		for ( int file = 0; file < time->raw.size ( ); file++ )
		{
			// If we're looking at time, we need to find the max timepoint
			if ( dimensionIndex == -1 )
			{
				double fileMax = time->raw[file].size ( ) * time->hopSize / time->sampleRates[file];
				if ( fileMax > max ) { max = fileMax; }
				continue;
			}

			// Otherwise, we're looking at a dimension
			for ( int timepoint = 0; timepoint < time->raw[file].size ( ); timepoint++ )
			{
				double value = time->raw[file][timepoint][dimensionIndex];

				if ( value < min ) { min = value; }
				if ( value > max ) { max = value; }
			}
		}

		return;
	}

	{
		Utils::StatsData* stats = mRawView->GetStatsData ( );

		min = std::numeric_limits<double>::max ( );
		max = std::numeric_limits<double>::max ( ) * -1;

		int loopSize = mRawView->IsReduction ( ) ? stats->reduced.size ( ) : stats->raw.size ( );

		for ( int file = 0; file < loopSize; file++ )
		{
			double value = 0.0;
			if ( !mRawView->IsReduction ( ) && statisticIndex > -1 ) { value = stats->raw[file][dimensionIndex][statisticIndex]; }
			else { value = stats->reduced[file][dimensionIndex]; }

			if ( value < min ) { min = value; }
			if ( value > max ) { max = value; }
		}
	}
}

void Explorer::LiveView::Init3DCam ( )
{ 
	double midSpacePoint = ( mSpaceMax + mSpaceMin ) / 2;
	mCamera.setPosition ( midSpacePoint, midSpacePoint, midSpacePoint ); 
	mCamPivot = ofPoint ( mSpaceMin, mSpaceMin, mSpaceMin );
	mCamera.lookAt ( mCamPivot ); 
	mCamera.setNearClip ( 0.01 ); 
	mCamera.setFarClip ( 99999 ); 
	mCamera.disableOrtho ( );
	mCamera.setScale ( 1 );
}

void Explorer::LiveView::Init2DCam ( Axis disabledAxis )
{ 
	double midSpacePoint = ( mSpaceMax + mSpaceMin ) / 2;
	mCamera.setPosition ( midSpacePoint, midSpacePoint, midSpacePoint );
	if ( disabledAxis == Axis::X ) { mCamera.lookAt ( { 0, midSpacePoint, midSpacePoint } ); }
	else if ( disabledAxis == Axis::Y ) { mCamera.lookAt ( { midSpacePoint, 0, midSpacePoint } ); }
	else { mCamera.lookAt ( { midSpacePoint, midSpacePoint, 0 } ); }
	mCamera.setNearClip ( 0.01 ); 
	mCamera.setFarClip ( 99999 );
	mCamera.enableOrtho ( );
	mCamera.setScale ( 1 );
	mCamMoveSpeedScaleAdjusted = mCamMoveSpeed * mCamera.getScale ( ).x;
}
