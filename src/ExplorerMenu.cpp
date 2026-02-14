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

#include "./ExplorerMenu.h"
#include "Utils/InterfaceDefs.h"
#include <ofUtils.h>
#include <of3dGraphics.h>

#include "Utils/TemporaryDefaults.h"

using namespace Acorex;

ExplorerMenu::ExplorerMenu ( ) :    mSlowUpdateInterval ( 100 ), mOpenCorpusButtonTimeout ( 3000 ),
                                    bListenersAddedHeader ( false ), bListenersAddedCorpusControls ( false ), 
                                    bListenersAddedAudioManager ( false )
{
    mRawView = std::make_shared<Explorer::RawView> ( );
    mLiveView.SetRawView ( mRawView );

    bDraw = false;
    bOpenCorpusWarningDraw = false;

    bIsCorpusOpen = false;
    bBlockDimensionFilling = false;

    mDisabledAxis = Utils::Axis::NONE;

    mLastUpdateTime = 0;
    mOpenCorpusButtonClickTime = 0;
}

void ExplorerMenu::Initialise ( )
{
    Clear ( );

    OpenStartupPanel ( );
}

void ExplorerMenu::Clear ( )
{
    bDraw = false;
    bOpenCorpusWarningDraw = false;

    bIsCorpusOpen = false;
    bBlockDimensionFilling = false;

    mDisabledAxis = Utils::Axis::NONE;

    mLiveView.Clear ( );

    mRawView->ClearCorpus ( );

    RemoveListeners ( );

    mMainPanel.clear ( );
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
            std::string fileName = mRawView->GetDataset ( )->fileList[playhead.fileIndex];
            fileName = fileName.substr ( fileName.find_last_of ( "/" ) + 1 );
            fileName = fileName.substr ( fileName.find_last_of ( "\\" ) + 1 );
            int maxLength = playhead.panelRect.width / 10 - 7;
            if ( fileName.length ( ) > maxLength )
            {
                fileName = fileName.substr ( 0, maxLength );
                fileName += "...";
            }
            ofDrawBitmapString ( "File: " + fileName, playhead.panelRect.x + 5, playhead.panelRect.y + 30 );
            // draw sample index in the top left
            ofDrawBitmapString ( "Samp: " + ofToString ( playhead.sampleIndex ), playhead.panelRect.x + 5, playhead.panelRect.y + 45 );

            // draw another smaller rectangle in the top right corner of the panel
            int smallRectSize = ( playhead.panelRect.width + playhead.panelRect.height ) / 10;
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
    mLiveView.SlowUpdate ( );

    if ( bOpenCorpusWarningDraw && ofGetElapsedTimeMillis ( ) - mOpenCorpusButtonClickTime > mOpenCorpusButtonTimeout )
    {
        bOpenCorpusWarningDraw = false;
        mOpenCorpusButton.setName ( "Open Corpus" );
    }
}

void ExplorerMenu::Exit ( )
{
    RemoveListenersHeader ( );
    RemoveListenersCorpusControls ( );
    RemoveListenersAudioManager ( );
    mLiveView.Exit ( );
}

// UI Management -------------------------------

void ExplorerMenu::OpenStartupPanel ( )
{
    mAudioSettingsManager.RefreshDeviceListChanged ( );

    RemoveListeners ( );

    mMainPanel.clear ( );

    mMainPanel.setup ( );

    SetupPanelSectionHeader ( "No Corpus Loaded" );

    SetupPanelSectionAudioManager ( );

    mMainPanel.setPosition ( ofGetWidth ( ) - mLayout->getExplorePanelWidth ( ), mLayout->getModePanelOriginY ( ) );
    mMainPanel.setWidthElements ( mLayout->getExplorePanelWidth ( ) );
    mMainPanel.disableHeader ( );

    AddListenersHeader ( );
    AddListenersAudioManager ( );

    bDraw = true;
}

void ExplorerMenu::OpenFullPanel ( const std::vector<std::string>& corpusDimensionDefaults )
{
    mAudioSettingsManager.RefreshDeviceListChanged ( );

    RemoveListeners ( );

    mMainPanel.clear ( );

    mMainPanel.setup ( );

    SetupPanelSectionHeader ( mRawView->GetCorpusName ( ) );

    SetupPanelSectionCorpusControls ( corpusDimensionDefaults );

    SetupPanelSectionAudioManager ( );

    mMainPanel.setPosition ( ofGetWidth ( ) - mLayout->getExplorePanelWidth ( ), mLayout->getModePanelOriginY ( ) );
    mMainPanel.setWidthElements ( mLayout->getExplorePanelWidth ( ) );
    mMainPanel.disableHeader ( );

    AddListenersHeader ( );
    AddListenersCorpusControls ( );
    AddListenersAudioManager ( );

    bDraw = true;
}

void ExplorerMenu::SetupPanelSectionHeader ( std::string corpusNameLabel )
{
    mMainPanel.add ( mCorpusNameLabel.setup ( "", corpusNameLabel ) );
    mCorpusNameLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );

    mMainPanel.add ( mOpenCorpusButton.setup ( "Open Corpus" ) );
    mOpenCorpusButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
}

void ExplorerMenu::SetupPanelSectionCorpusControls ( std::vector<std::string> corpusDimensionDefaults )
{
    if ( corpusDimensionDefaults.size ( ) < 5 )
    {
        ofLogWarning ( "ExplorerMenu::SetupPanelSectionCorpusControls" ) << "Expected 5 corpus dimension defaults, only received " << corpusDimensionDefaults.size ( ) << ".";
        for ( int i = corpusDimensionDefaults.size ( ); i < 5; i++ )
        {
            corpusDimensionDefaults.push_back ( "None" );
        }
    }

    // X Dimension Dropdown
    mDimensionDropdownX.reset ( );
    mDimensionDropdownX = make_unique<ofxDropdown> ( static_cast<std::string>("X Dimension"), Utils::ofxDropdownScrollSpeed );
    mDimensionDropdownX->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownX->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownX.get ( ) );
    mDimensionDropdownX->disableMultipleSelection ( );
    mDimensionDropdownX->enableCollapseOnSelection ( );
    mDimensionDropdownX->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownX->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownX->setSelectedValueByName ( corpusDimensionDefaults[0], false );

    // Y Dimension Dropdown
    mDimensionDropdownY.reset ( );
    mDimensionDropdownY = make_unique<ofxDropdown> ( static_cast<std::string>("Y Dimension"), Utils::ofxDropdownScrollSpeed );
    mDimensionDropdownY->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownY->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownY.get ( ) );
    mDimensionDropdownY->disableMultipleSelection ( );
    mDimensionDropdownY->enableCollapseOnSelection ( );
    mDimensionDropdownY->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownY->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownY->setSelectedValueByName ( corpusDimensionDefaults[1], false );

    // Z Dimension Dropdown
    mDimensionDropdownZ.reset ( );
    mDimensionDropdownZ = make_unique<ofxDropdown> ( static_cast<std::string>("Z Dimension"), Utils::ofxDropdownScrollSpeed );
    mDimensionDropdownZ->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownZ->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownZ.get ( ) );
    mDimensionDropdownZ->disableMultipleSelection ( );
    mDimensionDropdownZ->enableCollapseOnSelection ( );
    mDimensionDropdownZ->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownZ->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownZ->setSelectedValueByName ( corpusDimensionDefaults[2], false );

    // TODO - move color settings to the top of this panel section

    // Colour Dimension Dropdown
    mDimensionDropdownColor.reset ( );
    mDimensionDropdownColor = make_unique<ofxDropdown> ( static_cast<std::string>("Color Dimension"), Utils::ofxDropdownScrollSpeed );
    mDimensionDropdownColor->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownColor->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownColor.get ( ) );
    mDimensionDropdownColor->disableMultipleSelection ( );
    mDimensionDropdownColor->enableCollapseOnSelection ( );
    mDimensionDropdownColor->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownColor->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownColor->setSelectedValueByName ( corpusDimensionDefaults[3], false );

    // Color Spectrum Toggle
    mMainPanel.add ( mColorSpectrumSwitcher.setup ( "Color Spectrum: Red<->Blue", DEFAULT_COLOR_SPECTRUM ) );
    mColorSpectrumSwitcher.setBackgroundColor ( mColors.interfaceBackgroundColor );

    //

    // EOF Loop Playheads Toggle
    mMainPanel.add ( mLoopPlayheadsToggle.setup ( "Loop when reaching end of a file", DEFAULT_LOOP_PLAYHEADS ) );
    mLoopPlayheadsToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Playheads Jump Same File Toggle
    mMainPanel.add ( mJumpSameFileAllowedToggle.setup ( "Jump to same file allowed", DEFAULT_JUMP_SAME_FILE_ALLOWED ) );
    mJumpSameFileAllowedToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Playheads Same File Jump Minimum Distance Slider
    mMainPanel.add ( mJumpSameFileMinTimeDiffSlider.setup ( "Same file jump min point difference", DEFAULT_JUMP_SAME_FILE_MIN_DIFF, 1, 30 ) );
    mJumpSameFileMinTimeDiffSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Jump Chance Slider
    float crossoverJumpChance = static_cast<float>(DEFAULT_CROSSOVER_JUMP_CHANCE_X1000) / 1000.0;
    mMainPanel.add ( mCrossoverJumpChanceSlider.setup ( "Crossover Jump Chance", crossoverJumpChance, 0.0, 1.0 ) );
    mCrossoverJumpChanceSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Crossfade Max Length Slider
    size_t maxCrossfadeLength = mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction;
    size_t defaultCrossfadeLength = min ( static_cast<size_t>(DEFAULT_CROSSFADE_MAX_SAMPLE_LENGTH), maxCrossfadeLength );
    mMainPanel.add ( mCrossfadeMaxSampleLengthSlider.setup ( "Crossfade Max Sample Length", defaultCrossfadeLength, 1, maxCrossfadeLength ) );
    mCrossfadeMaxSampleLengthSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Jump Max Distance Slider
    float maxJumpDistance = static_cast<float>(DEFAULT_MAX_JUMP_DISTANCE_SPACE_X1000) / 1000.0;
    mMainPanel.add ( mMaxJumpDistanceSpaceSlider.setup ( "Max Jump Distance Space", maxJumpDistance, 0.0, 1.0 ) );
    mMaxJumpDistanceSpaceSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Jump Max Targets Slider
    mMainPanel.add ( mMaxJumpTargetsSlider.setup ( "Max Jump Targets", DEFAULT_MAX_JUMP_TARGETS, 1, 10 ) );
    mMaxJumpTargetsSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
    
    // 
    
    // Global Volume Slider
    float volume = static_cast<float>(DEFAULT_VOLUME_X1000) / 1000.0;
    mMainPanel.add ( mVolumeSlider.setup ( "Volume", volume, 0.0, 1.0 ) );
    mVolumeSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Dynamic Panning Dimension Dropdown
    mDimensionDropdownDynamicPan.reset ( );
    mDimensionDropdownDynamicPan = make_unique<ofxDropdown> ( static_cast<std::string>("Dynamic Panning Dimension"), Utils::ofxDropdownScrollSpeed );
    mDimensionDropdownDynamicPan->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownDynamicPan->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownDynamicPan.get ( ) );
    mDimensionDropdownDynamicPan->disableMultipleSelection ( );
    mDimensionDropdownDynamicPan->enableCollapseOnSelection ( );
    mDimensionDropdownDynamicPan->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownDynamicPan->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownDynamicPan->setSelectedValueByName ( corpusDimensionDefaults[4], false );

    // Global Panning Strength Slider
    float panningStrength = static_cast<float>(DEFAULT_PANNING_STRENGTH_X1000) / 1000.0;
    mMainPanel.add ( mPanningStrengthSlider.setup ( "Panning Width", panningStrength, 0.0, 1.0 ) );
    mPanningStrengthSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
}

void ExplorerMenu::SetupPanelSectionAudioManager ( )
{
    size_t apiIndex = mAudioSettingsManager.GetCurrentApiIndex ( );
    size_t outDeviceIndex = mAudioSettingsManager.GetCurrentDeviceIndex ( );
    int bufferSize = mAudioSettingsManager.GetCurrentBufferSize ( );

    mApiDropdown.reset ( );
    mApiDropdown = make_unique<ofxDropdown> ( (string)"Audio API", Utils::ofxDropdownScrollSpeed );
    for ( size_t i = 0; i < mAudioSettingsManager.GetApiCount ( ); i++ )
    {
        std::string apiName = std::string ( mAudioSettingsManager.GetApiName ( i ) );
        std::string descriptiveName = std::string ( mAudioSettingsManager.GetApiName ( i ) ) + " (" + std::to_string ( mAudioSettingsManager.GetOutDeviceCount ( i ) - 1 ) + " devices)";
        
        mApiDropdown->add ( apiName, descriptiveName );
    }
    mApiDropdown->disableMultipleSelection ( );
    mApiDropdown->enableCollapseOnSelection ( );
    mApiDropdown->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mApiDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mApiDropdown->setSelectedValueByIndex ( apiIndex, false );
    mMainPanel.add ( mApiDropdown.get ( ) );

    mOutDeviceDropdown.reset ( );
    mOutDeviceDropdown = make_unique<ofxDropdown> ( (string)"Output Device", Utils::ofxDropdownScrollSpeed );
    for ( size_t i = 0; i < mAudioSettingsManager.GetCurrentApiDevicesOut ( ).size ( ); i++ )
    {
        std::string deviceName = mAudioSettingsManager.GetCurrentApiDevicesOut ( )[i].name;
        mOutDeviceDropdown->add ( deviceName );
    }
    mOutDeviceDropdown->disableMultipleSelection ( );
    mOutDeviceDropdown->enableCollapseOnSelection ( );
    mOutDeviceDropdown->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mOutDeviceDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mOutDeviceDropdown->setSelectedValueByIndex ( outDeviceIndex, false );
    mMainPanel.add ( mOutDeviceDropdown.get ( ) );

    mBufferSizeDropdown.reset ( );
    mBufferSizeDropdown = make_unique<ofxIntDropdown> ( (string)"Buffer Size", Utils::ofxDropdownScrollSpeed );
    for ( auto& each : mAudioSettingsManager.GetBufferSizes ( ) ) { mBufferSizeDropdown->add ( each ); }
    mBufferSizeDropdown->disableMultipleSelection ( );
    mBufferSizeDropdown->enableCollapseOnSelection ( );
    mBufferSizeDropdown->setDropDownPosition ( ofxIntDropdown::DD_LEFT );
    mBufferSizeDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mBufferSizeDropdown->setSelectedValueByName ( std::to_string ( bufferSize ), false );
    mMainPanel.add ( mBufferSizeDropdown.get ( ) );
}

void ExplorerMenu::RefreshUI ( )
{
    if ( !bIsCorpusOpen )
        RefreshStartupPanelUI ( );
    else
        RefreshFullPanelUI ( );
}

void ExplorerMenu::WindowResized ( )
{
    mMainPanel.setPosition ( ofGetWidth ( ) - mLayout->getExplorePanelWidth ( ), mLayout->getModePanelOriginY ( ) );

    if ( !bIsCorpusOpen ) { return; }

    int rectWidth = ofGetWidth ( ) / 10; int rectSpacing = ofGetWidth ( ) / 100; int rectHeight = ofGetHeight ( ) / 10;
    for ( auto& playhead : mLiveView.GetPlayheads ( ) ) // TODO - fix playhead visual stacking when window resizing bug, easy fix
    {
        playhead.panelRect = ofRectangle ( rectSpacing * mLiveView.GetPlayheads ( ).size ( ) + rectWidth * (mLiveView.GetPlayheads ( ).size ( ) - 1),
                                            ofGetHeight ( ) - rectHeight - 5,
                                            rectWidth,
                                            rectHeight );
    }
}

void ExplorerMenu::RefreshStartupPanelUI ( )
{
    mMainPanel.setPosition ( ofGetWidth ( ) - mLayout->getExplorePanelWidth ( ), mLayout->getModePanelOriginY ( ) );

    // for split later: header panel
    mCorpusNameLabel.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mOpenCorpusButton.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );

    // for split later: audio manager panel
    mApiDropdown->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mOutDeviceDropdown->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mBufferSizeDropdown->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );

    mMainPanel.setWidthElements ( mLayout->getExplorePanelWidth ( ) );
    mMainPanel.sizeChangedCB ( );
}

void ExplorerMenu::RefreshFullPanelUI ( )
{
    mMainPanel.setPosition ( ofGetWidth ( ) - mLayout->getExplorePanelWidth ( ), mLayout->getModePanelOriginY ( ) );

    // for split later: header panel
    mCorpusNameLabel.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mOpenCorpusButton.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );

    // for split later: corpus controls panel
    mDimensionDropdownX->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mDimensionDropdownY->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mDimensionDropdownZ->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    
    mDimensionDropdownColor->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mColorSpectrumSwitcher.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    
    mLoopPlayheadsToggle.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mJumpSameFileAllowedToggle.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mJumpSameFileMinTimeDiffSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mCrossoverJumpChanceSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mCrossfadeMaxSampleLengthSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mMaxJumpDistanceSpaceSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mMaxJumpTargetsSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    
    mVolumeSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mDimensionDropdownDynamicPan->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mPanningStrengthSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );

    // for split later: audio manager panel
    mApiDropdown->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mOutDeviceDropdown->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mBufferSizeDropdown->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );

    mMainPanel.setWidthElements ( mLayout->getExplorePanelWidth ( ) );
    mMainPanel.sizeChangedCB ( );
}

// Listeners -----------------------------------

void ExplorerMenu::RemoveListeners ( )
{
    RemoveListenersHeader ( );
    RemoveListenersCorpusControls ( );
    RemoveListenersAudioManager ( );
}

void ExplorerMenu::AddListenersHeader ( )
{
    if ( bListenersAddedHeader ) { return; }

    mOpenCorpusButton.addListener ( this, &ExplorerMenu::OpenCorpus );

    bListenersAddedHeader = true;
}

void ExplorerMenu::RemoveListenersHeader ( )
{
    if ( !bListenersAddedHeader ) { return; }

    mOpenCorpusButton.removeListener ( this, &ExplorerMenu::OpenCorpus );

    bListenersAddedHeader = false;
}

void ExplorerMenu::AddListenersCorpusControls ( )
{
    if ( bListenersAddedCorpusControls ) { return; }

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

    mVolumeSlider.addListener ( this, &ExplorerMenu::SetVolume );

    mDimensionDropdownDynamicPan->addListener ( this, &ExplorerMenu::SwapDimensionDynamicPan );
    mPanningStrengthSlider.addListener ( this, &ExplorerMenu::SetPanningStrength );

    ofAddListener ( ofEvents ( ).mouseReleased, this, &ExplorerMenu::MouseReleased );

    bListenersAddedCorpusControls = true;
}

void ExplorerMenu::RemoveListenersCorpusControls ( )
{
    if ( !bListenersAddedCorpusControls ) { return; }

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

    mVolumeSlider.removeListener ( this, &ExplorerMenu::SetVolume );

    mDimensionDropdownDynamicPan->removeListener ( this, &ExplorerMenu::SwapDimensionDynamicPan );
    mPanningStrengthSlider.removeListener ( this, &ExplorerMenu::SetPanningStrength );

    ofRemoveListener ( ofEvents ( ).mouseReleased, this, &ExplorerMenu::MouseReleased );

    bListenersAddedCorpusControls = false;
}

void ExplorerMenu::AddListenersAudioManager ( )
{
    if ( bListenersAddedAudioManager ) { return; }

    ofAddListener ( mApiDropdown->dropdownHidden_E, this, &ExplorerMenu::SetApi );
    ofAddListener ( mOutDeviceDropdown->dropdownHidden_E, this, &ExplorerMenu::SetOutDevice );
    ofAddListener ( mBufferSizeDropdown->dropdownHidden_E, this, &ExplorerMenu::SetBufferSize );

    ofAddListener ( mApiDropdown->dropdownWillShow_E, this, &ExplorerMenu::RescanDevices );
    ofAddListener ( mOutDeviceDropdown->dropdownWillShow_E, this, &ExplorerMenu::RescanDevices );

    bListenersAddedAudioManager = true;
}

void ExplorerMenu::RemoveListenersAudioManager ( )
{
    if ( !bListenersAddedAudioManager ) { return; }

    ofRemoveListener ( mApiDropdown->dropdownHidden_E, this, &ExplorerMenu::SetApi );
    ofRemoveListener ( mOutDeviceDropdown->dropdownHidden_E, this, &ExplorerMenu::SetOutDevice );
    ofRemoveListener ( mBufferSizeDropdown->dropdownHidden_E, this, &ExplorerMenu::SetBufferSize );

    ofRemoveListener ( mApiDropdown->dropdownWillShow_E, this, &ExplorerMenu::RescanDevices );
    ofRemoveListener ( mOutDeviceDropdown->dropdownWillShow_E, this, &ExplorerMenu::RescanDevices );

    bListenersAddedAudioManager = false;
}

// Main Functions ------------------------------

void ExplorerMenu::OpenCorpus ( )
{
    // TODO - is this flag still needed now that OpenFullPanel has all value sets set to bNotify = false?
    bBlockDimensionFilling = true;
    
    if ( bIsCorpusOpen && !bOpenCorpusWarningDraw )
    {
        bOpenCorpusWarningDraw = true;
        mOpenCorpusButtonClickTime = ofGetElapsedTimeMillis ( );
        mOpenCorpusButton.setName ( "!! Close Current? !!" );
        return;
    }
    bOpenCorpusWarningDraw = false;

    // clear stuff
    mLiveView.Clear ( );

    bIsCorpusOpen = false;
    mRawView->ClearCorpus ( );

    // load new corpus, return to startup panel if fails
    if ( !mRawView->LoadCorpus ( ) )
    {
        Initialise ( );
        return;
    }

    mLiveView.Initialise ( );

    mLiveView.CreatePoints ( ); // TODO - combine with mLiveView.Initialise ( );?

    std::string xDimensionName          = mRawView->GetDimensions ( ).size ( ) > 1 ? mRawView->GetDimensions ( )[1] : "None";
    std::string yDimensionName          = mRawView->GetDimensions ( ).size ( ) > 2 ? mRawView->GetDimensions ( )[2] : "None";
    std::string zDimensionName          = mRawView->GetDimensions ( ).size ( ) > 3 ? mRawView->GetDimensions ( )[3] : "None";

    std::string colorDimensionName      = mRawView->GetDimensions ( )[0];

    size_t randomPanDimensionIndex      = mRawView->GetDimensions ( ).size ( ) > 1 ? ofRandom ( 1, mRawView->GetDimensions ( ).size ( ) - 1 ) : 0;
    std::string dynamicPanDimensionName = mRawView->GetDimensions ( )[randomPanDimensionIndex];

    OpenFullPanel ( std::vector<std::string> { xDimensionName, yDimensionName, zDimensionName, colorDimensionName, dynamicPanDimensionName } );

    bBlockDimensionFilling = false;

    // TODO - change these so that only the final call of the 3 triggers point picker Train ( )
    SwapDimension ( xDimensionName, Utils::Axis::X );
    SwapDimension ( yDimensionName, Utils::Axis::Y );
    SwapDimension ( zDimensionName, Utils::Axis::Z );
    SwapDimension ( colorDimensionName, Utils::Axis::COLOR );
    SwapDimension ( dynamicPanDimensionName, Utils::Axis::DYNAMIC_PAN );

    CameraSwitcher ( );

    bIsCorpusOpen = true;

    if ( mLiveView.StartAudio ( mAudioSettingsManager.GetCurrentAudioSettings ( ) ) ) { return; }

    AudioOutputFailed ( );
}

// TODO - change how this and FillDimension and Train (point picker) work, should have a function that triggers training and one that doesn't (for initial filling)
void ExplorerMenu::SwapDimension ( string dimension, Utils::Axis axis )
{
    if ( bBlockDimensionFilling ) { return; }

    if ( axis == Utils::Axis::DYNAMIC_PAN )
    {
        if ( dimension == "None" )
        {
            mLiveView.GetAudioPlayback ( )->SetDynamicPan ( false, 0 );
        }
        else
        {
            int dimensionIndex = GetDimensionIndex ( dimension );
            if ( dimensionIndex == -1 ) { return; }
            mLiveView.GetAudioPlayback ( )->SetDynamicPan ( true, dimensionIndex );
        }

        return;
    }

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

void ExplorerMenu::SetVolume(float & volume)
{
    mLiveView.GetAudioPlayback ( )->SetVolume ( (int)(volume * 1000) );
}

void ExplorerMenu::SwapDimensionDynamicPan ( string& dimension )
{
    SwapDimension ( dimension, Utils::Axis::DYNAMIC_PAN );
}

void ExplorerMenu::SetPanningStrength ( float& strength )
{
    mLiveView.GetAudioPlayback ( )->SetPanningStrength ( (int)(strength * 1000) );
}

void ExplorerMenu::MouseReleased ( ofMouseEventArgs& args )
{
    for ( auto& playhead : mLiveView.GetPlayheads ( ) )
    {
        int smallRectSize = ( playhead.panelRect.width + playhead.panelRect.height ) / 10;
        ofRectangle smallRect = ofRectangle ( playhead.panelRect.x + playhead.panelRect.width - smallRectSize, playhead.panelRect.y, smallRectSize, smallRectSize );

        if ( smallRect.inside ( args.x, args.y ) )
        {
            mLiveView.GetAudioPlayback ( )->KillPlayhead ( playhead.playheadID );
            return;
        }
    }
}

void ExplorerMenu::RescanDevices ( )
{
    // TODO TEST - could have some edge cases depending on how ofxDropdown works
    //          - this gets called when the dropdown is about to be shown - does it actually update the dropdown correctly if there's a change?
    //          - or does the dropdown then have to be closed and opened again to show this change

    bool modified = mAudioSettingsManager.RefreshDeviceListChanged ( );

    if ( !modified ) { return; }

    WriteApiDropdownDeviceCounts ( );

    ResetDeviceDropdown ( );

    if ( !bIsCorpusOpen ) { return; }

    if ( mLiveView.RestartAudio ( mAudioSettingsManager.GetCurrentAudioSettings ( ) ) ) { return; }

    AudioOutputFailed ( );
}

void Acorex::ExplorerMenu::SetApi ( string& dropdownName )
{
    if ( mAudioSettingsManager.GetCurrentApiIndex ( ) == mApiDropdown->getSelectedOptionIndex ( ) ) { return; }

    bool success = mAudioSettingsManager.ChangeSelectedApi ( mApiDropdown->getSelectedOptionIndex ( ) );

    if ( !success )
    {
        ofLogError ( "ExplorerMenu" ) << "Failed to change audio API to selected API."
            << ". Selecting API: " << mAudioSettingsManager.GetCurrentApiName ( )
            << ", Selecting Device: " << mAudioSettingsManager.GetOutDevices ( mAudioSettingsManager.GetCurrentApiIndex ( ) )[mAudioSettingsManager.GetCurrentDeviceIndex ( )].name;
        mApiDropdown->setSelectedValueByIndex ( mAudioSettingsManager.GetCurrentApiIndex ( ), false );
    }

    ResetDeviceDropdown ( );

    if ( !bIsCorpusOpen ) { return; }

    if ( mLiveView.RestartAudio ( mAudioSettingsManager.GetCurrentAudioSettings ( ) ) ) { return; }

    AudioOutputFailed ( );
}

void ExplorerMenu::SetOutDevice ( string& dropdownName )
{
    //if ( bBlockAudioSettingsUIListenersTemporaryFix ) { return; }

    if ( mAudioSettingsManager.GetCurrentDeviceIndex ( ) == mOutDeviceDropdown->getSelectedOptionIndex ( ) ) { return; }

    bool success = mAudioSettingsManager.ChangeSelectedDevice ( mOutDeviceDropdown->getSelectedOptionIndex ( ) );

    if ( !success )
    {
        ofLogError ( "ExplorerMenu" ) << "Failed to change output device to selected device.";
        mOutDeviceDropdown->setSelectedValueByIndex ( mAudioSettingsManager.GetCurrentDeviceIndex ( ), false );
    }

    if ( !bIsCorpusOpen ) { return; }

    if ( mLiveView.RestartAudio ( mAudioSettingsManager.GetCurrentAudioSettings ( ) ) ) { return; }

    AudioOutputFailed ( );
}

void ExplorerMenu::SetBufferSize ( string& dropdownName )
{
    if ( mAudioSettingsManager.GetCurrentBufferSize ( ) == mBufferSizeDropdown->getAllSelected ( )[0] ) { return; }

    mAudioSettingsManager.SetBufferSize ( mBufferSizeDropdown->getAllSelected ( )[0] );

    if ( !bIsCorpusOpen ) { return; }

    if ( mLiveView.RestartAudio ( mAudioSettingsManager.GetCurrentAudioSettings ( ) ) ) { return; }

    AudioOutputFailed ( );
}

void ExplorerMenu::AudioOutputFailed ( )
{
    // TODO - more error handling here? also more user feedback? - e.g. set Device/Api/Buffer dropdowns to red bg colour?

    ofLogError ( "ExplorerMenu" ) << "Audio output failed to restart with current settings. This likely means the selected output device is currently unavailable. Please check your audio output device and try again.";
}

void ExplorerMenu::ResetDeviceDropdown ( )
{
    mOutDeviceDropdown->clear ( );

    for ( size_t i = 0; i < mAudioSettingsManager.GetCurrentApiDevicesOut ( ).size ( ); i++ )
    {
        std::string deviceName = mAudioSettingsManager.GetCurrentApiDevicesOut ( )[i].name;
        mOutDeviceDropdown->add ( deviceName );
    }

    mOutDeviceDropdown->setSelectedValueByIndex ( mAudioSettingsManager.GetCurrentDeviceIndex ( ), false );
}

void ExplorerMenu::WriteApiDropdownDeviceCounts ( )
{
    for ( size_t i = 0; i < mAudioSettingsManager.GetApiCount ( ); i++ )
    {
        std::string descriptiveName = std::string ( mAudioSettingsManager.GetApiName ( i ) ) + " (" + std::to_string ( mAudioSettingsManager.GetOutDeviceCount ( i ) - 1 ) + " devices)";
        mApiDropdown->updateOptionName ( mApiDropdown->getOptionAt ( i ), descriptiveName );
    }
}