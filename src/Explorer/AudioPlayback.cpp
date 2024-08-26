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

#include "./AudioPlayback.h"
#include "ofLog.h"
#include <random>

using namespace Acorex;

void Explorer::AudioPlayback::Initialise ( )
{
	srand ( time ( NULL ) );

	ofSoundDevice outDevice;

	for ( int i = 1; i < ofSoundDevice::Api::NUM_APIS; i++ )
	{
		std::vector<ofSoundDevice> devices = mSoundStream.getDeviceList ( (ofSoundDevice::Api)i );
		for ( auto& device : devices )
		{
			if ( device.outputChannels == 0 ) { continue; }
			outDevice = device;
		}
	}
	RestartAudio ( 44100, 512, outDevice );
}

void Explorer::AudioPlayback::RestartAudio ( size_t sampleRate, size_t bufferSize, ofSoundDevice outDevice )
{
	if ( bStreamStarted )
	{
		SetFlagReset ( );
		WaitForResetConfirm ( );
	}

	if ( bStreamStarted ) { mSoundStream.close ( ); }

	ofSoundStreamSettings settings;
	settings.numInputChannels = 0;
	settings.numOutputChannels = 2;
	settings.sampleRate = sampleRate;
	settings.bufferSize = bufferSize;
	settings.numBuffers = 4;
	settings.setOutListener ( this );
	settings.setOutDevice ( outDevice );

	mSoundStream.setup ( settings );

	bStreamStarted = true;
}

void Explorer::AudioPlayback::audioOut ( ofSoundBuffer& outBuffer )
{
	if ( bResetFlag )
	{
		for ( size_t i = 0; i < mPlayheads.size ( ); i++ )
		{
			mPlayheads.erase ( mPlayheads.begin ( ) + i );
			i--;
		}

		bResetFlag = false;
	}

	for ( size_t sampleIndex = 0; sampleIndex < outBuffer.getNumFrames ( ); sampleIndex++ )
	{
		outBuffer.getSample ( sampleIndex, 0 ) = 0.0;
		outBuffer.getSample ( sampleIndex, 1 ) = 0.0;
	}

	std::vector<size_t> playheadsToKillThisBuffer;

	// get new playheads and playheads to kill from main thread
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
			playheadsToKillThisBuffer.push_back ( mPlayheadsToKill.front ( ) );
			mPlayheadsToKill.pop ( );
		}
	}

	double crossoverJumpChance = (double)mCrossoverJumpChanceX1000 / 1000.0;

	std::chrono::high_resolution_clock::time_point timeStart = std::chrono::high_resolution_clock::now ( );
	std::chrono::microseconds preLoopDuration = std::chrono::microseconds ( 0 );
	std::chrono::microseconds mainLoopDuration = std::chrono::microseconds ( 0 );
	std::chrono::microseconds mainLoopJumpDuration = std::chrono::microseconds ( 0 );
	std::chrono::microseconds afterLoopDuration = std::chrono::microseconds ( 0 );

	std::chrono::high_resolution_clock::time_point triggerStart = std::chrono::high_resolution_clock::now ( );

	for ( size_t playheadIndex = 0; playheadIndex < mPlayheads.size ( ); playheadIndex++ )
	{
		triggerStart = std::chrono::high_resolution_clock::now ( );
		Utils::AudioPlayhead* currentPlayhead = &mPlayheads[playheadIndex];

		ofSoundBuffer playheadBuffer;
		playheadBuffer.setSampleRate ( mSoundStream.getSampleRate ( ) );
		playheadBuffer.allocate ( outBuffer.getNumFrames ( ), 1 );
		
		for ( size_t i = 0; i < playheadBuffer.size ( ); i++ )
		{
			playheadBuffer.getSample ( i, 0 ) = 0.0;
		}
		preLoopDuration += std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now ( ) - triggerStart);

		size_t playheadBufferPosition = 0;
		bool jumpNext = false; size_t jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile;
		while ( true )
		{
			triggerStart = std::chrono::high_resolution_clock::now ( );
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
					JumpPlayhead ( mPlayheads[playheadIndex].fileIndex, 0, playheadIndex );
				}
				else
				{
					playheadsToKillThisBuffer.push_back ( mPlayheads[playheadIndex].playheadID );
					break;
				}
			}

			// exit loop - no more space in outbuffer, no more triggers hit
			if ( ( playheadBuffer.size ( ) - playheadBufferPosition ) < ( mPlayheads[playheadIndex].triggerSamplePoints.front ( ) - mPlayheads[playheadIndex].sampleIndex ) )
			{
				if ( jumpNext ) //jump
				{
					CrossfadeAudioSegment ( &playheadBuffer, &playheadBufferPosition, jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile, &mPlayheads[playheadIndex], mCrossfadeSampleLength, true );

					jumpNext = false;
				}
				else //no jump
				{
					FillAudioSegment ( &playheadBuffer, &playheadBufferPosition, &mPlayheads[playheadIndex], true );
				}

				break;
			}

			// fill audio up to the next trigger, due to previous if we can assume there is enough space in the outbuffer
			if ( !jumpNext ) //no jump
			{
				FillAudioSegment ( &playheadBuffer, &playheadBufferPosition, &mPlayheads[playheadIndex], false );
			}
			else //jump
			{
				CrossfadeAudioSegment ( &playheadBuffer, &playheadBufferPosition, jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile, &mPlayheads[playheadIndex], mCrossfadeSampleLength, false );

				FillAudioSegment ( &playheadBuffer, &playheadBufferPosition, &mPlayheads[playheadIndex], false );

				jumpNext = false;
			}
			mainLoopDuration += std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now ( ) - triggerStart);

			// perform checks on the current trigger, determine the parameters for the next audio segment fill --------------------------------------------
			// after this point it is assumed that a new trigger has been reached

			triggerStart = std::chrono::high_resolution_clock::now ( );
			// if current trigger point is not the final one and causes a jump
			if ( mPlayheads[playheadIndex].triggerSamplePoints.size ( ) > 1 && ((double)rand ( ) / RAND_MAX) < crossoverJumpChance && mTimeCorpusMutex.try_lock ( ) )
			{
				std::lock_guard<std::mutex> lock ( mTimeCorpusMutex, std::adopt_lock );

				size_t timePointIndex = mPlayheads[playheadIndex].sampleIndex / (mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction);
				glm::vec3 playheadPosition = mTimeCorpus[mPlayheads[playheadIndex].fileIndex].getVertex ( timePointIndex );
				Utils::PointFT nearestPoint;
				Utils::PointFT currentPoint; currentPoint.file = mPlayheads[playheadIndex].fileIndex; currentPoint.time = timePointIndex;

				if ( mPointPicker->FindNearestToPosition ( playheadPosition, nearestPoint, currentPoint, mMaxJumpDistanceSpaceX1000, mMaxJumpTargets ) )
				{
					jumpNext = true;
					jumpOriginFile = mPlayheads[playheadIndex].fileIndex;
					jumpOriginStartSample = mPlayheads[playheadIndex].sampleIndex;
					jumpOriginEndSample = mPlayheads[playheadIndex].triggerSamplePoints.front ( );

					size_t jumpFileIndex = nearestPoint.file;
					size_t jumpSampleIndex = nearestPoint.time * (mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction);
					JumpPlayhead ( jumpFileIndex, jumpSampleIndex, playheadIndex );
				}
			}
			mainLoopJumpDuration += std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now ( ) - triggerStart);
		}
		
		triggerStart = std::chrono::high_resolution_clock::now ( );
		// if playhead is marked for death, apply a fade out and remove from playheads
		{
			std::vector<size_t>::iterator it = std::find ( playheadsToKillThisBuffer.begin ( ), playheadsToKillThisBuffer.end ( ), mPlayheads[playheadIndex].playheadID );
			size_t killIndex = std::distance ( playheadsToKillThisBuffer.begin ( ), it );
			if ( it != playheadsToKillThisBuffer.end ( ) )
			{
				for ( size_t i = 0; i < playheadBuffer.size ( ); i++ )
				{
					float gain = cos ( (float)i / (float)playheadBuffer.size ( ) * 0.5 * M_PI );
					playheadBuffer.getSample ( i, 0 ) *= gain;
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
			outBuffer.getSample ( sampleIndex, 1 ) += playheadBuffer.getSample ( sampleIndex, 0 );
		}
		afterLoopDuration += std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now ( ) - triggerStart);
	}

	std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now ( );

	std::chrono::microseconds totalDuration = std::chrono::duration_cast<std::chrono::microseconds> (timeEnd - timeStart);
	if ( totalDuration.count ( ) > 10000 ) // only log if over 10ms
	{
		ofLogNotice ( "audio" ) << totalDuration.count ( ) << "us | "
			<< (float)preLoopDuration.count ( ) / (float)totalDuration.count ( ) * 100.0 << "% | "
			<< (float)mainLoopDuration.count ( ) / (float)totalDuration.count ( ) * 100.0 << "% | "
			<< (float)mainLoopJumpDuration.count ( ) / (float)totalDuration.count ( ) * 100.0 << "% | "
			<< (float)afterLoopDuration.count ( ) / (float)totalDuration.count ( ) * 100.0 << "% | ";
	}

	if ( mVisualPlayheadUpdateMutex.try_lock ( ) )
	{
		std::lock_guard<std::mutex> lock ( mVisualPlayheadUpdateMutex, std::adopt_lock );

		if ( mVisualPlayheads.size ( ) > 0 ) { mVisualPlayheads.clear ( ); }

		for ( size_t i = 0; i < mPlayheads.size ( ); i++ )
		{
			mVisualPlayheads.push_back ( Utils::VisualPlayhead ( mPlayheads[i].playheadID, mPlayheads[i].fileIndex, mPlayheads[i].sampleIndex ) );
		}
	}

	mActivePlayheads = mPlayheads.size ( );
}

void Explorer::AudioPlayback::FillAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, Utils::AudioPlayhead* playhead, bool outBufferFull )
{
	size_t segmentLength = playhead->triggerSamplePoints.front ( ) - playhead->sampleIndex;

	if ( outBufferFull && segmentLength > (outBuffer->size ( ) - *outBufferPosition) ) // cut off early if outBuffer is full
	{
		segmentLength = outBuffer->size ( ) - *outBufferPosition;
	}

	for ( size_t i = 0; i < segmentLength; i++ )
	{
		outBuffer->getSample ( *outBufferPosition + i, 0 ) = mRawView->GetAudioData ( )->raw[playhead->fileIndex].getSample ( playhead->sampleIndex + i, 0 );
	}

	playhead->sampleIndex += segmentLength;
	*outBufferPosition += segmentLength;
}

void Explorer::AudioPlayback::CrossfadeAudioSegment ( ofSoundBuffer* outBuffer, size_t* outBufferPosition, size_t startSample_A, size_t endSample_A, size_t fileIndex_A, Utils::AudioPlayhead* playhead_B, size_t lengthSetting, bool outBufferFull )
{
	size_t originLength = endSample_A - startSample_A;
	size_t jumpLength = playhead_B->triggerSamplePoints.front ( ) - playhead_B->sampleIndex;
	size_t crossfadeLength = (originLength < jumpLength) ? originLength : jumpLength;
	crossfadeLength = (crossfadeLength < lengthSetting) ? crossfadeLength : lengthSetting;

	if ( outBufferFull && crossfadeLength > ( outBuffer->size ( ) - *outBufferPosition ) ) // cut off early if outBuffer is full
	{
		crossfadeLength = outBuffer->size ( ) - *outBufferPosition;
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

bool Explorer::AudioPlayback::JumpPlayhead ( size_t fileIndex, size_t sampleIndex, size_t playheadIndex )
{
	if ( mRawView->GetAudioData ( )->loaded[fileIndex] )
	{
		mPlayheads[playheadIndex].fileIndex = fileIndex;
		mPlayheads[playheadIndex].sampleIndex = sampleIndex;

		CalculateTriggerPoints ( mPlayheads[playheadIndex] );

		return true;
	}
	else
	{
		ofLogError ( "AudioPlayback" ) << "File not loaded in memory, failed to move playhead for " << mRawView->GetDataset ( )->fileList[fileIndex];
		return false;
	}
}

bool Explorer::AudioPlayback::CreatePlayhead ( size_t fileIndex, size_t sampleIndex )
{
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
		Utils::AudioPlayhead newPlayhead ( playheadCounter, fileIndex, sampleIndex );
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

std::vector<Utils::VisualPlayhead> Explorer::AudioPlayback::GetPlayheadInfo ( )
{
	std::lock_guard<std::mutex> lock ( mVisualPlayheadUpdateMutex );

	return mVisualPlayheads;
}

void Explorer::AudioPlayback::SetFlagReset ( )
{
	bResetFlag = true;
}

void Explorer::AudioPlayback::WaitForResetConfirm ( )
{
	if ( !bStreamStarted ) { return; }
	while ( bResetFlag )
	{
		if ( mActivePlayheads == 0 ) { return; }
	}
}

void Explorer::AudioPlayback::SetTimeCorpus ( const std::vector<ofMesh>& timeCorpus )
{
	std::lock_guard<std::mutex> lock ( mTimeCorpusMutex );

	mTimeCorpus = timeCorpus;
}

void Explorer::AudioPlayback::CalculateTriggerPoints ( Utils::AudioPlayhead& playhead )
{
	while ( !playhead.triggerSamplePoints.empty ( ) )
	{
		playhead.triggerSamplePoints.pop ( );
	}

	int triggerPointDistance = mRawView->GetDataset ( )->analysisSettings.windowFFTSize / mRawView->GetDataset ( )->analysisSettings.hopFraction;
	int currentTriggerPoint = triggerPointDistance;

	while ( currentTriggerPoint < mRawView->GetAudioData ( )->raw[playhead.fileIndex].size ( ) )
	{
		if ( currentTriggerPoint > playhead.sampleIndex )
		{
			playhead.triggerSamplePoints.push ( currentTriggerPoint );
		}
		currentTriggerPoint += triggerPointDistance;
	}

	playhead.triggerSamplePoints.push ( mRawView->GetAudioData ( )->raw[playhead.fileIndex].size ( ) - 1 );
}