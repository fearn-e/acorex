#include "ofApp.h"

void ofApp::setup ( )
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

void ofApp::update ( )
{
	if ( mExploreToggle ) { mExplorerMenu.Update ( ); }
}

void ofApp::draw ( )
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

	if ( mExploreToggle )
		ofDrawBitmapStringHighlight ( "fps: " + ofToString ( (int)ofGetFrameRate ( ) ), 0, mLayout.topBarHeight + 10 );
	else
		ofDrawBitmapStringHighlight ( "fps: " + ofToString ( (int)ofGetFrameRate ( ) ), ofGetWidth ( ) - 70, mLayout.topBarHeight + 10 );
}

void ofApp::exit ( )
{
	mAnalyserMenu.Exit ( );
	mExplorerMenu.Exit ( );
}

void ofApp::windowResized ( int w, int h )
{
	mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout.topBarHeight / 4 );
	mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout.topBarHeight / 4 );

	mExplorerMenu.WindowResized ( );
}

void ofApp::RemoveListeners ( )
{
	if ( bListenersAdded )
	{
		mAnalyseToggle.removeListener ( this, &ofApp::AnalyseToggled );
		mExploreToggle.removeListener ( this, &ofApp::ExploreToggled );
		mDPIToggle.removeListener ( this, &ofApp::DPIToggled );
		bListenersAdded = false;
	}
}

void ofApp::SetupUI ( bool keepValues )
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

	mAnalyseToggle.addListener ( this, &ofApp::AnalyseToggled );
	mExploreToggle.addListener ( this, &ofApp::ExploreToggled );
	mDPIToggle.addListener ( this, &ofApp::DPIToggled );
	bListenersAdded = true;
}

void ofApp::AnalyseToggled ( bool& value )
{
	if ( value )
	{
		mAnalyserMenu.Show ( );
		if ( mExploreToggle ) { mExplorerMenu.Hide ( ); }
	}
	else
	{
		mAnalyserMenu.Hide ( );
	}
}

void ofApp::ExploreToggled ( bool& value )
{
	if ( value )
	{
		mExplorerMenu.Show ( );
		if ( mAnalyseToggle ) { mAnalyserMenu.Hide ( ); }
	}
	else
	{
		mExplorerMenu.Hide ( );
	}
}

void ofApp::DPIToggled ( bool& value )
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