#pragma once

#include "./RawView.h"
#include <ofMesh.h>
#include <ofEasyCam.h>

namespace Acorex {
namespace Explorer {

class LiveView {
public:
	enum Axis {
		X = 0,
		Y = 1,
		Z = 2,
		COLOR = 3,
		NONE = 4,
		MULTIPLE = 5
	};

	LiveView ( ) { }
	~LiveView ( ) { }

	void SetRawView ( std::shared_ptr<RawView>& rawPointer );
	void Initialise ( );
	void Exit ( );
	void RemoveListeners ( );

	void Update ( );
	void Draw ( );

	void CreatePoints ( );

	void FillDimensionTime ( int dimensionIndex, Axis axis );
	void FillDimensionStats ( int dimensionIndex, Axis axis );
	void FillDimensionStatsReduced ( int dimensionIndex, Axis axis );
	void FillDimensionNone ( Axis axis );

	void FindScaling ( int dimensionIndex, int statisticIndex, double& min, double& max );

	void Set3D ( bool is3D ) { b3D = is3D; }
	void Init3DCam ( );
	void Init2DCam ( Axis disabledAxis );

	bool Is3D ( ) const { return b3D; }

	// Camera Functions ----------------------------

	void Zoom3DCam ( int y );
	void Rotate3DCam ( int x, int y );
	void Pan3DCam ( int x, int y, bool mouse );

	// Listener Functions --------------------------

	void KeyEvent ( ofKeyEventArgs& args );
	void MouseEvent ( ofMouseEventArgs& args );

private:
	bool bDraw = false;
	bool b3D = true;

	float deltaTime = 0.1;
	float lastUpdateTime = 0;

	Axis mDisabledAxis = NONE;
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
	double mColorMin = 0; double mColorMax = 255;
};

} // namespace Explorer
} // namespace Acorex