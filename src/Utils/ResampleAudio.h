#pragma once

#include <raylib.h>
#include <cstring>
#include <iostream>
#include <vector>

// modified code from https://github.com/openframeworks/openFrameworks @ libs/openframeworks/sound/ofSoundBuffer
//--------------------------------------------------------------------------------------------------------------

// original license from openFrameworks:
/*
openFrameworks is distributed under the MIT License. This gives everyone the freedoms to use openFrameworks
in any context: commercial or non-commercial, public or private, open or closed source.

Copyright (c) 2025 - openFrameworks Community

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.
*/

class ResampleAudio {
public:
	// from http://musicdsp.org/showArchiveComment.php?ArchiveID=93
	// laurent de soras
	//--------------------------------------------------
	template <typename Type>
	inline Type ofInterpolateHermite(const Type & y0, const Type & y1, const Type & y2, const Type & y3, float pct) const {
		const Type c = (y2 - y0) * 0.5f;
		const Type v = y1 - y2;
		const Type w = c + v;
		const Type a = w + v + (y3 - y1) * 0.5f;
		const Type b_neg = w + a;

		return ((((a * pct) - b_neg) * pct + c) * pct + y1);
	}

	bool hermiteResampleTo( std::vector<float> &buffer, double fileRate, double targetRate,
							std::size_t inFrames, std::size_t inChannels) const
	{
		std::vector<float> modifiedBuffer = buffer;
		
		double speed = fileRate / targetRate;
		std::size_t numFrames = inFrames / speed;
		std::size_t totalModBufferSize = numFrames * inChannels;
		
		if(totalModBufferSize < modifiedBuffer.max_size()) {
			modifiedBuffer.resize(totalModBufferSize,0);
		} else {
			std::cerr << "resampling would create a buffer size of " << totalModBufferSize << " (too large for std::vector)" << std::endl;
			return false;
		}
		
		std::size_t start = 0;
		std::size_t end = start*inChannels + double(numFrames*inChannels)*speed;
		double position = start;
		std::size_t intPosition = position;
		float remainder = position - intPosition;
		float increment = speed;
		std::size_t copySize = inChannels*sizeof(float);
		std::size_t to;
		
		if(end<(numFrames*inChannels)-3*inChannels){
			to = numFrames;
		}else if(3>inFrames){
			to = 0;
		}else{
			to = double(inFrames-3)/speed;
		}
		
		float * resBufferPtr = &modifiedBuffer[0];
		float a,b,c,d;
		std::size_t from = 0;
		
		while(intPosition==0){
			intPosition *= inChannels;
			for(std::size_t j=0;j<inChannels;++j){
				a=0;
				b=buffer[intPosition+j];
				c=buffer[intPosition+j+inChannels];
				d=buffer[intPosition+j+inChannels*2];
				*resBufferPtr++ = ofInterpolateHermite(a, b, c, d, remainder);
			}
			position += increment;
			intPosition = position;
			remainder = position - intPosition;
			from++;
		}
		
		for(std::size_t i=from;i<to;++i){
			intPosition *= inChannels;
			for(std::size_t j=0;j<inChannels;++j){
				a=buffer[intPosition+j-inChannels];
				b=buffer[intPosition+j];
				c=buffer[intPosition+j+inChannels];
				d=buffer[intPosition+j+inChannels*2];
				*resBufferPtr++ = ofInterpolateHermite(a, b, c, d, remainder);
			}
			position += increment;
			intPosition = position;
			remainder = position - intPosition;
		}
		
		if(end>=(numFrames*inChannels)-3*inChannels){
			to = numFrames-to;
			memset(resBufferPtr,0,to*copySize);
		}

		buffer = modifiedBuffer;
		return true;
	}
};