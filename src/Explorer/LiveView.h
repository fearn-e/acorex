#pragma once

#include "Utils/DimensionBounds.h"
#include "./SpaceDefs.h"
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

	void UpdateAlphas ( );

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

	bool mKeyboardMoveState[4] = { 0, 0, 0, 0 };
	float mCamMoveSpeedScaleAdjusted = SpaceDefs::mCamMoveSpeed;

	float deltaTime = 0.1;
	float lastUpdateTime = 0;

	Utils::Axis mDisabledAxis = Utils::Axis::NONE;
	std::string xLabel = "X", yLabel = "Y", zLabel = "Z";

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
	std::vector<ofMesh> mTimeCorpus;
	ofMesh mStatsCorpus;

	int mLastNearestPointFile = -1;
	int mLastNearestPointTime = -1;

	// Camera ----------------------------------------

	std::shared_ptr<ofCamera> mCamera;
	ofPoint mCamPivot = ofPoint(0, 0, 0);
	int mLastMouseX = 0, mLastMouseY = 0;

	// Acorex Objects ------------------------------

	Utils::DimensionBounds mDimensionBounds;
	PointPicker mPointPicker;
};

} // namespace Explorer
} // namespace Acorex