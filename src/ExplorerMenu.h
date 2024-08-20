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
	void SetCrossoverJumpChance ( float& jumpChance );
	void SetCrossfadeMaxSampleLength ( int& length );
	void SetMaxJumpDistanceSpace ( float& distance );
	void SetMaxJumpTargets ( int& targets );

	void MouseReleased ( ofMouseEventArgs& args );

	void SetSampleRate ( int& sampleRate );
	void SetBufferSize ( int& bufferSize );
	void SetOutDevice ( string& outDevice );

	std::vector<ofSoundDevice> outDevices;
	ofSoundDevice currentOutDevice;
	int currentSampleRate;
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
	ofxFloatSlider mCrossoverJumpChanceSlider;		
	ofxIntSlider mCrossfadeMaxSampleLengthSlider;	
	ofxFloatSlider mMaxJumpDistanceSpaceSlider;		
	ofxIntSlider mMaxJumpTargetsSlider;

	unique_ptr<ofxIntDropdown> mSampleRateDropdown;
	unique_ptr<ofxIntDropdown> mBufferSizeDropdown;
	unique_ptr<ofxDropdown> mOutDeviceDropdown;

	// Acorex Objects ------------------------------

	std::shared_ptr<Explorer::RawView> mRawView;
	Explorer::LiveView mLiveView;
	Utils::Colors mColors;
	Utils::MenuLayout mLayout;
};

} // namespace Acorex