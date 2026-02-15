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

#pragma once

#include "Explorer/SpaceDefs.h"
#include "Explorer/PointPicker.h"
#include "Explorer/RawView.h"
#include "Explorer/AudioPlayback.h"
#include "Utilities/Data.h"
#include "Utilities/DimensionBounds.h"
#include "Utilities/InterfaceDefs.h"

#include <ofMesh.h>
#include <ofEasyCam.h>

namespace Acorex {
namespace Explorer {

class LiveView {
public:

    LiveView ( );
    ~LiveView ( ) { }

    void Initialise ( );
    void Clear ( );
    bool StartAudio ( std::pair<ofSoundDevice, int> audioSettings );
    bool RestartAudio ( std::pair<ofSoundDevice, int> audioSettings ) { return StartAudio ( audioSettings ); }
    void Exit ( );

    void AddListeners ( );
    void RemoveListeners ( );

    // Process Functions ---------------------------

    void Update ( );
    void UpdatePlayheads ( );
    void SlowUpdate ( );
    void Draw ( );

    // Sound Functions ------------------------------

    void CreatePlayhead ( );
    void KillPlayhead ( size_t playheadID );

    // Filler Functions ----------------------------

    void CreatePoints ( );

    void FillDimension ( int dimensionIndex, Utilities::Axis axis );
    void ClearDimension ( Utilities::Axis axis );
    void RefreshFileColors ( int fileIndex );

    // Camera Functions ----------------------------

    void Init3DCam ( );
    void Init2DCam ( Utilities::Axis disabledAxis );

    void Zoom2DCam ( float y, bool mouse );
    void Zoom3DCam ( float y, bool mouse );
    void Rotate3DCam ( float x, float y, bool mouse );
    void Pan3DCam ( float x, float y, float z, bool mouse );

    // Setters & Getters ----------------------------

    void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; mAudioPlayback.SetRawView ( rawPointer ); }
    void SetMenuLayout ( std::shared_ptr<Utilities::MenuLayout>& layout ) { mLayout = layout; }
    void Set3D ( bool is3D ) { b3D = is3D; }
    void SetColorFullSpectrum ( bool fullSpectrum ) { bColorFullSpectrum = fullSpectrum; }

    bool Is3D ( ) const { return b3D; }

    std::vector<Utilities::VisualPlayhead>& GetPlayheads ( ) { return mPlayheads; }

    AudioPlayback* GetAudioPlayback ( ) { return &mAudioPlayback; }

    // Listener Functions --------------------------

    void KeyEvent ( ofKeyEventArgs& args );
    void MouseEvent ( ofMouseEventArgs& args );

private:
    bool bListenersAdded;

    bool bDebug;
    bool bUserPaused;
    bool bDraw;
    bool b3D;
    bool bColorFullSpectrum;

    bool mKeyboardMoveState[10];
    float mCamMoveSpeedScaleAdjusted;

    float deltaTime;
    float lastUpdateTime;

    Utilities::Axis mDisabledAxis;
    std::string xLabel, yLabel, zLabel;
    int colorDimension;

    std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
    std::vector<ofMesh> mCorpusMesh;

    // Playheads -------------------------------------

    std::vector<Utilities::VisualPlayhead> mPlayheads;

    // Camera ----------------------------------------

    std::shared_ptr<ofCamera> mCamera;
    ofPoint mCamPivot;
    int mLastMouseX, mLastMouseY;

    // Acorex Objects ------------------------------

    Utilities::DimensionBounds mDimensionBounds;
    std::shared_ptr<PointPicker> mPointPicker;
    AudioPlayback mAudioPlayback;
    std::shared_ptr<Utils::MenuLayout> mLayout;
};

} // namespace Explorer
} // namespace Acorex