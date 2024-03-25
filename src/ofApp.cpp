#include "ofApp.h"

void ofApp::setup ( )
{
	ofSetVerticalSync ( true );
	ofBackground ( 30 );

	mControllerUI.setup ( );
}

void ofApp::update ( )
{
}

void ofApp::draw ( )
{
	mControllerUI.draw ( );
}

void ofApp::exit ( )
{
	mControllerUI.exit ( );
}

void ofApp::CreateCorpus ( )
{

}

void ofApp::ReduceCorpus ( )
{

}

void ofApp::InsertIntoCorpus ( )
{

}