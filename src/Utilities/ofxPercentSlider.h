#pragma once

#include <ofxSlider.h>

class ofxPercentSlider : public ofxIntSlider{
	friend class ofPanel;

protected:
	virtual void generateText ( );
};