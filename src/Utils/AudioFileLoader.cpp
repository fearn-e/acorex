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

#include "./AudioFileLoader.h"
#include <ofSoundBuffer.h>
#include "ofLog.h"

using namespace Acorex;

bool Utils::AudioFileLoader::ReadAudioFile ( std::string filename, fluid::RealVector& output, double targetSampleRate )
{
    //if file ends in .wav, .aiff, .flac - use htl::in_audio_file
    //if file ends in .mp3, .ogg - use ofxAudioFile

    if ( !ofFile::doesFileExist ( filename ) )
    {
        ofLogError ( "GenAnalysis" ) << "input file " << filename << " does not exist";
        return false;
    }

    /*if ( filename.find ( ".wav" ) != std::string::npos || filename.find ( ".flac" ) != std::string::npos )
    {
        htl::in_audio_file file ( filename.c_str ( ) );

        if ( !file.is_open ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << filename << " could not be opened";
            return false;
        }

        if ( file.is_error ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << filename << " returned errors:";
            for ( auto& error : file.get_errors ( ) )
            {
				ofLogError ( "GenAnalysis" ) << file.error_string ( error );
			}
            return false;
        }

        std::vector<float> temp;

        ReadToMono ( temp, file );

        Resample ( temp, file.sampling_rate ( ), targetSampleRate );

        output.resize ( file.frames ( ) );
        std::copy ( temp.begin ( ), temp.end ( ), output.data ( ) );
    }*/
    if ( filename.find ( ".mp3" ) != std::string::npos || filename.find ( ".ogg" ) != std::string::npos ||
        filename.find ( ".wav" ) != std::string::npos || filename.find ( ".flac" ) != std::string::npos )
    {
        ofxAudioFile file;
        file.load ( filename );

        if ( !file.loaded ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << filename << " could not be opened";
            return false;
        }

        std::vector<float> temp;

        ReadToMono ( temp, file );

        Resample ( temp, file.samplerate ( ), targetSampleRate );

        output.resize ( temp.size ( ) );
        std::copy ( temp.begin ( ), temp.end ( ), output.data ( ) );
    }
    else
    {
        ofLogError ( "GenAnalysis" ) << "input file " << filename << " is not supported";
		return false;
    }

    return true;
}

//void Utils::AudioFileLoader::ReadToMono ( std::vector<float>& output, htl::in_audio_file& file )
//{
//    int numChannels = file.channels ( );
//    int numSamples = file.frames ( );
//    output.resize ( numSamples );
//
//    if ( numChannels == 1 )
//    {
//        file.read_channel ( output.data ( ), numSamples, 0 );
//        return;
//    }
//
//    std::fill ( output.begin ( ), output.end ( ), 0 );
//
//    std::vector<std::vector<double>> allChannels ( numChannels, std::vector<double> ( numSamples ) );
//
//    for ( int channel = 0; channel < numChannels; channel++ )
//    {
//        file.read_channel ( allChannels[channel].data ( ), numSamples, channel );
//    }
//
//#pragma omp parallel for
//    for ( int sample = 0; sample < numSamples; sample++ )
//    {
//        for ( int channel = 0; channel < numChannels; channel++ )
//        {
//            output[sample] += allChannels[channel][sample];
//        }
//        output[sample] /= numChannels;
//    }
//
//}

void Utils::AudioFileLoader::ReadToMono ( std::vector<float>& output, ofxAudioFile& file )
{
    int numChannels = file.channels ( );
    int numSamples = file.length ( );
    output.resize ( numSamples );

    if ( numChannels == 1 )
    {
        for ( int sample = 0; sample < numSamples; sample++ )
        {
            output[sample] = file.sample ( sample, 0 );
        }
        return;
    }

    std::fill ( output.begin ( ), output.end ( ), 0 );

#pragma omp parallel for
    for ( int sample = 0; sample < numSamples; sample++ )
    {
        for ( int channel = 0; channel < numChannels; channel++ )
        {
            output[sample] += file.sample ( sample, channel );
        }
        output[sample] /= numChannels;
    }
}

void Utils::AudioFileLoader::Resample ( std::vector<float>& audio, double fileRate, double targetRate  )
{
    ofSoundBuffer resampleBuffer;

    resampleBuffer.copyFrom ( audio, 1, fileRate );
    
    resampleBuffer.resample ( (fileRate / targetRate), ofSoundBuffer::Hermite );
    resampleBuffer.setSampleRate ( targetRate );

    audio.resize ( resampleBuffer.size ( ) );
    resampleBuffer.copyTo ( audio.data ( ), resampleBuffer.size ( ), 1, 0, false );
}