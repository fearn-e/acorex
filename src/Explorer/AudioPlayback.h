#pragma once

#include "./RawView.h"
#include "Utils/Data.h"
#include <ofSoundBuffer.h>
#include <ofSoundStream.h>
#include <vector>
#include <mutex>
#include <atomic>

namespace Acorex {
namespace Explorer {

class AudioPlayback {
public:
	AudioPlayback ( ) { }
	~AudioPlayback ( ) { }

	void Initialise ( size_t sampleRate );

	void audioOut ( ofSoundBuffer& outBuffer );

	void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }
	void SetCrossoverJumpChance ( int jumpsInAHundred ) { crossoverJumpsInAHundred = jumpsInAHundred; }

	bool CreatePlayhead ( size_t fileIndex, size_t sampleIndex );
	bool KillPlayhead ( size_t playheadID );
	std::vector<Utils::VisualPlayhead> GetPlayheadInfo ( );
	void SetFlagReset ( );
	void WaitForResetConfirm ( );

private:

	void FillAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, Utils::AudioPlayhead* playhead, bool outBufferFull );
	void CrossfadeAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, size_t startSample_A, size_t endSample_A, size_t fileIndex_A, Utils::AudioPlayhead* playhead_B, size_t lengthSetting, bool outBufferFull );

	bool JumpPlayhead ( size_t fileIndex, size_t sampleIndex, size_t playheadIndex );

	void CalculateTriggerPoints ( Utils::AudioPlayhead& playhead );

	int crossoverJumpsInAHundred = 80;
	int crossfadeSampleLength = 256;
	bool loopPlayheads = true;

	std::vector<Utils::AudioPlayhead> mPlayheads;

	std::shared_ptr<RawView> mRawView;

	ofSoundStream mSoundStream;

	// thread safety ------------------------------

	std::mutex mNewPlayheadMutex;
	std::queue<Utils::AudioPlayhead> mNewPlayheads;
	std::queue<size_t> mPlayheadsToKill;
	size_t playheadCounter = 0;

	std::mutex mVisualPlayheadUpdateMutex;
	std::vector<Utils::VisualPlayhead> mVisualPlayheads;

	std::atomic<bool> bResetFlag = false;
};

} // namespace Explorer
} // namespace Acorex