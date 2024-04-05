#include "ofApp.h"

void ofApp::setup ( )
{
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	mControllerMenu.Show ( );
}

void ofApp::update ( )
{
}

void ofApp::draw ( )
{
	mControllerMenu.Draw ( );
}

void ofApp::exit ( )
{
	mControllerMenu.Exit ( );
}