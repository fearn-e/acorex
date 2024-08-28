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

#pragma once

#include "Explorer/RawView.h"
#include "Explorer/LiveView.h"
#include "Utils/InterfaceDefs.h"
#include <ofxGui.h>
#include <ofxDropdown.h>

namespace Acorex {

class ExplorerMenu {
public:
	ExplorerMenu ( ) { }
	~ExplorerMenu ( ) { }

	void Initialise ( bool HiDpi );
	void Show ( );
	void Hide ( );
	void Draw ( );
	void Update ( );
	void Exit ( );

	void WindowResized ( );

private:
	void SlowUpdate ( );
	void RemoveListeners ( );

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

	void MouseReleased ( ofMouseEventArgs& args );

	void SetBufferSize ( int& bufferSize );
	void SetOutDevice ( string& outDevice );

	std::vector<ofSoundDevice> outDevices;
	ofSoundDevice currentOutDevice;
	int currentBufferSize;

	// States --------------------------------------

	bool bDraw = false;

	bool bIsCorpusOpen = false; bool bBlockDimensionFilling = false;
	bool bOpenCorpusDrawWarning = false;
	bool bInitialiseShouldLoad = false;
	bool bListenersAdded = false;
	
	bool bViewPointerShared = false;

	Utils::Axis mDisabledAxis = Utils::Axis::NONE;

	// Timing --------------------------------------

	int mLastUpdateTime = 0;
	int mSlowUpdateInterval = 100;

	int mOpenCorpusButtonClickTime = 0;
	int mOpenCorpusButtonTimeout = 3000;

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

	unique_ptr<ofxIntDropdown> mBufferSizeDropdown;
	unique_ptr<ofxDropdown> mOutDeviceDropdown;

	// Acorex Objects ------------------------------

	std::shared_ptr<Explorer::RawView> mRawView;
	Explorer::LiveView mLiveView;
	Utils::Colors mColors;
	Utils::MenuLayout mLayout;
};

} // namespace Acorex