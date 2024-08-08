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
	
	void audioOut ( ofSoundBuffer &outBuffer );

	void windowResized ( int w, int h );
	//void dragEvent ( ofDragInfo dragInfo );
	//void gotMessage ( ofMessage msg );

private:
	void RemoveListeners ( );
	void SetupUI ( bool keepValues );
	void AnalyseToggled ( bool& value );
	void ExploreToggled ( bool& value );
	void DPIToggled ( bool& value );

	Acorex::AnalyserMenu mAnalyserMenu;
	Acorex::ExplorerMenu mExplorerMenu;
	Acorex::Utils::MenuLayout mLayout;
	Acorex::Utils::Colors mColors;

	ofxToggle mAnalyseToggle;
	ofxToggle mExploreToggle;
	ofxToggle mDPIToggle;

	bool bListenersAdded = false;

	ofSoundStream mSoundStream;
	float phase = 0;
};