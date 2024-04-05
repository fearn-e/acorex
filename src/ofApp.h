#pragma once

#include "Analyse/ControllerMenu.h"
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

	//void keyPressed ( int key );
	//void keyReleased ( int key );
	//void mouseMoved ( int x, int y );
	//void mouseDragged ( int x, int y, int button );
	//void mousePressed ( int x, int y, int button );
	//void mouseReleased ( int x, int y, int button );
	//void mouseEntered ( int x, int y );
	//void mouseExited ( int x, int y );
	//void windowResized ( int w, int h );
	//void dragEvent ( ofDragInfo dragInfo );
	//void gotMessage ( ofMessage msg );

private:
	void AnalyseToggled ( bool& value );
	void ExploreToggled ( bool& value );

	AcorexAnalyse::ControllerMenu mControllerMenu;
	AcorexUtils::MenuLayout mLayout;
	AcorexUtils::Colors mColors;

	ofxToggle mAnalyseToggle;
	ofxToggle mExploreToggle;
};