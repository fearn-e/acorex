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
#include "./ResampleAudio.h"
#include <raylib.h>
#include <iostream>
#include <filesystem>

using namespace Acorex;

//for files ending in .wav, .flac, .mp3, .ogg
bool Utils::AudioFileLoader::ReadAudioFile ( std::string filename, fluid::RealVector& output, double targetSampleRate )
{
    if ( std::filesystem::exists (filename ) )
    {
        std::cerr << "input file " << filename << " does not exist" << std::endl;
        return false;
    }

    if ( filename.find ( ".mp3" ) != std::string::npos || filename.find ( ".ogg" ) != std::string::npos ||
        filename.find ( ".wav" ) != std::string::npos || filename.find ( ".flac" ) != std::string::npos )
    {
        Wave file;

        file = LoadWave ( filename.c_str ( ) );

        if ( IsWaveValid ( file ) )
        {
            std::cerr << "input file " << filename << " could not be opened" << std::endl;
            return false;
        }

        std::vector<float> temp;

        ReadToMono ( temp, file );

        ResampleAudio resampler;
        bool success = resampler.hermiteResampleTo (    temp, (double)file.sampleRate, targetSampleRate, 
                                                        (file.frameCount / file.channels), file.channels );
        if (!success)
        {
            std::cerr << "loading failed due to resampling error, file: " << filename << std::endl;
            return false;
        }
        output.resize ( temp.size ( ) );
        std::copy ( temp.begin ( ), temp.end ( ), output.data ( ) );
    }
    else
    {
        std::cerr << "input file " << filename << " is not supported" << std::endl;
		return false;
    }

    return true;
}

void Utils::AudioFileLoader::ReadToMono ( std::vector<float>& output, Wave& file )
{
    size_t numChannels = file.channels;
    size_t numSamples = file.frameCount / numChannels;
    output.resize ( numSamples );

    float* samples = LoadWaveSamples ( file );

    if ( numChannels == 1 )
    {
        output.assign(samples, samples + numSamples);
    }
    else
    {
        std::fill ( output.begin ( ), output.end ( ), 0 );

#pragma omp parallel for
        for ( size_t sample = 0; sample < numSamples; sample++ )
        {
            for ( size_t channel = 0; channel < numChannels; channel++ )
            {
                output[sample] += samples[sample * numChannels + channel];
            }
            output[sample] /= numChannels;
        }
    }

    UnloadWaveSamples(samples);
}