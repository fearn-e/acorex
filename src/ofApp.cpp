#include "ofApp.h"

void ofApp::setup ( )
{
	int topBarButtonWidth = 100;
	if ( GetDpiForSystem ( ) > 119 )
	{
		ofxGuiEnableHiResDisplay ( );
		mLayout.enableHiDpi ( );
		topBarButtonWidth *= 2;
	}
	else
	{
		mLayout.disableHiDpi ( );
	}

	ofSetWindowTitle ( "ACorEx" );
	
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	ofSetWindowShape ( ofGetScreenWidth ( ) * 0.75, ofGetScreenHeight ( ) * 0.75 );
	ofSetWindowPosition ( ofGetScreenWidth ( ) / 2 - ofGetWidth ( ) / 2, ofGetScreenHeight ( ) / 2 - ofGetHeight ( ) / 2 );

	mAnalyserMenu.Initialise ( );
	mExplorerMenu.Initialise ( );

	mAnalyseToggle.setup ( "Analyse", false, topBarButtonWidth, mLayout.topBarHeight / 2 );
	mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout.topBarHeight / 4 );
	mAnalyseToggle.setBackgroundColor ( mColors.transparent );

	mExploreToggle.setup ( "Explore", false, topBarButtonWidth, mLayout.topBarHeight / 2 );
	mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout.topBarHeight / 4 );
	mExploreToggle.setBackgroundColor ( mColors.transparent );
	
	mAnalyseToggle.addListener ( this, &ofApp::AnalyseToggled );
	mExploreToggle.addListener ( this, &ofApp::ExploreToggled );
}

void ofApp::update ( )
{
	mExplorerMenu.Update ( );
}

void ofApp::draw ( )
{
	mExplorerMenu.Draw ( );
	mAnalyserMenu.Draw ( );

	{
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle ( 0, 0, ofGetWidth ( ), mLayout.topBarHeight );

		mAnalyseToggle.draw ( );
		mExploreToggle.draw ( );
	}

	ofDrawBitmapStringHighlight ( "fps: " + ofToString ( ofGetFrameRate ( ) ), 20, ofGetHeight ( ) - 20 );
}

void ofApp::exit ( )
{
	mAnalyserMenu.Exit ( );
}

void ofApp::windowResized ( int w, int h )
{
	mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout.topBarHeight / 4 );
	mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout.topBarHeight / 4 );

	mExplorerMenu.WindowResized ( );
}

void ofApp::AnalyseToggled ( bool& value )
{
	if ( value )
	{
		mAnalyserMenu.Show ( );
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
	}
	else
	{
		mExplorerMenu.Hide ( );
	}
}