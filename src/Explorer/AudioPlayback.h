#pragma once

#include "./RawView.h"
#include <ofSoundBuffer.h>
#include <ofSoundStream.h>

namespace Acorex {
namespace Explorer {

class AudioPlayback {
public:
	AudioPlayback ( ) { }
	~AudioPlayback ( ) { }

	void Initialise ( double sampleRate );

	void audioOut ( ofSoundBuffer& outBuffer );

	void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }

private:

	std::shared_ptr<RawView> mRawView;

	ofSoundStream mSoundStream;
	float phase = 0;
};

} // namespace Explorer
} // namespace Acorex