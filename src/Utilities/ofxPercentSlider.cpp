#include "Utilities/ofxPercentSlider.h"
#include <ofGraphics.h>

using std::string;

int ofxPercentSlider::operator=( int v )
{
	value = v;
	return v;
}

void ofxPercentSlider::generateText(){
    // convert value from int of 0-1000 to a string that shows the values as 00.0% to 100.0%
    string valStr = ofToString ( (float)value.get ( ) / 10.0f, 1 ) + "%";
	auto inputWidth = getTextBoundingBox(valStr,0,0).width;
	auto yPos = getTextVCenteredInRect(b);
	auto label = getTextBoundingBox(getName(), b.x + textPadding, yPos);
	auto value = getTextBoundingBox(valStr, b.x + b.width - textPadding - inputWidth, yPos);
	overlappingLabel = label.getMaxX() > value.x;

	textMesh.clear();
	if(!mouseInside || !overlappingLabel){
		std::string name;
		if(overlappingLabel){
			for(auto c: ofUTF8Iterator(getName())){
				auto next = name;
				ofUTF8Append(next, c);
				if(getTextBoundingBox(next,0,0).width > b.width-textPadding*2){
					break;
				}else{
					name = next;
				}
			}
		}else{
			name = getName();
		}
		textMesh.append(getTextMesh(name, b.x + textPadding, yPos));
	}
	if(!overlappingLabel || mouseInside){
		textMesh.append(getTextMesh(valStr, b.x + b.width - textPadding - getTextBoundingBox(valStr,0,0).width, yPos));
	}
}