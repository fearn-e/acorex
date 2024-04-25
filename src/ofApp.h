#pragma once

#include "AnalyserMenu.h"
#include "ExplorerMenu.h"
#include <ofMain.h>
#include <ofxGui.h>

class ofApp : public ofBaseApp {

public:
	ofApp ( ) { }
	~ofApp ( ) { }

	void setup ( );
	void update ( );
	void draw ( );
	void exit ( );

	//void audioIn ( float* input, int bufferSize, int nChannels );
	//void audioOut ( float* buffer, int bufferSize, int nChannels );
	
	void windowResized ( int w, int h );
	//void dragEvent ( ofDragInfo dragInfo );
	//void gotMessage ( ofMessage msg );

private:
	void AnalyseToggled ( bool& value );
	void ExploreToggled ( bool& value );

	Acorex::AnalyserMenu mAnalyserMenu;
	Acorex::ExplorerMenu mExplorerMenu;
	Acorex::Utils::MenuLayout mLayout;
	Acorex::Utils::Colors mColors;

	ofxToggle mAnalyseToggle;
	ofxToggle mExploreToggle;
};