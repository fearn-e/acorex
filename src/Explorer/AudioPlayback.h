#pragma once

#include "./RawView.h"
#include "./PointPicker.h"
#include "Utils/Data.h"
#include <ofSoundBuffer.h>
#include <ofSoundStream.h>
#include <ofMesh.h>
#include <vector>
#include <mutex>
#include <atomic>

namespace Acorex {
namespace Explorer {

class AudioPlayback {
public:
	AudioPlayback ( ) { }
	~AudioPlayback ( ) { }

	void Initialise ( );
	void RestartAudio ( size_t sampleRate, size_t bufferSize, ofSoundDevice outDevice );

	void audioOut ( ofSoundBuffer& outBuffer );

	void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }

	bool CreatePlayhead ( size_t fileIndex, size_t sampleIndex );
	bool KillPlayhead ( size_t playheadID );
	std::vector<Utils::VisualPlayhead> GetPlayheadInfo ( );
	void SetFlagReset ( );
	void WaitForResetConfirm ( );

	void SetTimeCorpus ( const std::vector<ofMesh>& timeCorpus );

	void SetPointPicker ( std::shared_ptr<PointPicker>& pointPicker ) { mPointPicker = pointPicker; }

	void SetLoopPlayheads ( bool loop ) { mLoopPlayheads = loop; }
	void SetCrossoverJumpChance ( int jumpsInAHundred ) { mCrossoverJumpChanceX100 = jumpsInAHundred; }
	void SetCrossfadeSampleLength ( int length ) { mCrossfadeSampleLength = length; }
	void SetMaxJumpDistanceSpace ( int distanceX100 ) { mMaxJumpDistanceSpaceX100 = distanceX100; }
	void SetMaxJumpTargets ( int targets ) { mMaxJumpTargets = targets; }

private:

	void FillAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, Utils::AudioPlayhead* playhead, bool outBufferFull );
	void CrossfadeAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, size_t startSample_A, size_t endSample_A, size_t fileIndex_A, Utils::AudioPlayhead* playhead_B, size_t lengthSetting, bool outBufferFull );

	bool JumpPlayhead ( size_t fileIndex, size_t sampleIndex, size_t playheadIndex );

	void CalculateTriggerPoints ( Utils::AudioPlayhead& playhead );

	std::vector<Utils::AudioPlayhead> mPlayheads;

	std::shared_ptr<RawView> mRawView;
	std::shared_ptr<PointPicker> mPointPicker;

	ofSoundStream mSoundStream;

	bool bStreamStarted = false;

	// settings -----------------------------------
	
	std::atomic<bool> mLoopPlayheads = false;
	std::atomic<int> mCrossoverJumpChanceX100 = 0;
	std::atomic<int> mCrossfadeSampleLength = 256;
	std::atomic<int> mMaxJumpDistanceSpaceX100 = 5;
	std::atomic<int> mMaxJumpTargets = 5;

	// thread safety ------------------------------

	std::mutex mNewPlayheadMutex;
	std::queue<Utils::AudioPlayhead> mNewPlayheads;
	std::queue<size_t> mPlayheadsToKill;
	size_t playheadCounter = 0;

	std::mutex mVisualPlayheadUpdateMutex;
	std::vector<Utils::VisualPlayhead> mVisualPlayheads;

	std::mutex mTimeCorpusMutex;
	std::vector<ofMesh> mTimeCorpus;

	std::atomic<bool> bResetFlag = false;
};

} // namespace Explorer
} // namespace Acorex