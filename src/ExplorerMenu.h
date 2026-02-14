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

#include "Explorer/RawView.h"
#include "Explorer/LiveView.h"
#include "Utils/AudioSettingsManager.h"
#include "Utils/InterfaceDefs.h"
#include <ofxGui.h>
#include <ofxDropdown.h>

// TODO - split mMainPanel into 3 panels, Header, Corpus Controls, an Audio Manager
// TODO(cont) - to match the implementation in AnalyserMenu

namespace Acorex {

class ExplorerMenu {
public:
    ExplorerMenu ( );
    ~ExplorerMenu ( ) { }

    void Initialise ( );
    void Clear ( );

    void Open ( ) { Initialise ( ); }
    void Close ( ) { Clear ( ); }

    void Draw ( );
    void Update ( );
    void Exit ( );

    void RefreshUI ( );
    void WindowResized ( );

    void SetMenuLayout ( std::shared_ptr<Utils::MenuLayout>& menuLayout ) { mLayout = menuLayout; }

private:
    void SlowUpdate ( );

    // UI Management -------------------------------

    void OpenStartupPanel ( );
    void OpenFullPanel ( const std::vector<std::string>& corpusDimensionDefaults );

    void SetupPanelSectionHeader ( std::string corpusNameLabel );
    // TODO - pass in default values for all the controls
    void SetupPanelSectionCorpusControls ( std::vector<std::string> corpusDimensionDefaults );
    void SetupPanelSectionAudioManager ( );

    void RefreshStartupPanelUI ( );
    void RefreshFullPanelUI ( );

    // Listeners -----------------------------------

    void RemoveListeners ( );

    void AddListenersHeader ( );
    void RemoveListenersHeader ( );
    
    void AddListenersCorpusControls ( );
    void RemoveListenersCorpusControls ( );

    void AddListenersAudioManager ( );
    void RemoveListenersAudioManager ( );

    bool bListenersAddedHeader;
    bool bListenersAddedCorpusControls;
    bool bListenersAddedAudioManager;

    // Main Functions ------------------------------

    void OpenCorpus ( );
    void SwapDimension ( string dimension, Utils::Axis axis );
    int GetDimensionIndex ( std::string& dimension );
    void CameraSwitcher ( );

    // Listener Functions --------------------------

    void SwapDimensionX ( string& dimension );
    void SwapDimensionY ( string& dimension );
    void SwapDimensionZ ( string& dimension );
    void SwapDimensionColor ( string& dimension );
    void SwitchColorSpectrum ( bool& fullSpectrum );
    void ToggleLoopPlayheads ( bool& loop );
    void ToggleJumpSameFileAllowed ( bool& allowed );
    void SetJumpSameFileMinTimeDiff ( int& timeDiff );
    void SetCrossoverJumpChance ( float& jumpChance );
    void SetCrossfadeMaxSampleLength ( int& length );
    void SetMaxJumpDistanceSpace ( float& distance );
    void SetMaxJumpTargets ( int& targets );
    void SetVolume(float & volume);

    void SwapDimensionDynamicPan ( string& dimension );
    void SetPanningStrength ( float& strength );

    void MouseReleased ( ofMouseEventArgs& args );

    void RescanDevices ( );

    void SetApi ( string& dropdownName );
    void SetOutDevice ( string& dropdownName );
    void SetBufferSize ( string& dropdownName );

    void AudioOutputFailed ( );

    void ResetDeviceDropdown ( );
    void WriteApiDropdownDeviceCounts ( );

    // States --------------------------------------

    bool bDraw;
    bool bOpenCorpusWarningDraw;

    bool bIsCorpusOpen;
    bool bBlockDimensionFilling;
    
    Utils::Axis mDisabledAxis;

    // Timing --------------------------------------

    int mLastUpdateTime;
    const int mSlowUpdateInterval;

    int mOpenCorpusButtonClickTime;
    const int mOpenCorpusButtonTimeout;

    // Panels --------------------------------------

    ofxPanel mMainPanel;
    ofxLabel mCorpusNameLabel;
    ofxButton mOpenCorpusButton;
    unique_ptr<ofxDropdown> mDimensionDropdownX;
    unique_ptr<ofxDropdown> mDimensionDropdownY;
    unique_ptr<ofxDropdown> mDimensionDropdownZ;
    unique_ptr<ofxDropdown> mDimensionDropdownColor;
    ofxToggle mColorSpectrumSwitcher;

    ofxToggle mLoopPlayheadsToggle;
    ofxToggle mJumpSameFileAllowedToggle;
    ofxIntSlider mJumpSameFileMinTimeDiffSlider;
    ofxFloatSlider mCrossoverJumpChanceSlider;
    ofxIntSlider mCrossfadeMaxSampleLengthSlider;
    ofxFloatSlider mMaxJumpDistanceSpaceSlider;
    ofxIntSlider mMaxJumpTargetsSlider;

    ofxFloatSlider mVolumeSlider;

    unique_ptr<ofxDropdown> mDimensionDropdownDynamicPan;
    ofxFloatSlider mPanningStrengthSlider;

    unique_ptr<ofxDropdown> mApiDropdown;
    unique_ptr<ofxDropdown> mOutDeviceDropdown;
    unique_ptr<ofxIntDropdown> mBufferSizeDropdown;

    // Acorex Objects ------------------------------

    std::shared_ptr<Explorer::RawView> mRawView;
    Explorer::LiveView mLiveView;
    Utils::AudioSettingsManager mAudioSettingsManager;
    Utils::Colors mColors;
    std::shared_ptr<Utils::MenuLayout> mLayout;
};

} // namespace Acorex