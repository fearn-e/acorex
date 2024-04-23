#pragma once

#include "Utils/DimensionBounds.h"
#include "./PointPicker.h"
#include "./RawView.h"
#include "Utils/Data.h"
#include <ofMesh.h>
#include <ofEasyCam.h>

namespace Acorex {
namespace Explorer {

class LiveView {
public:

	LiveView ( ) { }
	~LiveView ( ) { }

	void Initialise ( );
	void Exit ( );
	void RemoveListeners ( );

	// Process Functions ---------------------------

	void Update ( );
	void SlowUpdate ( );
	void Draw ( );

	// Filler Functions ----------------------------

	void CreatePoints ( );

	void FillDimensionTime ( int dimensionIndex, Utils::Axis axis );
	void FillDimensionStats ( int dimensionIndex, Utils::Axis axis );
	void FillDimensionStatsReduced ( int dimensionIndex, Utils::Axis axis );
	void FillDimensionNone ( Utils::Axis axis );

	// Camera Functions ----------------------------

	void Init3DCam ( );
	void Init2DCam ( Utils::Axis disabledAxis );

	void Zoom3DCam ( int y );
	void Rotate3DCam ( int x, int y );
	void Pan3DCam ( int x, int y, bool mouse );

	// Setters & Getters ----------------------------

	void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }
	void Set3D ( bool is3D ) { b3D = is3D; }

	bool Is3D ( ) const { return b3D; }

	// Listener Functions --------------------------

	void KeyEvent ( ofKeyEventArgs& args );
	void MouseEvent ( ofMouseEventArgs& args );

private:
	bool bDraw = false;
	bool b3D = true;

	float deltaTime = 0.1;
	float lastUpdateTime = 0;

	Utils::Axis mDisabledAxis = Utils::Axis::NONE;
	std::string xLabel = "X", yLabel = "Y", zLabel = "Z";

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
	std::vector<ofMesh> mTimeCorpus;
	ofMesh mStatsCorpus;

	// Camera ----------------------------------------

	ofCamera mCamera;
	ofPoint mCamPivot = ofPoint(0, 0, 0);
	int mLastMouseX = 0, mLastMouseY = 0;

	// Camera Adjustments --------------------

	float mCamZoomSpeed3D = 24.0; float mCamZoomSpeed2D = 0.1;
	float mZoomMin3D = 50.0; float mZoomMax3D = 5000.0;
	float mZoomMin2D = 0.1; float mZoomMax2D = 10.0;
	float mCamMoveSpeed = 1.0; float mCamMoveSpeedScaleAdjusted = mCamMoveSpeed;
	float mCamRotateSpeed = 0.006;
	bool mKeyboardMoveState[4] = { 0, 0, 0, 0 }; float mKeyboardSpeedMulti = 800.0;

	// Space and Color Limits --------------------

	double mSpaceMin = 0; double mSpaceMax = 1000;
	double mColorMin = 0; double mColorMax = 215;

	// Acorex Objects ------------------------------

	Utils::DimensionBounds mDimensionBounds;
	PointPicker mPointPicker;
};

} // namespace Explorer
} // namespace Acorex