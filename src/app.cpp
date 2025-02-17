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

#include "app.h"

void app::setup ( )
{    
	ofSetWindowTitle ( "ACorEx" );
	
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	ofSetWindowShape ( ofGetScreenWidth ( ) * 0.75, ofGetScreenHeight ( ) * 0.75 );
	ofSetWindowPosition ( ofGetScreenWidth ( ) / 2 - ofGetWidth ( ) / 2, ofGetScreenHeight ( ) / 2 - ofGetHeight ( ) / 2 );

	ofxGuiDisableHiResDisplay ( );
	mLayout.disableHiDpi ( );

	SetupUI ( false );

	mAnalyserMenu.Initialise ( false );
	mExplorerMenu.Initialise ( false );
}

void app::update ( )
{
	if ( mExploreToggle ) { mExplorerMenu.Update ( ); }
}

void app::draw ( )
{
	if ( mAnalyseToggle ) { mAnalyserMenu.Draw ( ); }
	if ( mExploreToggle ) { mExplorerMenu.Draw ( ); }

	{
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle ( 0, 0, ofGetWidth ( ), mLayout.topBarHeight );

		mAnalyseToggle.draw ( );
		mExploreToggle.draw ( );
		mDPIToggle.draw ( );
	}

	ofDrawBitmapStringHighlight ( "fps: " + ofToString ( (int)ofGetFrameRate ( ) ), 0, 10 );
}

void app::exit ( )
{
	mAnalyserMenu.Exit ( );
	mExplorerMenu.Exit ( );
}

void app::windowResized ( int w, int h )
{
	mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout.topBarHeight / 4 );
	mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout.topBarHeight / 4 );
	mDPIToggle.setPosition ( ofGetWidth ( ) - mLayout.topBarButtonWidth - 5, mLayout.topBarHeight / 4 );

	mExplorerMenu.WindowResized ( );
}

void app::RemoveListeners ( )
{
	if ( bListenersAdded )
	{
		mAnalyseToggle.removeListener ( this, &app::AnalyseToggled );
		mExploreToggle.removeListener ( this, &app::ExploreToggled );
		mDPIToggle.removeListener ( this, &app::DPIToggled );
		bListenersAdded = false;
	}
}

void app::SetupUI ( bool keepValues )
{
	RemoveListeners ( );

	bool tmpAnalyse = keepValues ? mAnalyseToggle : false;
	mAnalyseToggle.setup ( "Analyse", tmpAnalyse, mLayout.topBarButtonWidth, mLayout.topBarHeight / 2 );
	mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout.topBarHeight / 4 );
	mAnalyseToggle.setBackgroundColor ( mColors.transparent );

	bool tmpExplore = keepValues ? mExploreToggle : false;
	mExploreToggle.setup ( "Explore", tmpExplore, mLayout.topBarButtonWidth, mLayout.topBarHeight / 2 );
	mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout.topBarHeight / 4 );
	mExploreToggle.setBackgroundColor ( mColors.transparent );

	bool tmpDPI = keepValues ? mDPIToggle : false;
	mDPIToggle.setup ( "Bigger UI", tmpDPI, mLayout.topBarButtonWidth, mLayout.topBarHeight / 2 );
	mDPIToggle.setPosition ( ofGetWidth ( ) - mLayout.topBarButtonWidth - 5, mLayout.topBarHeight / 4 );
	mDPIToggle.setBackgroundColor ( mColors.transparent );

	mAnalyseToggle.addListener ( this, &app::AnalyseToggled );
	mExploreToggle.addListener ( this, &app::ExploreToggled );
	mDPIToggle.addListener ( this, &app::DPIToggled );
	bListenersAdded = true;
}

void app::AnalyseToggled ( bool& value )
{
	if ( value )
	{
		mAnalyserMenu.Show ( );
		mExploreToggle = false;
	}
	else
	{
		mAnalyserMenu.Hide ( );
	}
}

void app::ExploreToggled ( bool& value )
{
	if ( value )
	{
		mExplorerMenu.Show ( );
		mAnalyseToggle = false;
	}
	else
	{
		mExplorerMenu.Hide ( );
	}
}

void app::DPIToggled ( bool& value )
{
	if ( value )
	{
		ofxGuiEnableHiResDisplay ( );
		mLayout.enableHiDpi ( );
	}
	else
	{
		ofxGuiDisableHiResDisplay ( );
		mLayout.disableHiDpi ( );
	}

	SetupUI ( true );

	mExplorerMenu.Initialise ( value );
	mAnalyserMenu.Initialise ( value );
	if ( mAnalyseToggle ) { mAnalyserMenu.Show ( ); }
}