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

#include "./ExplorerMenu.h"
#include <ofUtils.h>
#include <of3dGraphics.h>

using namespace Acorex;

void ExplorerMenu::Initialise ( bool HiDpi )
{
	// DPI -----------------------------------------
    {
		if ( HiDpi ) { mLayout.enableHiDpi ( ); }
		else { mLayout.disableHiDpi ( ); }
	}
	
	// Pointer Sharing -----------------------------
	{
		if ( !bViewPointerShared )
		{
			mRawView = std::make_shared<Explorer::RawView> ( );
			mLiveView.SetRawView ( mRawView );
			bViewPointerShared = true;
		}
	}

	// Clear --------------------------------------
	{
		RemoveListeners ( );

		mMainPanel.clear ( );

		mLiveView.Initialise ( );
	}

	// Variables ----------------------------------
	{
		mLastUpdateTime = 0;
		mSlowUpdateInterval = 100;

		mOpenCorpusButtonClickTime = 0;
		mOpenCorpusButtonTimeout = 3000;
	}

	// States ------------------------------------
	{
		bIsCorpusOpen = false;
		bOpenCorpusDrawWarning = false;
	}

	int dropdownScrollSpeed = 32;

	// Find Audio Devices --------------------------
	{
		outDevices.clear ( );
		ofSoundStream temp;
		for ( int i = 1; i < ofSoundDevice::Api::NUM_APIS; i++ )
		{
			std::vector<ofSoundDevice> devices = temp.getDeviceList ( (ofSoundDevice::Api)i );
			for ( auto& device : devices )
			{
				if ( device.outputChannels == 0 ) { continue; }
				outDevices.push_back ( device );
			}
		}
	}

	// Main Panel --------------------------------
	{
		mMainPanel.setup ( );

		mMainPanel.add ( mCorpusNameLabel.setup ( "", bInitialiseShouldLoad ? mRawView->GetCorpusName ( ) : "No Corpus Loaded" ) );
		mMainPanel.add ( mOpenCorpusButton.setup ( "Open Corpus" ) );

		mCorpusNameLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mOpenCorpusButton.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mDimensionDropdownX.reset ( );
		mDimensionDropdownY.reset ( );
		mDimensionDropdownZ.reset ( );
		mDimensionDropdownColor.reset ( );

		mDimensionDropdownX = make_unique<ofxDropdown> ( "X Dimension", dropdownScrollSpeed );
		mDimensionDropdownY = make_unique<ofxDropdown> ( "Y Dimension", dropdownScrollSpeed );
		mDimensionDropdownZ = make_unique<ofxDropdown> ( "Z Dimension", dropdownScrollSpeed );
		mDimensionDropdownColor = make_unique<ofxDropdown> ( "Color Dimension", dropdownScrollSpeed );

		if ( bInitialiseShouldLoad )
		{
			mDimensionDropdownX->add ( "None" );
			mDimensionDropdownY->add ( "None" );
			mDimensionDropdownZ->add ( "None" );
			mDimensionDropdownColor->add ( "None" );

			for ( auto& dimension : mRawView->GetDimensions ( ) )
			{
				mDimensionDropdownX->add ( dimension );
				mDimensionDropdownY->add ( dimension );
				mDimensionDropdownZ->add ( dimension );
				mDimensionDropdownColor->add ( dimension );
			}

			bool needStatisticDropdowns = !mRawView->IsTimeAnalysis ( ) && !mRawView->IsReduction ( );

			mMainPanel.add ( mDimensionDropdownX.get ( ) );
			mMainPanel.add ( mDimensionDropdownY.get ( ) );
			mMainPanel.add ( mDimensionDropdownZ.get ( ) );
			mMainPanel.add ( mDimensionDropdownColor.get ( ) );
		}

		mDimensionDropdownX->disableMultipleSelection ( );
		mDimensionDropdownY->disableMultipleSelection ( );
		mDimensionDropdownZ->disableMultipleSelection ( );
		mDimensionDropdownColor->disableMultipleSelection ( );

		mDimensionDropdownX->enableCollapseOnSelection ( );
		mDimensionDropdownY->enableCollapseOnSelection ( );
		mDimensionDropdownZ->enableCollapseOnSelection ( );
		mDimensionDropdownColor->enableCollapseOnSelection ( );

		mDimensionDropdownX->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownY->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownZ->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownColor->setDropDownPosition ( ofxDropdown::DD_LEFT );

		mDimensionDropdownX->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownY->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownZ->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownColor->setBackgroundColor ( mColors.interfaceBackgroundColor );

		if ( bInitialiseShouldLoad )
		{
			mMainPanel.add ( mColorSpectrumSwitcher.setup ( "Color Spectrum: Red<->Blue", false ) );
			mColorSpectrumSwitcher.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.SetColorFullSpectrum ( false );

			mMainPanel.add ( mLoopPlayheadsToggle.setup ( "Loop when reaching end of a file", false ) );
			mLoopPlayheadsToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetLoopPlayheads ( false );

			mMainPanel.add ( mJumpSameFileAllowedToggle.setup ( "Jump to same file allowed", false ) );
			mJumpSameFileAllowedToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetJumpSameFileAllowed ( false );

			mMainPanel.add ( mJumpSameFileMinTimeDiffSlider.setup ( "Same file jump min point difference", 2, 1, 30 ) );
			mJumpSameFileMinTimeDiffSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetJumpSameFileMinTimeDiff ( 2 );

			mMainPanel.add ( mCrossoverJumpChanceSlider.setup ( "Crossover Jump Chance", 0.05, 0.0, 1.0 ) );
			mCrossoverJumpChanceSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetCrossoverJumpChance ( 50 );

			size_t maxCrossfadeLength = mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction;
			mMainPanel.add ( mCrossfadeMaxSampleLengthSlider.setup ( "Crossfade Max Sample Length", maxCrossfadeLength, 1, maxCrossfadeLength ) );
			mCrossfadeMaxSampleLengthSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetCrossfadeSampleLength ( maxCrossfadeLength );

			mMainPanel.add ( mMaxJumpDistanceSpaceSlider.setup ( "Max Jump Distance Space", 0.05, 0.0, 1.0 ) );
			mMaxJumpDistanceSpaceSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetMaxJumpDistanceSpace ( 50 );

			mMainPanel.add ( mMaxJumpTargetsSlider.setup ( "Max Jump Targets", 5, 1, 10 ) );
			mMaxJumpTargetsSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mLiveView.GetAudioPlayback ( )->SetMaxJumpTargets ( 5 );
		}

		mBufferSizeDropdown.reset ( );
		mOutDeviceDropdown.reset ( );

		mBufferSizeDropdown = make_unique<ofxIntDropdown> ( "Buffer Size", dropdownScrollSpeed );
		mOutDeviceDropdown = make_unique<ofxDropdown> ( "Output Device", dropdownScrollSpeed );

		for ( int i = 0; i < outDevices.size ( ); i++ )
		{
			std::string deviceName = (std::to_string ( i + 1 ) + ". " + outDevices[i].name);
			mOutDeviceDropdown->add ( deviceName );
		}

		{
			mBufferSizeDropdown->add ( 64 );
			mBufferSizeDropdown->add ( 128 );
			mBufferSizeDropdown->add ( 256 );
			mBufferSizeDropdown->add ( 512 );
			mBufferSizeDropdown->add ( 1024 );
			mBufferSizeDropdown->add ( 2048 );
			mBufferSizeDropdown->add ( 4096 );
			mBufferSizeDropdown->add ( 8192 );
		}

		mBufferSizeDropdown->disableMultipleSelection ( );
		mOutDeviceDropdown->disableMultipleSelection ( );

		mBufferSizeDropdown->enableCollapseOnSelection ( );
		mOutDeviceDropdown->enableCollapseOnSelection ( );

		mBufferSizeDropdown->setDropDownPosition ( ofxIntDropdown::DD_LEFT );
		mOutDeviceDropdown->setDropDownPosition ( ofxDropdown::DD_LEFT );

		mBufferSizeDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mOutDeviceDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );

		mBufferSizeDropdown->setSelectedValueByIndex ( 3, true ); currentBufferSize = 512;
		mOutDeviceDropdown->setSelectedValueByIndex ( 0, true ); currentOutDevice = outDevices[0];

		mMainPanel.add ( mBufferSizeDropdown.get ( ) );
		mMainPanel.add ( mOutDeviceDropdown.get ( ) );

		mMainPanel.setPosition ( ofGetWidth ( ) - mLayout.explorePanelWidth, mLayout.explorePanelOriginY );
		mMainPanel.setWidthElements ( mLayout.explorePanelWidth );
		mMainPanel.disableHeader ( );
	}

	// Listeners --------------------------------
	{
		mOpenCorpusButton.addListener ( this, &ExplorerMenu::OpenCorpus );
		mDimensionDropdownX->addListener ( this, &ExplorerMenu::SwapDimensionX );
		mDimensionDropdownY->addListener ( this, &ExplorerMenu::SwapDimensionY );
		mDimensionDropdownZ->addListener ( this, &ExplorerMenu::SwapDimensionZ );
		mDimensionDropdownColor->addListener ( this, &ExplorerMenu::SwapDimensionColor );
		mColorSpectrumSwitcher.addListener ( this, &ExplorerMenu::SwitchColorSpectrum );
		mLoopPlayheadsToggle.addListener ( this, &ExplorerMenu::ToggleLoopPlayheads );
		mJumpSameFileAllowedToggle.addListener ( this, &ExplorerMenu::ToggleJumpSameFileAllowed );
		mJumpSameFileMinTimeDiffSlider.addListener ( this, &ExplorerMenu::SetJumpSameFileMinTimeDiff );
		mCrossoverJumpChanceSlider.addListener ( this, &ExplorerMenu::SetCrossoverJumpChance );
		mCrossfadeMaxSampleLengthSlider.addListener ( this, &ExplorerMenu::SetCrossfadeMaxSampleLength );
		mMaxJumpDistanceSpaceSlider.addListener ( this, &ExplorerMenu::SetMaxJumpDistanceSpace );
		mMaxJumpTargetsSlider.addListener ( this, &ExplorerMenu::SetMaxJumpTargets );

		mBufferSizeDropdown->addListener ( this, &ExplorerMenu::SetBufferSize );
		mOutDeviceDropdown->addListener ( this, &ExplorerMenu::SetOutDevice );

		ofAddListener ( ofEvents ( ).mouseReleased, this, &ExplorerMenu::MouseReleased );
		bListenersAdded = true;
	}

	bInitialiseShouldLoad = false;
}

void ExplorerMenu::Show ( )
{
	bDraw = true;
}

// hides menu without resetting
void ExplorerMenu::Hide ( )
{
	bDraw = false;
}

void ExplorerMenu::Draw ( )
{
	if ( !bDraw ) { return; }

	mLiveView.Draw ( );

	// call pointpicker draw

	mMainPanel.draw ( );

	// draw playhead panels

	{
		for ( auto& playhead : mLiveView.GetPlayheads ( ) )
		{
			// highlight the playhead position if panel is hovered
			if ( playhead.panelRect.inside ( ofGetMouseX ( ), ofGetMouseY ( ) ) )
				playhead.highlight = true;
			else
				playhead.highlight = false;

			// draw panel
			ofSetColor ( playhead.color );
			ofDrawRectangle ( playhead.panelRect );
			ofSetColor ( 200, 200, 200, 255 );
			ofDrawRectangle ( playhead.panelRect.x, playhead.panelRect.y, playhead.panelRect.width, playhead.panelRect.height / 2 );
			// draw playhead id in the top left
			ofSetColor ( 0, 0, 0, 255 );
			ofDrawBitmapString ( "ID:" + ofToString ( playhead.playheadID ), playhead.panelRect.x + 5, playhead.panelRect.y + 15 );
			// draw file index in the top left
			ofDrawBitmapString ( "File: " + ofToString ( playhead.fileIndex ), playhead.panelRect.x + 5, playhead.panelRect.y + 30 );
			// draw sample index in the top left
			ofDrawBitmapString ( "Samp: " + ofToString ( playhead.sampleIndex ), playhead.panelRect.x + 5, playhead.panelRect.y + 45 );

			// draw another smaller rectangle in the top right corner of the panel
			int smallRectSize = ( playhead.panelRect.width + playhead.panelRect.height ) / 20;
			ofSetColor ( mColors.interfaceBackgroundColor );
			ofDrawRectangle ( playhead.panelRect.x + playhead.panelRect.width - smallRectSize, playhead.panelRect.y, smallRectSize, smallRectSize );
			// draw an X in the top right corner of the panel
			ofSetColor ( 255, 0, 0, 255 );
			ofSetLineWidth ( 2 );
			ofDrawLine ( playhead.panelRect.x + playhead.panelRect.width - smallRectSize, playhead.panelRect.y, playhead.panelRect.x + playhead.panelRect.width, playhead.panelRect.y + smallRectSize );
			ofDrawLine ( playhead.panelRect.x + playhead.panelRect.width, playhead.panelRect.y, playhead.panelRect.x + playhead.panelRect.width - smallRectSize, playhead.panelRect.y + smallRectSize );
		}
	}
}

void ExplorerMenu::Update ( )
{
	mLiveView.Update ( );

	if ( ofGetElapsedTimeMillis ( ) - mLastUpdateTime > mSlowUpdateInterval )
	{
		mLastUpdateTime = ofGetElapsedTimeMillis ( );
		SlowUpdate ( );
	}
}

void ExplorerMenu::SlowUpdate ( )
{
	if ( bOpenCorpusDrawWarning && ofGetElapsedTimeMillis ( ) - mOpenCorpusButtonClickTime > mOpenCorpusButtonTimeout )
	{
		bOpenCorpusDrawWarning = false;
		mOpenCorpusButton.setName ( "Open Corpus" );
	}

	mLiveView.SlowUpdate ( );
}

void ExplorerMenu::Exit ( )
{
	RemoveListeners ( );
	mLiveView.Exit ( );
}

void ExplorerMenu::RemoveListeners ( )
{
	if ( !bListenersAdded ) { return; }
	mOpenCorpusButton.removeListener ( this, &ExplorerMenu::OpenCorpus );
	mDimensionDropdownX->removeListener ( this, &ExplorerMenu::SwapDimensionX );
	mDimensionDropdownY->removeListener ( this, &ExplorerMenu::SwapDimensionY );
	mDimensionDropdownZ->removeListener ( this, &ExplorerMenu::SwapDimensionZ );
	mDimensionDropdownColor->removeListener ( this, &ExplorerMenu::SwapDimensionColor );
	mColorSpectrumSwitcher.removeListener ( this, &ExplorerMenu::SwitchColorSpectrum );
	mLoopPlayheadsToggle.removeListener ( this, &ExplorerMenu::ToggleLoopPlayheads );
	mJumpSameFileAllowedToggle.removeListener ( this, &ExplorerMenu::ToggleLoopPlayheads );
	mJumpSameFileMinTimeDiffSlider.removeListener ( this, &ExplorerMenu::ToggleLoopPlayheads );
	mCrossoverJumpChanceSlider.removeListener ( this, &ExplorerMenu::SetCrossoverJumpChance );
	mCrossfadeMaxSampleLengthSlider.removeListener ( this, &ExplorerMenu::SetCrossfadeMaxSampleLength );
	mMaxJumpDistanceSpaceSlider.removeListener ( this, &ExplorerMenu::SetMaxJumpDistanceSpace );
	mMaxJumpTargetsSlider.removeListener ( this, &ExplorerMenu::SetMaxJumpTargets );

	ofRemoveListener ( ofEvents ( ).mouseReleased, this, &ExplorerMenu::MouseReleased );
	bListenersAdded = false;
}

// Main Functions ------------------------------

void ExplorerMenu::OpenCorpus ( )
{
	bBlockDimensionFilling = true;

	if ( bIsCorpusOpen && !bOpenCorpusDrawWarning )
	{
		bOpenCorpusDrawWarning = true;
		mOpenCorpusButtonClickTime = ofGetElapsedTimeMillis ( );
		mOpenCorpusButton.setName ( "!! Close Current? !!" );
		return;
	}

	bOpenCorpusDrawWarning = false;
	mOpenCorpusButton.setName ( "Open Corpus" );

	mLiveView.KillAudio ( ); // waits for confirmation

	bool success = mRawView->LoadCorpus ( );
	if ( !success ) { return; }
	
	bInitialiseShouldLoad = true;
	Initialise ( mLayout.HiDpi );

	mLiveView.CreatePoints ( );

	// set default dropdown values
	std::string xDimension = "None", yDimension = "None", zDimension = "None", colorDimension = "None";
	{
		int dimensionCount = mRawView->GetDimensions ( ).size ( );
		if ( mRawView->IsTimeAnalysis ( ) || mRawView->IsReduction ( ) )
		{
			xDimension = mRawView->GetDimensions ( ).size ( ) > 0 ? mRawView->GetDimensions ( )[0] : "None";
			yDimension = mRawView->GetDimensions ( ).size ( ) > 1 ? mRawView->GetDimensions ( )[1] : "None";
			zDimension = mRawView->GetDimensions ( ).size ( ) > 2 ? mRawView->GetDimensions ( )[2] : "None";
			colorDimension = mRawView->GetDimensions ( ).size ( ) > 3 ? mRawView->GetDimensions ( )[3] : "None";
		}
		else
		{
			xDimension = mRawView->GetDimensions ( ).size ( ) > 0 ? mRawView->GetDimensions ( )[0] : "None";
			yDimension = mRawView->GetDimensions ( ).size ( ) > 7 ? mRawView->GetDimensions ( )[7] : "None";
			zDimension = mRawView->GetDimensions ( ).size ( ) > 14 ? mRawView->GetDimensions ( )[14] : "None";
			colorDimension = mRawView->GetDimensions ( ).size ( ) > 21 ? mRawView->GetDimensions ( )[21] : "None";
		}
		
	}

	mDimensionDropdownX->setSelectedValueByName ( xDimension, true );
	mDimensionDropdownY->setSelectedValueByName ( yDimension, true );
	mDimensionDropdownZ->setSelectedValueByName ( zDimension, true );
	mDimensionDropdownColor->setSelectedValueByName ( colorDimension, true );

	bBlockDimensionFilling = false;

	SwapDimension ( xDimension, Utils::Axis::X );
	SwapDimension ( yDimension, Utils::Axis::Y );
	SwapDimension ( zDimension, Utils::Axis::Z );

	bIsCorpusOpen = true;

	SwapDimension ( colorDimension, Utils::Axis::COLOR );

	mLiveView.ChangeAudioSettings ( currentBufferSize, currentOutDevice );
}

void ExplorerMenu::SwapDimension ( string dimension, Utils::Axis axis )
{
	if ( bBlockDimensionFilling ) { return; }

	if ( dimension == "None" )					{ mLiveView.FillDimensionNone ( axis ); }
	else
	{
		int dimensionIndex = GetDimensionIndex ( dimension );
		if ( dimensionIndex == -1 ) { return; }

		if ( mRawView->IsTimeAnalysis ( ) )		{ mLiveView.FillDimensionTime ( dimensionIndex, axis ); }
		else if ( !mRawView->IsReduction ( ) )	{ mLiveView.FillDimensionStats ( dimensionIndex, axis ); }
		else									{ mLiveView.FillDimensionStatsReduced ( dimensionIndex, axis ); }
	}
	
	if ( bIsCorpusOpen )
	{
		CameraSwitcher ( );
		// TODO - if axis != COLOR, retrain point picker // is this still needed here? already retraining in liveview
	}
}

int ExplorerMenu::GetDimensionIndex ( std::string& dimension )
{
	for ( int i = 0; i < mRawView->GetDimensions ( ).size ( ); i++ )
	{
		if ( mRawView->GetDimensions ( )[i] == dimension )
		{
			return i;
		}
	}
	ofLogWarning ( "LiveView" ) << "Dimension " << dimension << " not found";
	return -1;
}

void ExplorerMenu::CameraSwitcher ( )
{
	bool isXNone = mDimensionDropdownX->getAllSelected ( )[0] == "None";
	bool isYNone = mDimensionDropdownY->getAllSelected ( )[0] == "None";
	bool isZNone = mDimensionDropdownZ->getAllSelected ( )[0] == "None";
	int numDisabledAxes = isXNone + isYNone + isZNone;

	Utils::Axis							  disabledAxis = Utils::Axis::NONE;
	if		( isXNone )					{ disabledAxis = Utils::Axis::X; }
	else if ( isYNone )					{ disabledAxis = Utils::Axis::Y; }
	else if ( isZNone )					{ disabledAxis = Utils::Axis::Z; }
	else if ( numDisabledAxes > 1 )		{ disabledAxis = Utils::Axis::MULTIPLE; }

	bool current3D = mLiveView.Is3D ( );

	if ( disabledAxis == Utils::Axis::NONE || disabledAxis == Utils::Axis::MULTIPLE )
	{
		if ( !mLiveView.Is3D ( ) )
		{
			mLiveView.Set3D ( true );
			mLiveView.Init3DCam ( );
		}
	}
	else
	{
		if ( mLiveView.Is3D ( ) || disabledAxis != mDisabledAxis )
		{
			mLiveView.Set3D ( false );
			mLiveView.Init2DCam ( disabledAxis );
			mDisabledAxis = disabledAxis;
		}
	}
}

// Listener Functions --------------------------

void ExplorerMenu::WindowResized ( )
{
	mMainPanel.setPosition ( ofGetWidth ( ) - mLayout.explorePanelWidth, mLayout.explorePanelOriginY );

	int rectWidth = ofGetWidth ( ) / 10; int rectSpacing = ofGetWidth ( ) / 100; int rectHeight = ofGetHeight ( ) / 10;
	for ( auto& playhead : mLiveView.GetPlayheads ( ) )
	{
		playhead.panelRect = ofRectangle (	rectSpacing * mLiveView.GetPlayheads ( ).size ( ) + rectWidth * (mLiveView.GetPlayheads ( ).size ( ) - 1),
											ofGetHeight ( ) - rectHeight - 5,
											rectWidth,
											rectHeight );
	}
}

void ExplorerMenu::SwapDimensionX ( string& dimension )
{
	SwapDimension ( dimension, Utils::Axis::X );
}

void ExplorerMenu::SwapDimensionY ( string& dimension )
{
	SwapDimension ( dimension, Utils::Axis::Y );
}

void ExplorerMenu::SwapDimensionZ ( string& dimension )
{
	SwapDimension ( dimension, Utils::Axis::Z );
}

void ExplorerMenu::SwapDimensionColor ( string& dimension )
{
	SwapDimension ( dimension, Utils::Axis::COLOR );
}

void ExplorerMenu::SwitchColorSpectrum ( bool& fullSpectrum )
{
	if ( fullSpectrum ) { mColorSpectrumSwitcher.setName ( "Color Spectrum: Full" ); }
	else { mColorSpectrumSwitcher.setName ( "Color Spectrum: Red<->Blue" ); }
	mLiveView.SetColorFullSpectrum ( fullSpectrum );
	SwapDimension ( mDimensionDropdownColor->getAllSelected ( )[0], Utils::Axis::COLOR );
}

void ExplorerMenu::ToggleLoopPlayheads ( bool& loop )
{
	mLiveView.GetAudioPlayback ( )->SetLoopPlayheads ( loop );
}

void ExplorerMenu::ToggleJumpSameFileAllowed ( bool& allowed )
{
	mLiveView.GetAudioPlayback ( )->SetJumpSameFileAllowed ( allowed );
}

void ExplorerMenu::SetJumpSameFileMinTimeDiff ( int& timeDiff )
{
	mLiveView.GetAudioPlayback ( )->SetJumpSameFileMinTimeDiff ( timeDiff );
}

void ExplorerMenu::SetCrossoverJumpChance ( float& jumpChance )
{
	mLiveView.GetAudioPlayback ( )->SetCrossoverJumpChance ( (int)(jumpChance * 1000) );
}

void ExplorerMenu::SetCrossfadeMaxSampleLength ( int& length )
{
	mLiveView.GetAudioPlayback ( )->SetCrossfadeSampleLength ( length );
}

void ExplorerMenu::SetMaxJumpDistanceSpace ( float& distance )
{
	mLiveView.GetAudioPlayback ( )->SetMaxJumpDistanceSpace ( (int)(distance * 1000) );
}

void ExplorerMenu::SetMaxJumpTargets ( int& targets )
{
	mLiveView.GetAudioPlayback ( )->SetMaxJumpTargets ( targets );
}

void ExplorerMenu::MouseReleased ( ofMouseEventArgs& args )
{
	for ( auto& playhead : mLiveView.GetPlayheads ( ) )
	{
		int smallRectSize = ( playhead.panelRect.width + playhead.panelRect.height ) / 20;
		ofRectangle smallRect = ofRectangle ( playhead.panelRect.x + playhead.panelRect.width - smallRectSize, playhead.panelRect.y, smallRectSize, smallRectSize );

		if ( smallRect.inside ( args.x, args.y ) )
		{
			mLiveView.GetAudioPlayback ( )->KillPlayhead ( playhead.playheadID );
			return;
		}
	}
}

void ExplorerMenu::SetBufferSize ( int& bufferSize )
{
	currentBufferSize = bufferSize;
	mLiveView.ChangeAudioSettings ( currentBufferSize, currentOutDevice );
}

void ExplorerMenu::SetOutDevice ( string& outDevice )
{
	outDevice = outDevice.substr ( 0, outDevice.find_first_of ( "." ) );
	int deviceIndex = std::stoi ( outDevice ) - 1;

	currentOutDevice = outDevices[deviceIndex];

	mLiveView.ChangeAudioSettings ( currentBufferSize, currentOutDevice );
}