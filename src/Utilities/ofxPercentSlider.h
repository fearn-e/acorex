#pragma once

#include <ofxSlider.h>

class ofxPercentSlider : public ofxIntSlider{
	friend class ofPanel;

public:
	int operator=( int v );

protected:
	virtual void generateText ( );
};