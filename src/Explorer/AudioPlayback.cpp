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

#include "Explorer/AudioPlayback.h"

#include "ofLog.h"
#include <random>

using namespace Acorex;

// TODO - use mSoundStream.stop ( ) for killing the stream
// might still need flags depending on when exactly stream is killed (audioOut should be allowed to finish processing)
// but should simplify things

Explorer::AudioPlayback::AudioPlayback ( )
    : bStreamStarted ( false ),
    bRestartingAudioFlag ( false ), bRestartingAudioFlagConfirmed ( false ),
    bMissingOutputFlag ( false ), bMissingOutputFlagConfirmed ( false ),
    bUserPauseFlag ( false ),
    mActivePlayheads ( 0 ), playheadCounter ( 0 ),
    mLoopPlayheads ( false ),
    mJumpSameFileAllowed ( false ), mJumpSameFileMinTimeDiff ( 2 ),
    mCrossoverJumpChanceX1000 ( 50 ), mCrossfadeSampleLength ( 256 ), mMaxJumpDistanceSpaceX1000 ( 50 ), mMaxJumpTargets ( 5 ),
    mVolumeX1000 ( 500 ), mDynamicPanEnabled ( false ), mDynamicPanDimensionIndex ( 0 ), mPanningStrengthX1000 ( 1000 )
{
    srand ( time ( NULL ) );
}

bool Explorer::AudioPlayback::StartRestartAudio ( size_t sampleRate, size_t bufferSize, ofSoundDevice outDevice )
{
    // TODO - apply fade out here if stream already started and active playheads exist ?
    {
        std::lock_guard<std::mutex> lock ( mRestartingAudioMutex );
        bRestartingAudioFlag = true;
    }

    ofSleepMillis ( 100 );

    // TODO - could probably just replace this flag system with the same mutex that is in the kill audio / clear function
    // instead of waiting for the other thread to confirm, just hold until i can grab the mutex that blocks that thread
    // this lets it finish what it's doing, and then i can go ahead
    // alternatively, could combine both to do the fade out effect mentioned above
    while ( bStreamStarted && !bRestartingAudioFlagConfirmed )
    { ofSleepMillis ( 10 ); }

    ofSoundStreamSettings settings;
    {
        settings.numInputChannels = 0;
        settings.numOutputChannels = 2;
        settings.sampleRate = sampleRate;
        settings.bufferSize = bufferSize;
        settings.numBuffers = 4;
        settings.setOutListener ( this );
        settings.setOutDevice ( outDevice );
    }

    {
        std::lock_guard<std::mutex> audioOutLock ( mKillAudioOnlyAudioThreadBlockingMutex );
        mSoundStream.close ( );
    }
    
    bool success = false;

    if ( outDevice.deviceID != -1 ) // outDevice.name != "No output selected." - TODO - maybe use an extra bool parameter instead of piggy backing on the deviceID?
    {
        success = mSoundStream.setup ( settings );
    }

    bStreamStarted = success;

    {
        std::lock_guard<std::mutex> lock ( mMissingOutputMutex );
        bMissingOutputFlag = !success; bMissingOutputFlagConfirmed = false;
    }
    {
        std::lock_guard<std::mutex> lock ( mRestartingAudioMutex );
        bRestartingAudioFlag = false; bRestartingAudioFlagConfirmed = false;
    }

    if ( !success )
    {
        ofLogError ( "AudioPlayback::RestartAudio" ) << "Failed to start audio stream with device \"" << outDevice.name << "\". Audio playback will not function.";
        return false;
    }

    srand ( time ( NULL ) ); // reset random seed just for fun

    return true;
}

void Explorer::AudioPlayback::ClearAndKillAudio ( )
{
    std::lock_guard<std::mutex> killAudioLock ( mKillAudioOnlyAudioThreadBlockingMutex );

    mSoundStream.close ( );
    bStreamStarted = false;

    {
        std::lock_guard<std::mutex> restartAudioLock ( mRestartingAudioMutex );
        bRestartingAudioFlag = false; bRestartingAudioFlagConfirmed = false;
    }

    {
        std::lock_guard<std::mutex> missingOutputLock ( mMissingOutputMutex );
        bMissingOutputFlag = false; bMissingOutputFlagConfirmed = false;
    }

    bUserPauseFlag = false;

    mPlayheads.clear ( );
    mActivePlayheads = 0;

    {
        std::lock_guard<std::mutex> newPlayheadLock ( mNewPlayheadMutex );
        while ( !mNewPlayheads.empty ( ) ) { mNewPlayheads.pop ( ); }
        while ( !mPlayheadsToKill.empty ( ) ) { mPlayheadsToKill.pop ( ); }
        playheadCounter = 0;
    }

    {
        std::lock_guard<std::mutex> visualPlayheadUpdateLock ( mVisualPlayheadUpdateMutex );
        mVisualPlayheads.clear ( );
    }

    {
        std::lock_guard<std::mutex> dimensionBoundsLock ( mDimensionBoundsMutex );
        mDimensionBounds = Utilities::DimensionBoundsData ( );
    }

    {
        std::lock_guard<std::mutex> timeCorpusLock ( mCorpusMeshMutex );
        mCorpusMesh.clear ( );
    }

    mLoopPlayheads = false;
    mJumpSameFileAllowed = false;
    mJumpSameFileMinTimeDiff = 2;
    mCrossoverJumpChanceX1000 = 50;
    mCrossfadeSampleLength = 256;
    mMaxJumpDistanceSpaceX1000 = 50;
    mMaxJumpTargets = 5;
    mVolumeX1000 = 500;
    mDynamicPanEnabled = false;
    mDynamicPanDimensionIndex = 0;
    mPanningStrengthX1000 = 1000;

    return;
}

void Explorer::AudioPlayback::audioOut ( ofSoundBuffer& outBuffer )
{
    // TODO - change this and other lock_guard/try_lock instances to unique_lock with something like:
    //              std::unique_lock<std::mutex> lock ( mNewPlayheadMutex, std::try_to_lock );
    //              if ( lock.owns_lock ( ) )
    if ( mKillAudioOnlyAudioThreadBlockingMutex.try_lock ( ) )
    {
        std::lock_guard<std::mutex> lock ( mKillAudioOnlyAudioThreadBlockingMutex, std::adopt_lock );

        // zero the output buffer
        for ( size_t sampleIndex = 0; sampleIndex < outBuffer.getNumFrames ( ); sampleIndex++ )
        {
            outBuffer.getSample ( sampleIndex, 0 ) = 0.0;
            outBuffer.getSample ( sampleIndex, 1 ) = 0.0;
        }

        // check flags that might block audio processing
        bool audioProcessingBlocked = false;
        {
            if ( mRestartingAudioMutex.try_lock ( ) )
            {
                std::lock_guard<std::mutex> tempLock ( mRestartingAudioMutex, std::adopt_lock );
                if ( bRestartingAudioFlag )
                { audioProcessingBlocked = true;    bRestartingAudioFlagConfirmed = true; }
                else { bRestartingAudioFlagConfirmed = false; }
            }

            if ( mMissingOutputMutex.try_lock ( ) )
            {
                std::lock_guard<std::mutex> tempLock ( mMissingOutputMutex, std::adopt_lock );
                if ( bMissingOutputFlag )
                { audioProcessingBlocked = true;    bMissingOutputFlagConfirmed = true; }
                else { bMissingOutputFlagConfirmed = false; }
            }

            if ( bUserPauseFlag )
            { audioProcessingBlocked = true; }

            //if ( audioProcessingBlocked ) { return; }
        }

        std::vector<size_t> playheadsToKillThisBuffer;

        // get playhead user changes from main thread, kill playheads here if audio processing is blocked
        if ( mNewPlayheadMutex.try_lock ( ) )
        {
            std::lock_guard<std::mutex> lock ( mNewPlayheadMutex, std::adopt_lock );

            while ( !mNewPlayheads.empty ( ) )
            {
                mPlayheads.push_back ( mNewPlayheads.front ( ) );
                mNewPlayheads.pop ( );
            }

            while ( !mPlayheadsToKill.empty ( ) )
            {
                if ( audioProcessingBlocked )
                {
                    for ( size_t i = 0; i < mPlayheads.size ( ); i++ )
                    {
                        if ( mPlayheads[i].playheadID == mPlayheadsToKill.front ( ) )
                        {
                            mPlayheads.erase ( mPlayheads.begin ( ) + i );
                            break;
                        }
                    }
                }
                else
                {
                    playheadsToKillThisBuffer.push_back ( mPlayheadsToKill.front ( ) );
                }

                mPlayheadsToKill.pop ( );
            }
        }

        // audio processing
        if ( !audioProcessingBlocked )
        {
            double crossoverJumpChance = (double)mCrossoverJumpChanceX1000 / 1000.0;
            double volume = (double)mVolumeX1000 / 1000.0;
            double panningStrength = (double)mPanningStrengthX1000 / 1000.0;

            // playhead loop
            for ( size_t playheadIndex = 0; playheadIndex < mPlayheads.size ( ); playheadIndex++ )
            {
                Utilities::AudioPlayhead* currentPlayhead = &mPlayheads[playheadIndex]; // TODO - why is this not used - replace all instances of mPlayheads[playheadIndex] with currentPlayhead?

                ofSoundBuffer playheadBuffer;
                playheadBuffer.setSampleRate ( mSoundStream.getSampleRate ( ) );
                playheadBuffer.allocate ( outBuffer.getNumFrames ( ), 2 );

                for ( size_t i = 0; i < playheadBuffer.getNumFrames ( ); i++ )
                {
                    playheadBuffer.getSample ( i, 0 ) = 0.0;
                    playheadBuffer.getSample ( i, 1 ) = 0.0;
                }

                size_t playheadBufferPosition = 0;
                // processing loop
                while ( true )
                {
                    // crossfade jump
                    // CrossfadeAudioSegment ( &playheadBuffer, &playheadBufferPosition, jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile, &mPlayheads[playheadIndex], mCrossfadeSampleLength, true );
                    if ( mPlayheads[playheadIndex].crossfading )
                    {
                        size_t crossfadeSamplesLeft = mPlayheads[playheadIndex].crossfadeSampleLength - mPlayheads[playheadIndex].crossfadeCurrentSample;
                        size_t bufferSpace = playheadBuffer.getNumFrames ( ) - playheadBufferPosition;
                        if ( crossfadeSamplesLeft > bufferSpace ) { crossfadeSamplesLeft = bufferSpace; }

                        float panStartNorm = 0.5f, panEndNorm = 0.5f;
                        if ( mDynamicPanEnabled && panningStrength > 0.0 )
                        {
                            size_t thisTimePointIndex = mPlayheads[playheadIndex].sampleIndex / mRawView->GetHopSize ( );
                            size_t jumpTimePointIndex = mPlayheads[playheadIndex].jumpSampleIndex / mRawView->GetHopSize ( );

                            float panStart = mRawView->GetTrailData ( )->raw[mPlayheads[playheadIndex].fileIndex][thisTimePointIndex][mDynamicPanDimensionIndex];
                            float panEnd = mRawView->GetTrailData ( )->raw[mPlayheads[playheadIndex].jumpFileIndex][jumpTimePointIndex][mDynamicPanDimensionIndex];

                            {
                                std::lock_guard <std::mutex> lock ( mDimensionBoundsMutex );

                                panStartNorm    = (float)(                  panStart                        - mDimensionBounds.min[mDynamicPanDimensionIndex] )
                                                / (float)( mDimensionBounds.max[mDynamicPanDimensionIndex]  - mDimensionBounds.min[mDynamicPanDimensionIndex] );
                                panEndNorm      = (float)(                  panEnd                          - mDimensionBounds.min[mDynamicPanDimensionIndex] ) 
                                                / (float)( mDimensionBounds.max[mDynamicPanDimensionIndex]  - mDimensionBounds.min[mDynamicPanDimensionIndex] );
                            }

                            panStartNorm =  glm::clamp ( panStartNorm,  0.0f, 1.0f );
                            panEndNorm =    glm::clamp ( panEndNorm,    0.0f, 1.0f );
                        }

                        for ( size_t i = 0; i < crossfadeSamplesLeft; i++ )
                        {
                            float crossfadeProgress = (float)(mPlayheads[playheadIndex].crossfadeCurrentSample + i) / (float)mPlayheads[playheadIndex].crossfadeSampleLength;

                            float gain_A = cos ( crossfadeProgress * 0.5 * M_PI );
                            float gain_B = sin ( crossfadeProgress * 0.5 * M_PI );

                            float sample_A = mRawView->GetAudioData ( )->raw[mPlayheads[playheadIndex].fileIndex].getSample ( mPlayheads[playheadIndex].sampleIndex + i, 0 );
                            float sample_B = mRawView->GetAudioData ( )->raw[mPlayheads[playheadIndex].jumpFileIndex].getSample ( mPlayheads[playheadIndex].jumpSampleIndex + i, 0 );
                            float samplePostCrossfade = (sample_A * gain_A) + (sample_B * gain_B);

                            float panGainL = 1.0f, panGainR = 1.0f;
                            if ( mDynamicPanEnabled && panningStrength > 0.0 )
                            {
                                float panPostCrossfade = panStartNorm + (panEndNorm - panStartNorm) * crossfadeProgress;
                                panGainL = cos ( panPostCrossfade * 0.5 * M_PI );
                                panGainR = sin ( panPostCrossfade * 0.5 * M_PI );

                                panGainL = 1.0f - panningStrength * (1.0f - panGainL);
                                panGainR = 1.0f - panningStrength * (1.0f - panGainR);
                                // TODO - could have a power curve here instead of linear - something like: float result = 1.0f - pow(Y, power) * (1.0f - X);
                                // TODO - same thing further down in FillAudioSegment();
                            }

                            playheadBuffer.getSample ( playheadBufferPosition + i, 0 ) = samplePostCrossfade * panGainL;
                            playheadBuffer.getSample ( playheadBufferPosition + i, 1 ) = samplePostCrossfade * panGainR;
                        }

                        mPlayheads[playheadIndex].crossfadeCurrentSample += crossfadeSamplesLeft;
                        mPlayheads[playheadIndex].sampleIndex += crossfadeSamplesLeft;
                        mPlayheads[playheadIndex].jumpSampleIndex += crossfadeSamplesLeft;
                        playheadBufferPosition += crossfadeSamplesLeft;

                        if ( mPlayheads[playheadIndex].crossfadeCurrentSample >= mPlayheads[playheadIndex].crossfadeSampleLength )
                        {
                            mPlayheads[playheadIndex].crossfading = false;
                            mPlayheads[playheadIndex].fileIndex = mPlayheads[playheadIndex].jumpFileIndex;
                            mPlayheads[playheadIndex].sampleIndex = mPlayheads[playheadIndex].jumpSampleIndex;
                            CalculateTriggerPoints ( mPlayheads[playheadIndex] );
                        }
                        else
                        {
                            break;
                        }
                    }

                    // remove trigger points that have been hit
                    while ( mPlayheads[playheadIndex].triggerSamplePoints.size ( ) > 0 && mPlayheads[playheadIndex].sampleIndex >= mPlayheads[playheadIndex].triggerSamplePoints.front ( ) )
                    {
                        mPlayheads[playheadIndex].triggerSamplePoints.pop ( );
                    }

                    // if EOF: loop/kill
                    if ( mPlayheads[playheadIndex].triggerSamplePoints.size ( ) == 0 )
                    {
                        if ( mLoopPlayheads )
                        {
                            mPlayheads[playheadIndex].sampleIndex = 0;
                            CalculateTriggerPoints ( mPlayheads[playheadIndex] );
                        }
                        else
                        {
                            playheadsToKillThisBuffer.push_back ( mPlayheads[playheadIndex].playheadID );
                            break;
                        }
                    }

                    // exit loop - no more space in outbuffer, no more triggers hit
                    if ( (playheadBuffer.getNumFrames ( ) - playheadBufferPosition) < (mPlayheads[playheadIndex].triggerSamplePoints.front ( ) - mPlayheads[playheadIndex].sampleIndex) )
                    {
                        FillAudioSegment ( &playheadBuffer, &playheadBufferPosition, &mPlayheads[playheadIndex], true );
                        break;
                    }

                    // fill audio up to the next trigger, already established that there is enough space in outBuffer
                    FillAudioSegment ( &playheadBuffer, &playheadBufferPosition, &mPlayheads[playheadIndex], false );

                    // after this point it is assumed that a new trigger has been reached, perform jump checks for this trigger

                    int requiredSamples = mCrossfadeSampleLength;
                    if ( mPlayheads[playheadIndex].sampleIndex + requiredSamples >= mRawView->GetAudioData ( )->raw[mPlayheads[playheadIndex].fileIndex].getNumFrames ( ) ) { continue; }
                    if ( ((double)rand ( ) / RAND_MAX) > crossoverJumpChance ) { continue; }
                    if ( mCorpusMeshMutex.try_lock ( ) )
                        // TODO - investigate if this lock could work differently, currently this would cause a brief moment of no possible jumps
                    {
                        std::lock_guard<std::mutex> lock ( mCorpusMeshMutex, std::adopt_lock );

                        size_t timePointIndex = mPlayheads[playheadIndex].sampleIndex / mRawView->GetHopSize ( );
                        glm::vec3 playheadPosition = mCorpusMesh[mPlayheads[playheadIndex].fileIndex].getVertex ( timePointIndex );
                        Utilities::PointFT nearestPoint;
                        Utilities::PointFT currentPoint; currentPoint.file = mPlayheads[playheadIndex].fileIndex; currentPoint.time = timePointIndex;

                        if ( !mPointPicker->FindNearestToPosition ( playheadPosition, nearestPoint, currentPoint,
                                                                    mMaxJumpDistanceSpaceX1000, mMaxJumpTargets, mJumpSameFileAllowed,
                                                                    mJumpSameFileMinTimeDiff, requiredSamples, *mRawView->GetAudioData ( ), mRawView->GetHopSize ( ) ) )
                        {
                            continue;
                        }

                        if ( mRawView->GetAudioData ( )->loaded[nearestPoint.file] == false ) { continue; }

                        mPlayheads[playheadIndex].crossfading = true;
                        mPlayheads[playheadIndex].jumpFileIndex = nearestPoint.file;
                        mPlayheads[playheadIndex].jumpSampleIndex = nearestPoint.time * mRawView->GetHopSize ( );
                        mPlayheads[playheadIndex].crossfadeCurrentSample = 0;
                        mPlayheads[playheadIndex].crossfadeSampleLength = requiredSamples;
                    }
                }

                // if playhead is marked for death, apply a fade out and remove from playheads
                {
                    std::vector<size_t>::iterator it = std::find ( playheadsToKillThisBuffer.begin ( ), playheadsToKillThisBuffer.end ( ), mPlayheads[playheadIndex].playheadID );
                    size_t killIndex = std::distance ( playheadsToKillThisBuffer.begin ( ), it );
                    if ( it != playheadsToKillThisBuffer.end ( ) )
                    {
                        for ( size_t i = 0; i < playheadBuffer.getNumFrames ( ); i++ )
                        {
                            float gain = cos ( (float)i / (float)playheadBuffer.getNumFrames ( ) * 0.5 * M_PI );
                            playheadBuffer.getSample ( i, 0 ) *= gain;
                            playheadBuffer.getSample ( i, 1 ) *= gain;
                        }

                        playheadsToKillThisBuffer.erase ( playheadsToKillThisBuffer.begin ( ) + killIndex );

                        mPlayheads.erase ( mPlayheads.begin ( ) + playheadIndex );
                        playheadIndex--;
                    }
                }

                // processing done, add to outBuffer
                for ( size_t sampleIndex = 0; sampleIndex < outBuffer.getNumFrames ( ); sampleIndex++ )
                {
                    outBuffer.getSample ( sampleIndex, 0 ) += playheadBuffer.getSample ( sampleIndex, 0 );
                    outBuffer.getSample ( sampleIndex, 1 ) += playheadBuffer.getSample ( sampleIndex, 1 );
                }
            }

            // multiply by global volume
            for ( size_t sampleIndex = 0; sampleIndex < outBuffer.getNumFrames ( ); sampleIndex++ )
            {
                outBuffer.getSample ( sampleIndex, 0 ) *= volume;
                outBuffer.getSample ( sampleIndex, 1 ) *= volume;
            }
        }

        // get playhead post-processing location info for main thread
        if ( mVisualPlayheadUpdateMutex.try_lock ( ) )
        {
            std::lock_guard<std::mutex> lock ( mVisualPlayheadUpdateMutex, std::adopt_lock );

            mVisualPlayheads.clear ( );

            for ( size_t i = 0; i < mPlayheads.size ( ); i++ )
            {
                mVisualPlayheads.push_back ( Utilities::VisualPlayhead ( mPlayheads[i].playheadID, mPlayheads[i].fileIndex, mPlayheads[i].sampleIndex ) );
            }
        }

        mActivePlayheads = mPlayheads.size ( );
    }
}

void Explorer::AudioPlayback::FillAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, Utilities::AudioPlayhead* playhead, bool outBufferFull )
{
    float panGainL = 1.0f, panGainR = 1.0f;
    double panningStrength = (double)mPanningStrengthX1000 / 1000.0;
    if ( mDynamicPanEnabled && panningStrength > 0.0 )
    {
        size_t timePointIndex = playhead->sampleIndex / mRawView->GetHopSize ( );
        float pan = mRawView->GetTrailData ( )->raw[playhead->fileIndex][timePointIndex][mDynamicPanDimensionIndex];
        float panNorm = 0.5f;
        {
            std::lock_guard <std::mutex> lock ( mDimensionBoundsMutex );

            panNorm = (float)(                 pan                            - mDimensionBounds.min[mDynamicPanDimensionIndex])
                    / (float)(mDimensionBounds.max[mDynamicPanDimensionIndex] - mDimensionBounds.min[mDynamicPanDimensionIndex]);
        }
        panNorm = glm::clamp ( panNorm, 0.0f, 1.0f );
        panGainL = cos ( panNorm * 0.5 * M_PI );
        panGainR = sin ( panNorm * 0.5 * M_PI );

        panGainL = 1.0f - panningStrength * (1.0f - panGainL);
        panGainR = 1.0f - panningStrength * (1.0f - panGainR);
    }

    size_t segmentLength = playhead->triggerSamplePoints.front ( ) - playhead->sampleIndex;

    if ( outBufferFull && segmentLength > (outBuffer->getNumFrames ( ) - *outBufferPosition) ) // cut off early if outBuffer is full
    {
        segmentLength = outBuffer->getNumFrames ( ) - *outBufferPosition;
    }

    for ( size_t i = 0; i < segmentLength; i++ )
    {
        outBuffer->getSample ( *outBufferPosition + i, 0 ) = mRawView->GetAudioData ( )->raw[playhead->fileIndex].getSample ( playhead->sampleIndex + i, 0 ) * panGainL;
        outBuffer->getSample ( *outBufferPosition + i, 1 ) = mRawView->GetAudioData ( )->raw[playhead->fileIndex].getSample ( playhead->sampleIndex + i, 0 ) * panGainR;
    }

    playhead->sampleIndex += segmentLength;
    *outBufferPosition += segmentLength;
}

// TODO - i think this function is no longer used - remove?
void Explorer::AudioPlayback::CrossfadeAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, size_t startSample_A, size_t endSample_A, size_t fileIndex_A, Utilities::AudioPlayhead* playhead_B, size_t lengthSetting, bool outBufferFull )
{
    size_t originLength = endSample_A - startSample_A;
    size_t jumpLength = playhead_B->triggerSamplePoints.front ( ) - playhead_B->sampleIndex;
    size_t crossfadeLength = (originLength < jumpLength) ? originLength : jumpLength;
    crossfadeLength = (crossfadeLength < lengthSetting) ? crossfadeLength : lengthSetting;

    if ( outBufferFull && crossfadeLength > ( outBuffer->getNumFrames ( ) - *outBufferPosition ) ) // cut off early if outBuffer is full
    {
        crossfadeLength = outBuffer->getNumFrames ( ) - *outBufferPosition;
    }


    for ( size_t i = 0; i < crossfadeLength; i++ )
    {
        float gain_A = cos ( (float)i / (float)crossfadeLength * 0.5 * M_PI );
        float gain_B = sin ( (float)i / (float)crossfadeLength * 0.5 * M_PI );

        outBuffer->getSample ( *outBufferPosition + i, 0 ) =	mRawView->GetAudioData ( )->raw[fileIndex_A].getSample ( startSample_A + i, 0 ) * gain_A +
                                                                mRawView->GetAudioData ( )->raw[playhead_B->fileIndex].getSample ( playhead_B->sampleIndex + i, 0 ) * gain_B;
    }

    playhead_B->sampleIndex += crossfadeLength;
    *outBufferPosition += crossfadeLength;
}

bool Explorer::AudioPlayback::CreatePlayhead ( size_t fileIndex, size_t timePointIndex )
{
    if ( bMissingOutputFlag )
    {
        ofLogWarning ( "AudioPlayback" ) << "Missing output flag is active, failed to create new playhead";
        return false;
    }
    else if ( bUserPauseFlag )
    {
        ofLogWarning ( "AudioPlayback" ) << "User pause flag is active, failed to create new playhead";
        return false;
    }
    else if ( !bStreamStarted )
    {
        ofLogWarning ( "AudioPlayback" ) << "Audio stream not started, failed to create new playhead";
        return false;
    }
    else if ( mRawView->GetDataset ( )->fileList.size ( ) == 0 )
    {
        ofLogWarning ( "AudioPlayback" ) << "No files in dataset, failed to create new playhead";
        return false;
    }

    {
        std::lock_guard<std::mutex> lock ( mNewPlayheadMutex );
        if ( mNewPlayheads.size ( ) > 3 )
        {
            ofLogWarning ( "AudioPlayback" ) << "Too many playheads queued already, failed to create new playhead";
            return false;
        }
    }

    if ( mRawView->GetAudioData ( )->loaded[fileIndex] )
    {
        size_t sampleIndex = timePointIndex * mRawView->GetHopSize ( );
        Utilities::AudioPlayhead newPlayhead ( playheadCounter, fileIndex, sampleIndex );
        playheadCounter++;

        CalculateTriggerPoints ( newPlayhead );

        {
            std::lock_guard<std::mutex> lock ( mNewPlayheadMutex );
            mNewPlayheads.push ( newPlayhead );
        }

        return true;
    }
    else
    {
        ofLogError ( "AudioPlayback" ) << "File not loaded in memory, failed to create playhead for " << mRawView->GetDataset ( )->fileList[fileIndex];
        return false;
    }
}

bool Explorer::AudioPlayback::KillPlayhead ( size_t playheadID )
{
    {
        std::lock_guard<std::mutex> lock ( mNewPlayheadMutex );
        mPlayheadsToKill.push ( playheadID );
    }

    return true;
}

std::vector<Utilities::VisualPlayhead> Explorer::AudioPlayback::GetPlayheadInfo ( )
{
    std::lock_guard<std::mutex> lock ( mVisualPlayheadUpdateMutex );

    return mVisualPlayheads;
}

void Explorer::AudioPlayback::SetFlagMissingOutput ( bool missing )
{
    bMissingOutputFlag = missing;
}

void Explorer::AudioPlayback::WaitForMissingOutputConfirm ( )
{
    if ( !bStreamStarted ) { return; }
    while ( bMissingOutputFlag )
    {
        if ( bMissingOutputFlagConfirmed ) { return; }
    }
}

void Explorer::AudioPlayback::SetDimensionBounds ( const Utilities::DimensionBoundsData& dimensionBoundsData )
{
    if ( bStreamStarted )
    {
        ofLogError ( "AudioPlayback" ) << "Attempted to set dimension bounds while audio stream is active, this should never happen, as it could hang the audio thread.";
    }

    std::lock_guard<std::mutex> lock ( mDimensionBoundsMutex );

    mDimensionBounds = dimensionBoundsData;
}

// TODO - change mTimeCorpus from an ofMesh to a more efficient data structure
void Explorer::AudioPlayback::SetCorpusMesh ( const std::vector<ofMesh>& corpusMesh )
{
    std::lock_guard<std::mutex> lock ( mCorpusMeshMutex );

    mCorpusMesh = corpusMesh;
}

void Explorer::AudioPlayback::CalculateTriggerPoints ( Utilities::AudioPlayhead& playhead )
{
    while ( !playhead.triggerSamplePoints.empty ( ) )
    {
        playhead.triggerSamplePoints.pop ( );
    }

    int triggerPointDistance = mRawView->GetHopSize ( );
    int currentTriggerPoint = triggerPointDistance;

    while ( currentTriggerPoint < mRawView->GetAudioData ( )->raw[playhead.fileIndex].size ( ) ) // might have to change if stereo input support is added
    {
        if ( currentTriggerPoint > playhead.sampleIndex )
        {
            playhead.triggerSamplePoints.push ( currentTriggerPoint );
        }
        currentTriggerPoint += triggerPointDistance;
    }

    playhead.triggerSamplePoints.push ( mRawView->GetAudioData ( )->raw[playhead.fileIndex].size ( ) - 1 ); // might have to change if stereo input support is added
}