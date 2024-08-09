#pragma once

#include <ofSoundBuffer.h>
#include <ofSoundStream.h>

namespace Acorex {
namespace Explorer {

class AudioPlayback {
public:
	AudioPlayback ( ) { }
	~AudioPlayback ( ) { }

	void Initialise ( );

	void audioOut ( ofSoundBuffer& outBuffer );

private:

	ofSoundStream mSoundStream;
	float phase = 0;
};

} // namespace Explorer
} // namespace Acorex