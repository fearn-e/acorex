/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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
};