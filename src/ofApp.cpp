#include "ofApp.h"
#include "CorpusController.h"

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
	mGui.add ( mTimeDimension.setup ( "Time Dimension", false ) );
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
	bool success;

	ofFileDialogResult resultIn = ofSystemLoadDialog ( "Select folder containing audio files...", true, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !resultIn.bSuccess )
	{
		ofLogError ( "ofApp::startAnalysis" ) << "No folder selected";
		return;
	}

	ofFileDialogResult resultOut = ofSystemSaveDialog ( "analysed_corpus.json", "Saving corpus as..." );
	if ( !resultOut.bSuccess )
	{
		ofLogError ( "ofApp::startAnalysis" ) << "Invalid save query";
		return;
	}

	success = mCorpusController.CreateCorpus ( resultIn.getPath ( ), resultOut.getPath ( ), mTimeDimension );
	if ( !success )
	{
		ofLogError ( "ofApp" ) << "Corpus creation failed";
		return;
	}
}

void ofApp::StartUMAP ( )
{
	bool success;


	ofFileDialogResult resultIn = ofSystemLoadDialog ( "Select corpus file...", false, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !resultIn.bSuccess )
	{
		ofLogError ( "ofApp::startUMAP" ) << "No file selected";
		return;
	}

	//#ifdef _WIN32
	//auto resultOut = pfd::save_file::save_file("Saving reduced analysis as...", "reduced_corpus.json", { "JSON Files", "*.json" });
	//std::string resultOutPath = resultOut.result();
	//#endif

	ofFileDialogResult resultOut = ofSystemSaveDialog ( "reduced_corpus.json", "Saving reduced analysis as..." );
	if ( !resultOut.bSuccess )
	{
		ofLogError ( "ofApp::startUMAP" ) << "Invalid save query";
		return;
	}
	
	success = mCorpusController.ReduceCorpus ( resultIn.getPath ( ), resultOut.getPath ( ) );
	if ( !success )
	{
		ofLogError ( "ofApp" ) << "Corpus reduction failed";
		return;
	}
}