#include "./AudioPlayback.h"
#include <ofMathConstants.h> // REMOVE WHEN REMOVING TEMP TWO_PI STUFF

using namespace Acorex;

void Explorer::AudioPlayback::Initialise ( double sampleRate )
{
	ofSoundStreamSettings settings;
	settings.numInputChannels = 0;
	settings.numOutputChannels = 2;
	settings.sampleRate = sampleRate;
	settings.bufferSize = 512;
	settings.numBuffers = 4;
	settings.setOutListener ( this );

	auto devices = mSoundStream.getDeviceList ( ofSoundDevice::Api::MS_DS ); // TODO - set devices properly with UI
	settings.setOutDevice ( devices[0] );

	mSoundStream.setup ( settings );
}

void Explorer::AudioPlayback::audioOut ( ofSoundBuffer& outBuffer )
{
	float freq = 440.0f;
	float phaseStep = TWO_PI * freq / (float)mSoundStream.getSampleRate ( );

	while ( phase > TWO_PI )
	{
		phase -= TWO_PI;
	}

	for ( size_t i = 0; i < outBuffer.getNumFrames ( ); i++ )
	{
		float sample = sin ( phase );
		outBuffer.getSample ( i, 0 ) = sample;
		outBuffer.getSample ( i, 1 ) = sample;
		phase += phaseStep;
	}
}