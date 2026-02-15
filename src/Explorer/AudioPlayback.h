/*
The MIT License (MIT)

Copyright (c) 2024-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Explorer/RawView.h"
#include "Explorer/PointPicker.h"
#include "Utils/Data.h"
#include "Utils/DimensionBounds.h"

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
    AudioPlayback ( );
    ~AudioPlayback ( ) { }

    bool StartRestartAudio ( size_t sampleRate, size_t bufferSize, ofSoundDevice outDevice );
    void ClearAndKillAudio ( );

    void audioOut ( ofSoundBuffer& outBuffer );

    void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }

    bool CreatePlayhead ( size_t fileIndex, size_t sampleIndex );
    bool KillPlayhead ( size_t playheadID );
    std::vector<Utils::VisualPlayhead> GetPlayheadInfo ( );
    void SetFlagMissingOutput ( bool missing );
    void WaitForMissingOutputConfirm ( );

    void UserInvokedPause ( bool pause ) { bUserPauseFlag = pause; }

    void SetDimensionBounds ( const Utils::DimensionBoundsData& dimensionBoundsData );
    void SetCorpusMesh ( const std::vector<ofMesh>& corpusMesh );

    void SetPointPicker ( std::shared_ptr<PointPicker>& pointPicker ) { mPointPicker = pointPicker; }

    void SetLoopPlayheads ( bool loop ) { mLoopPlayheads = loop; }
    void SetJumpSameFileAllowed ( bool allowed ) { mJumpSameFileAllowed = allowed; }
    void SetJumpSameFileMinTimeDiff ( int timeDiff ) { mJumpSameFileMinTimeDiff = timeDiff; }
    void SetCrossoverJumpChance ( int jumpsInAThousand ) { mCrossoverJumpChanceX1000 = jumpsInAThousand; }
    void SetCrossfadeSampleLength ( int length ) { mCrossfadeSampleLength = length; }
    void SetMaxJumpDistanceSpace ( int distanceX1000 ) { mMaxJumpDistanceSpaceX1000 = distanceX1000; }
    void SetMaxJumpTargets ( int targets ) { mMaxJumpTargets = targets; }
    void SetVolume(int volumeX1000) { mVolumeX1000 = volumeX1000; }
    void SetDynamicPan ( bool enabled, int dimensionIndex ) { mDynamicPanEnabled = false; mDynamicPanDimensionIndex = dimensionIndex; mDynamicPanEnabled = enabled; }
    void SetPanningStrength ( int panStrengthX1000 ) { mPanningStrengthX1000 = panStrengthX1000; }

private:
    void FillAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, Utils::AudioPlayhead* playhead, bool outBufferFull );
    void CrossfadeAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, size_t startSample_A, size_t endSample_A, size_t fileIndex_A, Utils::AudioPlayhead* playhead_B, size_t lengthSetting, bool outBufferFull );

    void CalculateTriggerPoints ( Utils::AudioPlayhead& playhead );

    std::shared_ptr<RawView> mRawView;
    std::shared_ptr<PointPicker> mPointPicker;

    // TODO - apply a pause/unpause fade over the length of a single buffer
    //bool audioPauseFadeApplied = false;
    //bool audioUnpauseFadeApplied = false;

    // TODO //implement stereo loading of source files, not just mono
    // TODO //panning bias? to globally statically shift this acorex instance left/right
    // TODO //pan smoothing? average dynamic pan position with the previous and next X segments
    
    // audio states ------------------------------

    ofSoundStream mSoundStream;
    std::atomic<bool> bStreamStarted;

    std::mutex mKillAudioOnlyAudioThreadBlockingMutex;

    std::mutex mRestartingAudioMutex;
    std::atomic<bool> bRestartingAudioFlag;
    std::atomic<bool> bRestartingAudioFlagConfirmed;

    std::mutex mMissingOutputMutex;
    std::atomic<bool> bMissingOutputFlag;
    std::atomic<bool> bMissingOutputFlagConfirmed;

    std::atomic<bool> bUserPauseFlag;

    // playhead states ---------------------------

    std::vector<Utils::AudioPlayhead> mPlayheads;
    std::atomic<int> mActivePlayheads;

    std::mutex mNewPlayheadMutex;
    std::queue<Utils::AudioPlayhead> mNewPlayheads;
    std::queue<size_t> mPlayheadsToKill;
    size_t playheadCounter;

    std::mutex mVisualPlayheadUpdateMutex;
    std::vector<Utils::VisualPlayhead> mVisualPlayheads;

    // audio thread local copies ------------------

    std::mutex mDimensionBoundsMutex;
    Utils::DimensionBoundsData mDimensionBounds;

    std::mutex mCorpusMeshMutex;
    std::vector<ofMesh> mCorpusMesh;

    // settings -----------------------------------

    std::atomic<bool> mLoopPlayheads;
    std::atomic<bool> mJumpSameFileAllowed;
    std::atomic<int> mJumpSameFileMinTimeDiff;
    std::atomic<int> mCrossoverJumpChanceX1000;
    std::atomic<int> mCrossfadeSampleLength;
    std::atomic<int> mMaxJumpDistanceSpaceX1000;
    std::atomic<int> mMaxJumpTargets;
    std::atomic<int> mVolumeX1000;
    std::atomic<bool> mDynamicPanEnabled;
    std::atomic<int> mDynamicPanDimensionIndex;
    std::atomic<int> mPanningStrengthX1000;
};

} // namespace Explorer
} // namespace Acorex