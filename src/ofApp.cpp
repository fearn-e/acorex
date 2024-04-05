#include "ofApp.h"

void ofApp::setup ( )
{
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	mAnalyseToggle.setup ( "Analyse", false, 100, mLayout.topBarHeight / 2 );
	mAnalyseToggle.setPosition ( ofGetWidth ( ) / 2 - 5 - mAnalyseToggle.getWidth ( ), mLayout.topBarHeight / 4 );
	mAnalyseToggle.setBackgroundColor ( mColors.transparent );

	mExploreToggle.setup ( "Explore", false, 100, mLayout.topBarHeight / 2 );
	mExploreToggle.setPosition ( ofGetWidth ( ) / 2 + 5, mLayout.topBarHeight / 4 );
	mExploreToggle.setBackgroundColor ( mColors.transparent );
	
	mAnalyseToggle.addListener ( this, &ofApp::AnalyseToggled );
	mExploreToggle.addListener ( this, &ofApp::ExploreToggled );
}

void ofApp::update ( )
{
}

void ofApp::draw ( )
{
	if ( mAnalyseToggle )
	{
		mControllerMenu.Draw ( );
	}
	
	if ( mExploreToggle )
	{
		//draw explore menu
	}

	{
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle ( 0, 0, ofGetWidth ( ), mLayout.topBarHeight );

		mAnalyseToggle.draw ( );
		mExploreToggle.draw ( );
	}
}

void ofApp::exit ( )
{
	mControllerMenu.Exit ( );
}

void ofApp::AnalyseToggled ( bool& value )
{
	if ( value )
	{
		mControllerMenu.Show ( );
	}
	else
	{
		mControllerMenu.Reset ( );
	}
}

void ofApp::ExploreToggled ( bool& value )
{
	if ( value )
	{
		//show explore menu
	}
	else
	{
		//hide explore menu
	}
}