#include "ofApp.h"

ofApp::ofApp ( )
{
}

ofApp::~ofApp ( )
{
}

void ofApp::setup ( )
{
	ofSetVerticalSync ( true );
	ofBackground ( 30 );
	mStartAnalysis.addListener ( this, &ofApp::StartAnalysis );
	mStartUMAP.addListener ( this, &ofApp::StartUMAP );
	mGui.setup ( "Analysis" );
	mGui.add ( mStartAnalysis.setup ( "Start Analysis" ) );
	mGui.add ( mStartUMAP.setup ( "Start UMAP" ) );
	mGui.setPosition ( 10, 10 );
}

void ofApp::update ( )
{
}

void ofApp::draw ( )
{
	mGui.draw ( );
}

void ofApp::exit ( )
{
	mStartAnalysis.removeListener ( this, &ofApp::StartAnalysis );
}

void ofApp::StartAnalysis ( )
{
	ofFileDialogResult result = ofSystemLoadDialog ( "Select folder containing audio files", true, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !result.bSuccess )
	{
		ofLogError ( "ofApp::startAnalysis" ) << "No folder selected";
		return;
	}

	std::string inPath = result.getPath ( );
	std::vector<std::string> files;
	
	ofFile temp = ofFile ( result.getPath ( ) );
	std::string outPath = temp.getEnclosingDirectory ( ) + "analysis_output.json";

	files = mAnalyseCorpus.SearchDirectory ( inPath );

	mAnalyseCorpus.Analyse ( files, outPath );
}

void ofApp::StartUMAP ( )
{
	ofFileDialogResult result = ofSystemLoadDialog ( "Select analysis file", false, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !result.bSuccess )
	{
		ofLogError ( "ofApp::startUMAP" ) << "No file selected";
		return;
	}

	if ( result.getPath ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "ofApp::startUMAP" ) << "File is not a json file";
		return;
	}

	std::string inPath = result.getPath ( );
	ofFile temp = ofFile ( result.getPath ( ) );
	std::string outPath = temp.getEnclosingDirectory ( ) + "umap_output.json";

	mUMAP.Fit ( inPath, outPath );
}