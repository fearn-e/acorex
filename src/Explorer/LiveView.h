#pragma once

#include "Utils/DimensionBounds.h"
#include "./SpaceDefs.h"
#include "./PointPicker.h"
#include "./RawView.h"
#include "./AudioPlayback.h"
#include "Utils/Data.h"
#include <ofMesh.h>
#include <ofEasyCam.h>
#include <ofSoundPlayer.h>

namespace Acorex {
namespace Explorer {

class LiveView {
public:

	LiveView ( ) { }
	~LiveView ( ) { }

	void Initialise ( );
	void ChangeAudioSettings ( size_t sampleRate, size_t bufferSize, ofSoundDevice outDevice );
	void KillAudio ( );
	void Exit ( );
	void RemoveListeners ( );

	// Process Functions ---------------------------

	void Update ( );
	void UpdatePlayheads ( );
	void OLD_UpdateAudioPlayers ( );
	void SlowUpdate ( );
	void Draw ( );

	// Sound Functions ------------------------------

	void CreatePlayhead ( );
	void KillPlayhead ( size_t playheadID );
	void OLD_PlaySound ( );

	// Filler Functions ----------------------------

	void CreatePoints ( );

	void FillDimensionTime ( int dimensionIndex, Utils::Axis axis );
	void FillDimensionStats ( int dimensionIndex, Utils::Axis axis );
	void FillDimensionStatsReduced ( int dimensionIndex, Utils::Axis axis );
	void FillDimensionNone ( Utils::Axis axis );
	void RefreshFileColors ( int fileIndex );

	// Camera Functions ----------------------------

	void Init3DCam ( );
	void Init2DCam ( Utils::Axis disabledAxis );

	void Zoom2DCam ( float y, bool mouse );
	void Zoom3DCam ( float y, bool mouse );
	void Rotate3DCam ( float x, float y, bool mouse );
	void Pan3DCam ( float x, float y, float z, bool mouse );

	// Setters & Getters ----------------------------

	void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }
	void Set3D ( bool is3D ) { b3D = is3D; }
	void SetColorFullSpectrum ( bool fullSpectrum ) { bColorFullSpectrum = fullSpectrum; }

	bool Is3D ( ) const { return b3D; }

	std::vector<Utils::VisualPlayhead>& GetPlayheads ( ) { return mPlayheads; }

	AudioPlayback* GetAudioPlayback ( ) { return &mAudioPlayback; }

	// Listener Functions --------------------------

	void KeyEvent ( ofKeyEventArgs& args );
	void MouseEvent ( ofMouseEventArgs& args );

private:
	bool bPointersShared = false;

	bool listenersAdded = false;

	bool bDebug = false;
	bool bDraw = false;
	bool b3D = true;
	bool bColorFullSpectrum = false;
	bool bLoopAudio = false;

	bool mKeyboardMoveState[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // W, A, S, D, R, F, Q, E, Z, X
	float mCamMoveSpeedScaleAdjusted = SpaceDefs::mCamMoveSpeed;

	float deltaTime = 0.1;
	float lastUpdateTime = 0;

	Utils::Axis mDisabledAxis = Utils::Axis::NONE;
	std::string xLabel = "X", yLabel = "Y", zLabel = "Z";
	int colorDimension = -1;

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
	std::vector<ofMesh> mTimeCorpus;
	ofMesh mStatsCorpus;

	std::vector<ofSoundPlayer> mSoundPlayers;
	std::vector<int> mPlayingFiles;
	std::vector<int> mPlayingTimeHeads;
	std::vector<float> mPlayingLastPositionMS;
	std::vector<ofColor> mPlayingLastColor;

	// Playheads -------------------------------------

	std::vector<Utils::VisualPlayhead> mPlayheads;

	// Camera ----------------------------------------

	std::shared_ptr<ofCamera> mCamera;
	ofPoint mCamPivot = ofPoint(0, 0, 0);
	int mLastMouseX = 0, mLastMouseY = 0;

	// Acorex Objects ------------------------------

	Utils::DimensionBounds mDimensionBounds;
	std::shared_ptr<PointPicker> mPointPicker;
	AudioPlayback mAudioPlayback;
};

} // namespace Explorer
} // namespace Acorex