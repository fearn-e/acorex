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

#include "ExplorerMenu.h"

#include "Utilities/InterfaceDefs.h"
#include "Utilities/TemporaryDefaults.h"

#include <ofUtils.h>
#include <of3dGraphics.h>

using namespace Acorex;

ExplorerMenu::ExplorerMenu ( ) :    mSlowUpdateInterval ( 100 ), mOpenCorpusButtonTimeout ( 3000 ),
                                    bListenersAddedHeader ( false ), bListenersAddedCorpusControls ( false ), 
                                    bListenersAddedAudioManager ( false )
{
    mRawView = std::make_shared<Explorer::RawView> ( );
    mLiveView.SetRawView ( mRawView );

    bDraw = false;
    bDrawOpenCorpusWarning = false;

    bIsCorpusOpen = false;
    bBlockDimensionFilling = false;

    mDisabledAxis = Utilities::Axis::NONE;

    mLastUpdateTime = 0;
    mOpenCorpusButtonClickTime = 0;
}

void ExplorerMenu::Initialise ( )
{
    Clear ( );

    OpenStartupPanel ( );

    mMIDI.Initialise ( );
}

void ExplorerMenu::Clear ( )
{
    bool clearedOpenCorpus = bIsCorpusOpen;

    bDraw = false;
    bDrawOpenCorpusWarning = false;

    bIsCorpusOpen = false;
    bBlockDimensionFilling = false;

    mDisabledAxis = Utilities::Axis::NONE;

    mLiveView.Clear ( );

    mRawView->ClearCorpus ( );

    RemoveListeners ( );

    mMainPanel.clear ( );

    if ( clearedOpenCorpus ) { ofLogNotice ( "Explorer" ) << "Cleared corpus."; }
}

void ExplorerMenu::Draw ( )
{
    if ( !bDraw ) { return; }

    mLiveView.Draw ( );

    // call pointpicker draw

    mMainPanel.draw ( );

    // draw playhead panels
    // TODO - this all needs to be cleaned up, made prettier, and a lot of it moved into InterfaceDefs.h
    for ( auto& playhead : mLiveView.GetPlayheads ( ) )
    {
        // highlight the playhead position if panel is hovered
        if ( playhead.panelRect.inside ( ofGetMouseX ( ), ofGetMouseY ( ) ) )
            playhead.highlight = true;
        else
            playhead.highlight = false;

        // draw panel outline
        ofColor outlineColor = playhead.highlight ? ofColor ( 255, 255, 255, 255 ) : ofColor ( 50, 50, 50, 255 );
        int lineWidth = playhead.highlight ? 3 : 2;
        ofSetColor ( outlineColor );
        ofDrawRectangle ( playhead.panelRect.x - lineWidth, playhead.panelRect.y - lineWidth, 
                          playhead.panelRect.width + (lineWidth * 2), playhead.panelRect.height + (lineWidth * 2) );

        // draw panel
        ofSetColor ( 90, 90, 90, 255 );
        ofDrawRectangle ( playhead.panelRect );
        ofColor dampenedPlayheadColor = playhead.color.getLerped ( ofColor ( 255, 255, 255, 255 ), 0.2 );
        ofSetColor ( dampenedPlayheadColor );
        ofDrawRectangle ( playhead.playheadColorRect );

        // draw playhead id
        ofSetColor ( 0, 0, 0, 255 );
        ofDrawBitmapString ( ofToString ( playhead.playheadID ), playhead.panelRect.x + ( playhead.panelRect.width / 3 ), playhead.panelRect.y + ( playhead.panelRect.height / 3 ) );

        // draw X "kill playhead" button
        ofSetColor ( mColors.interfaceBackgroundColor );
        ofDrawRectangle ( playhead.killButtonRect );
        ofSetColor ( 255, 0, 0, 255 );
        ofSetLineWidth ( 2 );
        ofDrawLine ( playhead.killButtonRect.x, playhead.killButtonRect.y, playhead.killButtonRect.x + playhead.killButtonRect.width, playhead.killButtonRect.y + playhead.killButtonRect.height );
        ofDrawLine ( playhead.killButtonRect.x + playhead.killButtonRect.width, playhead.killButtonRect.y, playhead.killButtonRect.x, playhead.killButtonRect.y + playhead.killButtonRect.height );
    }
}

void ExplorerMenu::Update ( )
{
    mLiveView.Update ( );

    mMIDI.Update ( );

    if ( ofGetElapsedTimeMillis ( ) - mLastUpdateTime > mSlowUpdateInterval )
    {
        mLastUpdateTime = ofGetElapsedTimeMillis ( );
        SlowUpdate ( );
    }
}

void ExplorerMenu::SlowUpdate ( )
{
    mLiveView.SlowUpdate ( );

    if ( bDrawOpenCorpusWarning && ofGetElapsedTimeMillis ( ) - mOpenCorpusButtonClickTime > mOpenCorpusButtonTimeout )
    {
        bDrawOpenCorpusWarning = false;
        mOpenCorpusButton.setName ( "Open Corpus" );
    }
}

void ExplorerMenu::Exit ( )
{
    RemoveListenersHeader ( );
    RemoveListenersCorpusControls ( );
    RemoveListenersAudioManager ( );
    mLiveView.Exit ( );
    mMIDI.Exit ( );
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

void ExplorerMenu::OpenFullPanel ( const Utilities::ExploreSettings& settings )
{
    mAudioSettingsManager.RefreshDeviceListChanged ( );

    RemoveListeners ( );

    mMainPanel.clear ( );

    mMainPanel.setup ( );

    SetupPanelSectionHeader ( mRawView->GetCorpusName ( ) );

    SetupPanelSectionCorpusControls ( settings );

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

void ExplorerMenu::SetupPanelSectionCorpusControls ( const Utilities::ExploreSettings& settings )
{
    // X Dimension Dropdown
    mDimensionDropdownX.reset ( );
    mDimensionDropdownX = make_unique<ofxDropdown> ( static_cast<std::string>("X Dimension"), Utilities::ofxDropdownScrollSpeed );
    mDimensionDropdownX->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownX->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownX.get ( ) );
    mDimensionDropdownX->disableMultipleSelection ( );
    mDimensionDropdownX->enableCollapseOnSelection ( );
    mDimensionDropdownX->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownX->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownX->setSelectedValueByName ( settings.GetDimensionX ( ), false );

    // Y Dimension Dropdown
    mDimensionDropdownY.reset ( );
    mDimensionDropdownY = make_unique<ofxDropdown> ( static_cast<std::string>("Y Dimension"), Utilities::ofxDropdownScrollSpeed );
    mDimensionDropdownY->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownY->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownY.get ( ) );
    mDimensionDropdownY->disableMultipleSelection ( );
    mDimensionDropdownY->enableCollapseOnSelection ( );
    mDimensionDropdownY->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownY->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownY->setSelectedValueByName ( settings.GetDimensionY ( ), false );

    // Z Dimension Dropdown
    mDimensionDropdownZ.reset ( );
    mDimensionDropdownZ = make_unique<ofxDropdown> ( static_cast<std::string>("Z Dimension"), Utilities::ofxDropdownScrollSpeed );
    mDimensionDropdownZ->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownZ->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownZ.get ( ) );
    mDimensionDropdownZ->disableMultipleSelection ( );
    mDimensionDropdownZ->enableCollapseOnSelection ( );
    mDimensionDropdownZ->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownZ->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownZ->setSelectedValueByName ( settings.GetDimensionZ ( ), false );

    // TODO - move color settings to the top of this panel section

    // Colour Dimension Dropdown
    mDimensionDropdownColor.reset ( );
    mDimensionDropdownColor = make_unique<ofxDropdown> ( static_cast<std::string>("Color Dimension"), Utilities::ofxDropdownScrollSpeed );
    mDimensionDropdownColor->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownColor->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownColor.get ( ) );
    mDimensionDropdownColor->disableMultipleSelection ( );
    mDimensionDropdownColor->enableCollapseOnSelection ( );
    mDimensionDropdownColor->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownColor->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownColor->setSelectedValueByName ( settings.GetDimensionColor ( ), false );

    // Color Spectrum Toggle
    mMainPanel.add ( mColorSpectrumSwitcher.setup ( "Color Spectrum: Red<->Blue", settings.GetColorSpectrum ( ) ) );
    mColorSpectrumSwitcher.setBackgroundColor ( mColors.interfaceBackgroundColor );

    //

    // EOF Loop Playheads Toggle
    mMainPanel.add ( mLoopPlayheadsToggle.setup ( "Loop when reaching end of a file", settings.GetLoopPlayheads ( ) ) );
    mLoopPlayheadsToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Playheads Jump Same File Toggle
    mMainPanel.add ( mJumpSameFileAllowedToggle.setup ( "Jump to same file allowed", settings.GetJumpSameFileAllowed ( ) ) );
    mJumpSameFileAllowedToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Playheads Same File Jump Minimum Distance Slider
    mMainPanel.add ( mJumpSameFileMinTimeDiffSlider.setup ( "Same file jump min point difference", settings.GetJumpSameFileMinTimeDiff ( ), 1, 30 ) );
    mJumpSameFileMinTimeDiffSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Jump Chance Slider
    mMainPanel.add ( mCrossoverJumpChanceSliderX1000.setup ( "Crossover Jump Chance", settings.GetCrossoverJumpChanceX1000 ( ), 0, 1000 ) );
    mCrossoverJumpChanceSliderX1000.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Crossfade Max Length Slider
    mMainPanel.add ( mCrossfadeSampleLengthSlider.setup ( "Crossfade Sample Length", settings.GetCrossfadeSampleLengthLimitedByHopSize ( ), 1, settings.GetHopSize ( ) ) );
    mCrossfadeSampleLengthSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Jump Max Distance Slider
    mMainPanel.add ( mMaxJumpDistanceSpaceSlider.setup ( "Max Jump Distance Space", settings.GetMaxJumpDistanceSpace ( ), 0.0, 1.0 ) );
    mMaxJumpDistanceSpaceSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Jump Max Targets Slider
    mMainPanel.add ( mMaxJumpTargetsSlider.setup ( "Max Jump Targets", settings.GetMaxJumpTargets ( ), 1, 10 ) );
    mMaxJumpTargetsSlider.setBackgroundColor ( mColors.interfaceBackgroundColor );
    
    // 
    
    // Global Volume Slider
    mMainPanel.add ( mVolumeSliderX1000.setup ( "Volume", settings.GetVolumeX1000 ( ), 0, 1000 ) );
    mVolumeSliderX1000.setBackgroundColor ( mColors.interfaceBackgroundColor );

    // Dynamic Panning Dimension Dropdown
    mDimensionDropdownDynamicPan.reset ( );
    mDimensionDropdownDynamicPan = make_unique<ofxDropdown> ( static_cast<std::string>("Dynamic Panning Dimension"), Utilities::ofxDropdownScrollSpeed );
    mDimensionDropdownDynamicPan->add ( "None" );
    for ( auto& dimension : mRawView->GetDimensions ( ) ) { mDimensionDropdownDynamicPan->add ( dimension ); }
    mMainPanel.add ( mDimensionDropdownDynamicPan.get ( ) );
    mDimensionDropdownDynamicPan->disableMultipleSelection ( );
    mDimensionDropdownDynamicPan->enableCollapseOnSelection ( );
    mDimensionDropdownDynamicPan->setDropDownPosition ( ofxDropdown::DD_LEFT );
    mDimensionDropdownDynamicPan->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mDimensionDropdownDynamicPan->setSelectedValueByName ( settings.GetDimensionDynamicPan ( ), false );

    // Global Panning Strength Slider
    mMainPanel.add ( mPanningStrengthSliderX1000.setup ( "Panning Width", settings.GetPanningStrengthX1000 ( ), 0, 1000 ) );
    mPanningStrengthSliderX1000.setBackgroundColor ( mColors.interfaceBackgroundColor );
}

void ExplorerMenu::SetupPanelSectionAudioManager ( )
{
    size_t apiIndex = mAudioSettingsManager.GetCurrentApiIndex ( );
    size_t outDeviceIndex = mAudioSettingsManager.GetCurrentDeviceIndex ( );
    int bufferSize = mAudioSettingsManager.GetCurrentBufferSize ( );

    mApiDropdown.reset ( );
    mApiDropdown = make_unique<ofxDropdown> ( (string)"Audio API", Utilities::ofxDropdownScrollSpeed );
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
    mOutDeviceDropdown = make_unique<ofxDropdown> ( (string)"Output Device", Utilities::ofxDropdownScrollSpeed );
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
    mBufferSizeDropdown = make_unique<ofxIntDropdown> ( (string)"Buffer Size", Utilities::ofxDropdownScrollSpeed );
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

    for ( size_t i = 0; i < mLiveView.GetPlayheads ( ).size ( ); i++ ) // TODO - fix playhead visual stacking when window resizing bug, easy fix
    {
        mLiveView.GetPlayheads ( )[i].ResizeBox ( i, mLayout->getTopBarHeight ( ), ofGetHeight ( ), ofGetWidth ( ) );
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
    mCrossoverJumpChanceSliderX1000.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mCrossfadeSampleLengthSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mMaxJumpDistanceSpaceSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mMaxJumpTargetsSlider.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    
    mVolumeSliderX1000.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );
    mDimensionDropdownDynamicPan->setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelDropdownRowHeight ( ) );
    mPanningStrengthSliderX1000.setSize ( mLayout->getExplorePanelWidth ( ), mLayout->getPanelRowHeight ( ) );

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

    mDimensionDropdownX->addListener ( this, &ExplorerMenu::SetDimensionXListener );
    mDimensionDropdownY->addListener ( this, &ExplorerMenu::SetDimensionYListener );
    mDimensionDropdownZ->addListener ( this, &ExplorerMenu::SetDimensionZListener );

    mDimensionDropdownColor->addListener ( this, &ExplorerMenu::SetDimensionColorListener );
    mColorSpectrumSwitcher.addListener ( this, &ExplorerMenu::SwitchColorSpectrumListener );

    mLoopPlayheadsToggle.addListener ( this, &ExplorerMenu::ToggleLoopPlayheadsListener );
    mJumpSameFileAllowedToggle.addListener ( this, &ExplorerMenu::ToggleJumpSameFileAllowedListener );
    mJumpSameFileMinTimeDiffSlider.addListener ( this, &ExplorerMenu::SetJumpSameFileMinTimeDiffListener );
    mCrossoverJumpChanceSliderX1000.addListener ( this, &ExplorerMenu::SetCrossoverJumpChanceX1000Listener );
    mCrossfadeSampleLengthSlider.addListener ( this, &ExplorerMenu::SetCrossfadeSampleLengthListener );
    mMaxJumpDistanceSpaceSlider.addListener ( this, &ExplorerMenu::SetMaxJumpDistanceSpaceListener );
    mMaxJumpTargetsSlider.addListener ( this, &ExplorerMenu::SetMaxJumpTargetsListener );

    mVolumeSliderX1000.addListener ( this, &ExplorerMenu::SetVolumeX1000Listener );

    mDimensionDropdownDynamicPan->addListener ( this, &ExplorerMenu::SetDimensionDynamicPanListener );
    mPanningStrengthSliderX1000.addListener ( this, &ExplorerMenu::SetPanningStrengthX1000Listener );

    ofAddListener ( ofEvents ( ).mouseReleased, this, &ExplorerMenu::MouseReleased );

    bListenersAddedCorpusControls = true;
}

void ExplorerMenu::RemoveListenersCorpusControls ( )
{
    if ( !bListenersAddedCorpusControls ) { return; }

    mDimensionDropdownX->removeListener ( this, &ExplorerMenu::SetDimensionXListener );
    mDimensionDropdownY->removeListener ( this, &ExplorerMenu::SetDimensionYListener );
    mDimensionDropdownZ->removeListener ( this, &ExplorerMenu::SetDimensionZListener );

    mDimensionDropdownColor->removeListener ( this, &ExplorerMenu::SetDimensionColorListener );
    mColorSpectrumSwitcher.removeListener ( this, &ExplorerMenu::SwitchColorSpectrumListener );

    mLoopPlayheadsToggle.removeListener ( this, &ExplorerMenu::ToggleLoopPlayheadsListener );
    mJumpSameFileAllowedToggle.removeListener ( this, &ExplorerMenu::ToggleJumpSameFileAllowedListener );
    mJumpSameFileMinTimeDiffSlider.removeListener ( this, &ExplorerMenu::SetJumpSameFileMinTimeDiffListener );
    mCrossoverJumpChanceSliderX1000.removeListener ( this, &ExplorerMenu::SetCrossoverJumpChanceX1000Listener );
    mCrossfadeSampleLengthSlider.removeListener ( this, &ExplorerMenu::SetCrossfadeSampleLengthListener );
    mMaxJumpDistanceSpaceSlider.removeListener ( this, &ExplorerMenu::SetMaxJumpDistanceSpaceListener );
    mMaxJumpTargetsSlider.removeListener ( this, &ExplorerMenu::SetMaxJumpTargetsListener );

    mVolumeSliderX1000.removeListener ( this, &ExplorerMenu::SetVolumeX1000Listener );

    mDimensionDropdownDynamicPan->removeListener ( this, &ExplorerMenu::SetDimensionDynamicPanListener );
    mPanningStrengthSliderX1000.removeListener ( this, &ExplorerMenu::SetPanningStrengthX1000Listener );

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
    
    if ( bIsCorpusOpen && !bDrawOpenCorpusWarning )
    {
        bDrawOpenCorpusWarning = true;
        mOpenCorpusButtonClickTime = ofGetElapsedTimeMillis ( );
        mOpenCorpusButton.setName ( "!! Close Current? !!" );
        return;
    }
    bDrawOpenCorpusWarning = false;

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

    if ( mRawView->GetDimensions ( ).size ( ) < 2 )
    {
        Initialise ( );
        ofLogError ( "Explorer" ) << "Corpus must have at least 2 dimensions for exploration.";
        return;
    }

    Utilities::ExploreSettings initialSettings { };
    {
        initialSettings.SetHopSize ( mRawView->GetHopSize ( ) );

        initialSettings.SetDimensionX ( mRawView->GetDimensions ( ).size ( ) > 1 ? mRawView->GetDimensions ( )[1] : DEFAULT_DIMENSION_X );
        initialSettings.SetDimensionY ( mRawView->GetDimensions ( ).size ( ) > 2 ? mRawView->GetDimensions ( )[2] : DEFAULT_DIMENSION_Y );
        initialSettings.SetDimensionZ ( mRawView->GetDimensions ( ).size ( ) > 3 ? mRawView->GetDimensions ( )[3] : DEFAULT_DIMENSION_Z );

        initialSettings.SetDimensionColor ( DEFAULT_DIMENSION_COLOR );
        initialSettings.SetColorSpectrum ( DEFAULT_COLOR_SPECTRUM );

        initialSettings.SetLoopPlayheads ( DEFAULT_LOOP_PLAYHEADS );
        initialSettings.SetJumpSameFileAllowed ( DEFAULT_JUMP_SAME_FILE_ALLOWED );
        initialSettings.SetJumpSameFileMinTimeDiff ( DEFAULT_JUMP_SAME_FILE_MIN_DIFF );
        initialSettings.SetCrossoverJumpChanceX1000 ( DEFAULT_CROSSOVER_JUMP_CHANCE_X1000 );
        initialSettings.SetCrossfadeSampleLength ( DEFAULT_CROSSFADE_SAMPLE_LENGTH );
        initialSettings.SetMaxJumpDistanceSpaceX1000 ( DEFAULT_MAX_JUMP_DISTANCE_SPACE_X1000 );
        initialSettings.SetMaxJumpTargets ( DEFAULT_MAX_JUMP_TARGETS );

        initialSettings.SetVolumeX1000 ( DEFAULT_VOLUME_X1000 );
        initialSettings.SetDimensionDynamicPan ( DEFAULT_DIMENSION_DYNAMIC_PAN );
        initialSettings.SetPanningStrengthX1000 ( DEFAULT_PANNING_STRENGTH_X1000 );
    }

    mLiveView.Initialise ( );

    mLiveView.CreatePoints ( ); // TODO - combine with mLiveView.Initialise ( );?

    OpenFullPanel ( initialSettings );

    bBlockDimensionFilling = false;

    PropogateCorpusSettings ( initialSettings );

    CameraSwitcher ( );

    bIsCorpusOpen = true;

    bool audioStarted = mLiveView.StartAudio ( mAudioSettingsManager.GetCurrentAudioSettings ( ) );

    ofLogNotice ( "Explorer" ) << "Opened corpus: " << mRawView->GetCorpusName ( );
    ofLogNotice ( "Explorer" ) << mRawView->GetLoadedFileCount ( ) << "/" << mRawView->GetFileCount ( ) << " audio files loaded successfully.";

    if ( !audioStarted ) { AudioOutputFailed ( ); }
}

// TODO - change how this and FillDimension and Train (point picker) work, should have a function that triggers training and one that doesn't (for initial filling)
void ExplorerMenu::SetDimension ( string dimension, Utilities::Axis axis )
{
    if ( bBlockDimensionFilling ) { return; }

    if ( axis == Utilities::Axis::DYNAMIC_PAN )
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

    if ( dimension == "None" )					{ mLiveView.ClearDimension ( axis ); }
    else
    {
        int dimensionIndex = GetDimensionIndex ( dimension );
        if ( dimensionIndex == -1 ) { return; }

        mLiveView.FillDimension ( dimensionIndex, axis );
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
    ofLogError ( "Explorer" ) << "Dimension " << dimension << " name not found";
    return -1;
}

void ExplorerMenu::CameraSwitcher ( )
{
    bool isXNone = mDimensionDropdownX->getAllSelected ( )[0] == "None";
    bool isYNone = mDimensionDropdownY->getAllSelected ( )[0] == "None";
    bool isZNone = mDimensionDropdownZ->getAllSelected ( )[0] == "None";
    int numDisabledAxes = isXNone + isYNone + isZNone;

    Utilities::Axis							  disabledAxis = Utilities::Axis::NONE;
    if		( isXNone )					{ disabledAxis = Utilities::Axis::X; }
    else if ( isYNone )					{ disabledAxis = Utilities::Axis::Y; }
    else if ( isZNone )					{ disabledAxis = Utilities::Axis::Z; }
    else if ( numDisabledAxes > 1 )		{ disabledAxis = Utilities::Axis::MULTIPLE; }

    bool current3D = mLiveView.Is3D ( );

    if ( disabledAxis == Utilities::Axis::NONE || disabledAxis == Utilities::Axis::MULTIPLE )
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

void ExplorerMenu::PropogateCorpusSettings ( const Utilities::ExploreSettings& settings )
{
    // TODO - change these 3 so that only the final call of the 3 triggers point picker Train ( )
    SetDimensionX ( settings.GetDimensionX ( ) );
    SetDimensionY ( settings.GetDimensionY ( ) );
    SetDimensionZ ( settings.GetDimensionZ ( ) ); // the one that actually calls training stuff

    SetDimensionColor ( settings.GetDimensionColor ( ) );
    SwitchColorSpectrum ( settings.GetColorSpectrum ( ) );

    ToggleLoopPlayheads ( settings.GetLoopPlayheads ( ) );
    ToggleJumpSameFileAllowed ( settings.GetJumpSameFileAllowed ( ) );
    SetJumpSameFileMinTimeDiff ( settings.GetJumpSameFileMinTimeDiff ( ) );
    SetCrossoverJumpChanceX1000 ( settings.GetCrossoverJumpChanceX1000 ( ) );
    SetCrossfadeSampleLength ( settings.GetCrossfadeSampleLengthLimitedByHopSize ( ) );
    SetMaxJumpDistanceSpace ( settings.GetMaxJumpDistanceSpace ( ) );
    SetMaxJumpTargets ( settings.GetMaxJumpTargets ( ) );

    SetVolumeX1000 ( settings.GetVolumeX1000 ( ) );
    SetDimensionDynamicPan ( settings.GetDimensionDynamicPan ( ) );
    SetPanningStrengthX1000 ( settings.GetPanningStrengthX1000 ( ) );
}

// Listener Functions --------------------------
    // Corpus Controls
void ExplorerMenu::SetDimensionX ( const string& dimension )
{
    SetDimension ( dimension, Utilities::Axis::X );
}

void ExplorerMenu::SetDimensionY ( const string& dimension )
{
    SetDimension ( dimension, Utilities::Axis::Y );
}

void ExplorerMenu::SetDimensionZ ( const string& dimension )
{
    SetDimension ( dimension, Utilities::Axis::Z );
}

void ExplorerMenu::SetDimensionColor ( const string& dimension )
{
    SetDimension ( dimension, Utilities::Axis::COLOR );
}

void ExplorerMenu::SwitchColorSpectrum ( const bool& fullSpectrum )
{
    if ( fullSpectrum ) { mColorSpectrumSwitcher.setName ( "Color Spectrum: Full" ); }
    else { mColorSpectrumSwitcher.setName ( "Color Spectrum: Red<->Blue" ); }
    mLiveView.SetColorFullSpectrum ( fullSpectrum );
    SetDimension ( mDimensionDropdownColor->getAllSelected ( )[0], Utilities::Axis::COLOR );
}

void ExplorerMenu::ToggleLoopPlayheads ( const bool& loop )
{
    mLiveView.GetAudioPlayback ( )->SetLoopPlayheads ( loop );
}

void ExplorerMenu::ToggleJumpSameFileAllowed ( const bool& allowed )
{
    mLiveView.GetAudioPlayback ( )->SetJumpSameFileAllowed ( allowed );
}

void ExplorerMenu::SetJumpSameFileMinTimeDiff ( const int& timeDiff )
{
    mLiveView.GetAudioPlayback ( )->SetJumpSameFileMinTimeDiff ( timeDiff );
}

void ExplorerMenu::SetCrossoverJumpChanceX1000 ( const int& jumpChanceX1000 )
{
    mLiveView.GetAudioPlayback ( )->SetCrossoverJumpChanceX1000 ( jumpChanceX1000 );
}

void ExplorerMenu::SetCrossfadeSampleLength ( const int& length )
{
    mLiveView.GetAudioPlayback ( )->SetCrossfadeSampleLength ( length );
}

void ExplorerMenu::SetMaxJumpDistanceSpace ( const float& distance )
{
    mLiveView.GetAudioPlayback ( )->SetMaxJumpDistanceSpace ( (int)(distance * 1000) );
}

void ExplorerMenu::SetMaxJumpTargets ( const int& targets )
{
    mLiveView.GetAudioPlayback ( )->SetMaxJumpTargets ( targets );
}

void ExplorerMenu::SetVolumeX1000 ( const int& volumeX1000 )
{
    mLiveView.GetAudioPlayback ( )->SetVolumeX1000 ( volumeX1000 );
}

void ExplorerMenu::SetDimensionDynamicPan ( const string& dimension )
{
    SetDimension ( dimension, Utilities::Axis::DYNAMIC_PAN );
}

void ExplorerMenu::SetPanningStrengthX1000 ( const int& strengthX1000 )
{
    mLiveView.GetAudioPlayback ( )->SetPanningStrengthX1000 ( strengthX1000 );
}

void ExplorerMenu::MouseReleased ( ofMouseEventArgs& args )
{
    for ( auto& playhead : mLiveView.GetPlayheads ( ) )
    {
        if ( playhead.killButtonRect.inside ( args.x, args.y ) )
        {
            mLiveView.GetAudioPlayback ( )->KillPlayhead ( playhead.playheadID );
            return;
        }
    }
}
    // Audio Manager
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
        ofLogError ( "Explorer" ) << "Failed to change audio API to selected API."
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
        ofLogError ( "Explorer" ) << "Failed to change output device to selected device.";
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

    ofLogError ( "Explorer" ) << "Audio output failed to restart with current settings. This likely means the selected output device is currently unavailable. Please check your audio output device and try again.";
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