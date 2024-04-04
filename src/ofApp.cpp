#include "ofApp.h"

void ofApp::setup ( )
{
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	mControllerMenu.setup ( );
}

void ofApp::update ( )
{
}

void ofApp::draw ( )
{
	mControllerMenu.draw ( );
}

void ofApp::exit ( )
{
	mControllerMenu.exit ( );
}