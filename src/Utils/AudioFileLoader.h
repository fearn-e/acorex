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

#include <data/TensorTypes.hpp>
#include <audio_file/in_file.hpp>
#include <ofxAudioFile.h>
#include <string>

namespace Acorex {
namespace Utils {

class AudioFileLoader {
public:
	AudioFileLoader ( ) { }
	~AudioFileLoader ( ) { }

	bool ReadAudioFile ( std::string filename, fluid::RealVector& output, double targetSampleRate );

private:
	//void ReadToMono ( std::vector<float>& output, htl::in_audio_file& file );
	void ReadToMono ( std::vector<float>& output, ofxAudioFile& file );

	void Resample ( std::vector<float>& audio, double fileRate, double targetRate );
};

} // namespace Utils
} // namespace Acorex