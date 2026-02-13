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

#include "ofApp.h"

#include "Utils/TemporaryDefaults.h"

ofApp::ofApp ( ) : bListenersAdded ( false ) { }

void ofApp::setup ( )
{   
    mLayout = std::make_shared<Acorex::Utils::MenuLayout> ( );
    mAnalyserMenu.SetMenuLayout ( mLayout );
    mExplorerMenu.SetMenuLayout ( mLayout );

    ofSetWindowTitle ( "ACorEx" );

    ofSetVerticalSync ( true );
    ofBackground ( 30 );

    ofSetWindowShape ( ofGetScreenWidth ( ) * 0.75, ofGetScreenHeight ( ) * 0.75 );
    ofSetWindowPosition ( ofGetScreenWidth ( ) / 2 - ofGetWidth ( ) / 2, ofGetScreenHeight ( ) / 2 - ofGetHeight ( ) / 2 );

    mLayout->toggleHiDpi ( DEFAULT_HI_DPI );
    if ( DEFAULT_HI_DPI ) { ofxGuiEnableHiResDisplay ( ); }
    else { ofxGuiDisableHiResDisplay ( ); }

    InitialiseUI ( );

    mAnalyserMenu.Initialise ( );

    // opens startup panel
    mExplorerMenu.Initialise ( );
}

void ofApp::update ( )
{
    mExplorerMenu.Update ( );
}

void ofApp::draw ( )
{
    mAnalyserMenu.Draw ( );
    mExplorerMenu.Draw ( );

    {
        ofSetColor ( mColors.interfaceBackgroundColor );
        ofDrawRectangle ( 0, 0, ofGetWidth ( ), mLayout->getTopBarHeight ( ) );

        mAnalyseToggle.draw ( );
        mExploreToggle.draw ( );
        mDPIToggle.draw ( );
    }

    ofDrawBitmapStringHighlight ( "fps: " + ofToString ( (int)ofGetFrameRate ( ) ), 0, 10 );
}

void ofApp::exit ( )
{
    mAnalyserMenu.Exit ( );
    mExplorerMenu.Exit ( );
}

void ofApp::windowResized ( int w, int h )
{
    mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout->getTopBarHeight ( ) / 4 );
    mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout->getTopBarHeight ( ) / 4 );
    mDPIToggle.setPosition ( ofGetWidth ( ) - mLayout->getTopBarButtonWidth ( ) - 5, mLayout->getTopBarHeight ( ) / 4 );

    mExplorerMenu.WindowResized ( );
}

void ofApp::AddListeners ( )
{
    if ( bListenersAdded ) { return; }

    mAnalyseToggle.addListener ( this, &ofApp::AnalyseToggled );
    mExploreToggle.addListener ( this, &ofApp::ExploreToggled );
    mDPIToggle.addListener ( this, &ofApp::DPIToggled );
    bListenersAdded = true;
}

void ofApp::RemoveListeners ( )
{
    if ( !bListenersAdded ) { return; }

    mAnalyseToggle.removeListener ( this, &ofApp::AnalyseToggled );
    mExploreToggle.removeListener ( this, &ofApp::ExploreToggled );
    mDPIToggle.removeListener ( this, &ofApp::DPIToggled );
    bListenersAdded = false;
}

void ofApp::InitialiseUI ( )
{
    RemoveListeners ( );

    mAnalyseToggle.setup ( "Analyse", DEFAULT_ANALYSE_OPEN, mLayout->getTopBarButtonWidth ( ), mLayout->getTopBarHeight ( ) / 2 );
    mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout->getTopBarHeight ( ) / 4 );
    mAnalyseToggle.setBackgroundColor ( mColors.transparent );

    mExploreToggle.setup ( "Explore", DEFAULT_EXPLORE_OPEN, mLayout->getTopBarButtonWidth ( ), mLayout->getTopBarHeight ( ) / 2 );
    mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout->getTopBarHeight ( ) / 4 );
    mExploreToggle.setBackgroundColor ( mColors.transparent );

    mDPIToggle.setup ( "Bigger UI", DEFAULT_HI_DPI, mLayout->getTopBarButtonWidth ( ), mLayout->getTopBarHeight ( ) / 2 );
    mDPIToggle.setPosition ( ofGetWidth ( ) - mLayout->getTopBarButtonWidth ( ) - 5, mLayout->getTopBarHeight ( ) / 4 );
    mDPIToggle.setBackgroundColor ( mColors.transparent );

    AddListeners ( );
}

void ofApp::RefreshUI ( )
{
    mAnalyseToggle.setSize ( mLayout->getTopBarButtonWidth ( ), mLayout->getTopBarHeight ( ) / 2 );
    mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout->getTopBarHeight ( ) / 4 );
    mAnalyseToggle.sizeChangedCB ( );

    mExploreToggle.setSize ( mLayout->getTopBarButtonWidth ( ), mLayout->getTopBarHeight ( ) / 2 );
    mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout->getTopBarHeight ( ) / 4 );
    mExploreToggle.sizeChangedCB ( );

    mDPIToggle.setSize ( mLayout->getTopBarButtonWidth ( ), mLayout->getTopBarHeight ( ) / 2 );
    mDPIToggle.setPosition ( ofGetWidth ( ) - mLayout->getTopBarButtonWidth ( ) - 5, mLayout->getTopBarHeight ( ) / 4 );
    mDPIToggle.sizeChangedCB ( );
}

void ofApp::AnalyseToggled ( bool& value )
{
    if ( value )
    {
        mAnalyserMenu.Open ( );
        mExploreToggle = false;
    }
    else
    {
        mAnalyserMenu.Close ( );
    }
}

void ofApp::ExploreToggled ( bool& value )
{
    if ( value )
    {
        mExplorerMenu.Open ( );
        mAnalyseToggle = false;
    }
    else
    {
        mExplorerMenu.Close ( );
    }
}

void ofApp::DPIToggled ( bool& value )
{
    mLayout->toggleHiDpi ( value );
    if ( value ) { ofxGuiEnableHiResDisplay ( ); }
    else { ofxGuiDisableHiResDisplay ( ); }

    RefreshUI ( );
    mExplorerMenu.RefreshUI ( );
    mAnalyserMenu.RefreshUI ( );
}