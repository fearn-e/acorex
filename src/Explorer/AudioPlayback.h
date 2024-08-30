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
	void SetJumpSameFileAllowed ( bool allowed ) { mJumpSameFileAllowed = allowed; }
	void SetJumpSameFileMinTimeDiff ( int timeDiff ) { mJumpSameFileMinTimeDiff = timeDiff; }
	void SetCrossoverJumpChance ( int jumpsInAThousand ) { mCrossoverJumpChanceX1000 = jumpsInAThousand; }
	void SetCrossfadeSampleLength ( int length ) { mCrossfadeSampleLength = length; }
	void SetMaxJumpDistanceSpace ( int distanceX1000 ) { mMaxJumpDistanceSpaceX1000 = distanceX1000; }
	void SetMaxJumpTargets ( int targets ) { mMaxJumpTargets = targets; }

private:

	void FillAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, Utils::AudioPlayhead* playhead, bool outBufferFull );
	void CrossfadeAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, size_t startSample_A, size_t endSample_A, size_t fileIndex_A, Utils::AudioPlayhead* playhead_B, size_t lengthSetting, bool outBufferFull );

	void CalculateTriggerPoints ( Utils::AudioPlayhead& playhead );

	std::vector<Utils::AudioPlayhead> mPlayheads;

	std::shared_ptr<RawView> mRawView;
	std::shared_ptr<PointPicker> mPointPicker;

	ofSoundStream mSoundStream;

	bool bStreamStarted = false;

	// settings -----------------------------------
	
	std::atomic<bool> mLoopPlayheads = false;
	std::atomic<bool> mJumpSameFileAllowed = false;
	std::atomic<int> mJumpSameFileMinTimeDiff = 2;
	std::atomic<int> mCrossoverJumpChanceX1000 = 50;
	std::atomic<int> mCrossfadeSampleLength = 256;
	std::atomic<int> mMaxJumpDistanceSpaceX1000 = 50;
	std::atomic<int> mMaxJumpTargets = 5;

	// thread safety ------------------------------

	std::atomic<int> mActivePlayheads = 0;

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