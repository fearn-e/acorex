#include "ofApp.h"

void ofApp::setup ( )
{
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	mAnalyserMenu.Initialise ( );
	mExplorerMenu.Initialise ( );

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
	mExplorerMenu.Update ( );
}

void ofApp::draw ( )
{
	mAnalyserMenu.Draw ( );
	mExplorerMenu.Draw ( );

	{
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle ( 0, 0, ofGetWidth ( ), mLayout.topBarHeight );

		mAnalyseToggle.draw ( );
		mExploreToggle.draw ( );
	}
}

void ofApp::exit ( )
{
	mAnalyserMenu.Exit ( );
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