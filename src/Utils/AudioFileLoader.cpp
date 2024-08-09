#include "./AudioFileLoader.h"
#include "ofLog.h"

using namespace Acorex;

bool Utils::AudioFileLoader::ReadAudioFile ( std::string filename, fluid::RealVector& output, double& sampleRate )
{
    //if file ends in .wav, .aiff, .flac - use htl::in_audio_file
    //if file ends in .mp3, .ogg - use ofxAudioFile

    if ( !ofFile::doesFileExist ( filename ) )
    {
        ofLogError ( "GenAnalysis" ) << "input file " << filename << " does not exist";
        return false;
    }

    if ( filename.find ( ".wav" ) != std::string::npos || filename.find ( ".aiff" ) != std::string::npos || filename.find ( ".flac" ) != std::string::npos )
    {
        htl::in_audio_file file ( filename.c_str ( ) );

        if ( !file.is_open ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << filename << " could not be opened";
            return false;
        }

        if ( file.is_error ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << filename << " is not supported";
            return false;
        }

        output.resize ( file.frames ( ) );
        sampleRate = file.sampling_rate ( );

        ReadToMono ( output, file );
    }
    else if ( filename.find ( ".mp3" ) != std::string::npos || filename.find ( ".ogg" ) != std::string::npos )
    {
        ofxAudioFile file;
        file.load ( filename );

        if ( !file.loaded ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << filename << " could not be opened";
            return false;
        }

        output.resize ( file.length ( ) );
        sampleRate = file.samplerate ( );

        ReadToMono ( output, file );
    }

    return true;
}

void Utils::AudioFileLoader::ReadToMono ( fluid::RealVector& output, htl::in_audio_file& file )
{
    int numChannels = file.channels ( );
    int numSamples = file.frames ( );

    if ( numChannels == 1 )
    {
        file.read_channel ( output.data ( ), numSamples, 0 );
        return;
    }

    std::fill ( output.begin ( ), output.end ( ), 0 );

    std::vector<std::vector<double>> allChannels ( numChannels, std::vector<double> ( numSamples ) );

    for ( int channel = 0; channel < numChannels; channel++ )
    {
        file.read_channel ( allChannels[channel].data ( ), numSamples, channel );
    }

#pragma omp parallel for
    for ( int sample = 0; sample < numSamples; sample++ )
    {
        for ( int channel = 0; channel < numChannels; channel++ )
        {
            output[sample] += allChannels[channel][sample];
        }
        output[sample] /= numChannels;
    }

}

void Utils::AudioFileLoader::ReadToMono ( fluid::RealVector& output, ofxAudioFile& file )
{
    int numChannels = file.channels ( );
    int numSamples = file.length ( );

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
