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

	bool ReadAudioFile ( std::string filename, fluid::RealVector& output, double& sampleRate );

private:
	void ReadToMono ( fluid::RealVector& output, htl::in_audio_file& file );
	void ReadToMono ( fluid::RealVector& output, ofxAudioFile& file );
};

} // namespace Utils
} // namespace Acorex