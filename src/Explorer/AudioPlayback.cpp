#include "./AudioPlayback.h"
#include "ofLog.h"
#include <random>

using namespace Acorex;

void Explorer::AudioPlayback::Initialise ( size_t sampleRate )
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

	srand ( time ( NULL ) );
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

	double crossoverJumpChance = (double)crossoverJumpsInAHundred / 100.0;

	for ( size_t playheadIndex = 0; playheadIndex < mPlayheads.size ( ); playheadIndex++ )
	{
		Utils::AudioPlayhead* currentPlayhead = &mPlayheads[playheadIndex];

		ofSoundBuffer playheadBuffer;
		playheadBuffer.setSampleRate ( mSoundStream.getSampleRate ( ) );
		playheadBuffer.allocate ( outBuffer.getNumFrames ( ), 1 );
		
		for ( size_t i = 0; i < playheadBuffer.size ( ); i++ )
		{
			playheadBuffer.getSample ( i, 0 ) = 0.0;
		}

		size_t playheadBufferPosition = 0;
		bool jumpNext = false; size_t jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile;
		while ( true )
		{
			// if EOF: loop/kill
			if ( mPlayheads[playheadIndex].triggerSamplePoints.size ( ) == 0 )
			{
				if ( loopPlayheads )
				{
					JumpPlayhead ( mPlayheads[playheadIndex].fileIndex, 0, playheadIndex );
				}
				else
				{
					playheadsToKillThisBuffer.push_back ( mPlayheads[playheadIndex].playheadID );
				}
			}

			// exit loop - no more space in outbuffer, no more triggers hit
			if ( ( playheadBuffer.size ( ) - playheadBufferPosition ) < ( mPlayheads[playheadIndex].triggerSamplePoints.front ( ) - mPlayheads[playheadIndex].sampleIndex ) )
			{
				if ( jumpNext ) //jump
				{
					CrossfadeAudioSegment ( &playheadBuffer, &playheadBufferPosition, jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile, &mPlayheads[playheadIndex], crossfadeSampleLength, true );

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
				CrossfadeAudioSegment ( &playheadBuffer, &playheadBufferPosition, jumpOriginStartSample, jumpOriginEndSample, jumpOriginFile, &mPlayheads[playheadIndex], crossfadeSampleLength, false );

				FillAudioSegment ( &playheadBuffer, &playheadBufferPosition, &mPlayheads[playheadIndex], false );

				jumpNext = false;
			}

			// perform checks on the current trigger, determine the parameters for the next audio segment fill --------------------------------------------
			// after this point it is assumed that a new trigger has been reached

			// if current trigger point is not the final one and causes a jump
			if ( mPlayheads[playheadIndex].triggerSamplePoints.size ( ) > 1 &&
				((double)rand ( ) / RAND_MAX) < crossoverJumpChance &&
				((double)rand ( ) / RAND_MAX) < 0.2 ) //REPLACE SECOND RAND WITH IF KDTREE SEARCH RETURNS A POINT - TEMPORARY CODE
			{
				jumpNext = true;
				jumpOriginFile = mPlayheads[playheadIndex].fileIndex;
				jumpOriginStartSample = mPlayheads[playheadIndex].sampleIndex;
				jumpOriginEndSample = mPlayheads[playheadIndex].triggerSamplePoints.front ( );

				size_t jumpFileIndex = rand ( ) % mRawView->GetDataset ( )->fileList.size ( ); // REPLACE RAND WITH KDTREE NEAREST FILE
				size_t jumpSampleIndex = rand ( ) % mRawView->GetAudioData ( )->raw[jumpFileIndex].size ( ); // REPLACE RAND WITH KDTREE NEAREST TIME
				JumpPlayhead ( jumpFileIndex, jumpSampleIndex, playheadIndex );

				continue;
			}
			else // if current trigger point is the final one
			{
				mPlayheads[playheadIndex].triggerSamplePoints.pop ( );
			}
		}
		
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
}

/* if ( mRawView->GetAudioData ( )->raw[mPlayheads[playheadIndex].fileIndex].getSampleRate ( ) != mSoundStream.getSampleRate ( ) )
{
	// RESAMPLE ALGO
	if ( sampleRate != globalSampleRate )
	{
		audioData.resample ( (sampleRate / globalSampleRate), ofSoundBuffer::Hermite );
		audioData.setSampleRate ( globalSampleRate );
	}
} */

// need to add above resampling code to the FillAudioSegment and CrossfadeAudioSegment functions


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

		ofLogNotice ( "AudioPlayback" ) << "New playhead created for " << mRawView->GetDataset ( )->fileList[fileIndex] << " at sample " << sampleIndex;

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
	while ( bResetFlag )
	{
		// wait for reset to be confirmed
	}
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