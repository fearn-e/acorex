#include "./GenAnalysis.h"
#include <ofLog.h>

#if __has_include(<omp.h>)
#include <omp.h>
#endif

#ifndef DATA_CHANGE_CHECK_1
#error "Check if dataset is still used correctly"
#endif

using namespace Acorex;

int Analyser::GenAnalysis::ProcessFiles ( Utils::DataSet& dataset )
{  
    ofLogNotice ( "GenAnalysis" ) << "Calculating file lengths...";
    double fileLengthSumTracker = 0;
    double fileLengthSumTotal = 0;

    {
        unsigned long long int sampleTotal = 0;
#pragma omp parallel for reduction(+:sampleTotal)
        for ( int i = 0; i < dataset.fileList.size ( ); i++ )
        {
            double temp;
            fluid::RealVector in ( 0 );
            bool success = ReadFile ( dataset.fileList[i], in, temp );
            if ( success ) { sampleTotal += in.size ( ); }
        }
        fileLengthSumTotal = sampleTotal;
    }

    ofLogNotice ( "GenAnalysis" ) << "Total sample count: " << fileLengthSumTotal;

    if ( dataset.analysisSettings.bTime )
    {
        dataset.time.raw.clear ( );
    }
	else
	{
		dataset.stats.raw.clear ( );
	}

    int analysedFileIndex = 0;
    std::vector<std::string> analysedFiles;

    fluid::index numTimeDimensions = dataset.analysisSettings.bTime         ? 1 : 0;
    fluid::index numPitchDimensions = dataset.analysisSettings.bPitch       ? 2 : 0;
    fluid::index numLoudnessDimensions = dataset.analysisSettings.bLoudness ? 2 : 0;
    fluid::index numShapeDimensions = dataset.analysisSettings.bShape       ? 7 : 0;
    fluid::index numMFCCDimensions = dataset.analysisSettings.bMFCC         ? dataset.analysisSettings.nCoefs : 0;

    fluid::index numDimensions = numTimeDimensions + numPitchDimensions + numLoudnessDimensions + numShapeDimensions + numMFCCDimensions;

    if ( dataset.analysisSettings.currentDimensionCount > 0 )
    {
        assert ( numDimensions == dataset.analysisSettings.currentDimensionCount );
    }
    else
    {
		dataset.analysisSettings.currentDimensionCount = numDimensions;
	}

    fluid::index nBins = dataset.analysisSettings.windowFFTSize / 2 + 1;
    fluid::index hopSize = dataset.analysisSettings.windowFFTSize / dataset.analysisSettings.hopFraction;
    fluid::index halfWindow = dataset.analysisSettings.windowFFTSize / 2;
    
    //if ( dataset.analysisSettings.bTime )
    //{
    //    int reserveSize = 0;
    //    for ( auto each : dataset.fileList )
    //    {
    //        htl::in_audio_file file ( each.c_str ( ) );
    //        reserveSize += floor ( (file.frames ( ) + hopSize) / hopSize );
    //    }
    //    reserveSize *= numDimensions;
    //    dataset.time.raw.reserve ( reserveSize ); //TODO - double check this works as expected
    //}
    //else
    //{
    //    int reserveSize = dataset.fileList.size ( ) * numDimensions * DATA_NUM_STATS;
    //    dataset.stats.raw.reserve ( reserveSize );
    //}

    double startTime = ofGetElapsedTimef ( );
    for ( int fileIndex = 0; fileIndex < dataset.fileList.size ( ); fileIndex++ )
    {
        double samplingRate = 0;
        fluid::RealVector in ( 0 );
        bool success = ReadFile ( dataset.fileList[fileIndex], in, samplingRate );
        if ( !success ) { continue; }

        fluid::algorithm::STFT stft { dataset.analysisSettings.windowFFTSize, dataset.analysisSettings.windowFFTSize, hopSize };
        fluid::algorithm::MelBands bands { dataset.analysisSettings.nBands, dataset.analysisSettings.windowFFTSize };
        fluid::algorithm::DCT dct { dataset.analysisSettings.nBands, dataset.analysisSettings.nCoefs };
        fluid::algorithm::YINFFT yin { nBins, fluid::FluidDefaultAllocator ( ) };
        fluid::algorithm::SpectralShape shape ( fluid::FluidDefaultAllocator ( ) );
        fluid::algorithm::Loudness loudness { dataset.analysisSettings.windowFFTSize };
        fluid::algorithm::MultiStats stats;

        bands.init ( dataset.analysisSettings.minFreq, dataset.analysisSettings.maxFreq, 
                    dataset.analysisSettings.nBands, nBins, samplingRate, dataset.analysisSettings.windowFFTSize );
        dct.init ( dataset.analysisSettings.nBands, dataset.analysisSettings.nCoefs );
        stats.init ( 0, 0, 50, 100 );
        loudness.init ( dataset.analysisSettings.windowFFTSize, samplingRate );

        fluid::RealVector padded ( in.size ( ) + dataset.analysisSettings.windowFFTSize + hopSize );
        fluid::index      nFrames = floor ( (padded.size ( ) - dataset.analysisSettings.windowFFTSize) / hopSize );
        fluid::RealMatrix pitchMat ( nFrames, 2 );
        fluid::RealMatrix loudnessMat ( nFrames, 2 );
        fluid::RealMatrix mfccMat ( nFrames, dataset.analysisSettings.nCoefs );
        fluid::RealMatrix shapeMat ( nFrames, 7 );
        std::fill ( padded.begin ( ), padded.end ( ), 0 );
        padded ( fluid::Slice ( halfWindow, in.size ( ) ) ) <<= in;

        for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
        {
            fluid::RealVector     magnitude ( nBins );
            fluid::RealVectorView window = padded ( fluid::Slice ( frameIndex * hopSize, dataset.analysisSettings.windowFFTSize ) );

            if ( dataset.analysisSettings.bPitch || dataset.analysisSettings.bShape || dataset.analysisSettings.bMFCC )
            {
                fluid::ComplexVector  frame ( nBins );
                stft.processFrame ( window, frame );
                stft.magnitude ( frame, magnitude );
            }

            if ( dataset.analysisSettings.bPitch )
            {
                fluid::RealVector     pitch ( 2 );
                yin.processFrame ( magnitude, pitch, dataset.analysisSettings.minFreq, dataset.analysisSettings.maxFreq, samplingRate );
                pitchMat.row ( frameIndex ) <<= pitch;
            }

            if ( dataset.analysisSettings.bLoudness )
            {
                fluid::RealVector     loudnessDesc ( 2 );
                loudness.processFrame ( window, loudnessDesc, true, true );
                loudnessMat.row ( frameIndex ) <<= loudnessDesc;
            }

            if ( dataset.analysisSettings.bShape )
            {
                fluid::RealVector     shapeDesc ( 7 );
                shape.processFrame ( magnitude, shapeDesc, samplingRate, 0, -1, 0.95, false, false, fluid::FluidDefaultAllocator ( ) );
                shapeMat.row ( frameIndex ) <<= shapeDesc;
			}

            if ( dataset.analysisSettings.bMFCC )
            {
                fluid::RealVector     mfccs ( dataset.analysisSettings.nCoefs );
                fluid::RealVector     mels ( dataset.analysisSettings.nBands );
                bands.processFrame ( magnitude, mels, false, false, true, fluid::FluidDefaultAllocator ( ) );
                dct.processFrame ( mels, mfccs );
                mfccMat.row ( frameIndex ) <<= mfccs;
            }
        }

        if ( dataset.analysisSettings.bTime )
        {
            std::vector<std::vector<double>> allVectors ( nFrames );

            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                allVectors[frameIndex].push_back ( frameIndex * hopSize / samplingRate );
            }

			if ( dataset.analysisSettings.bPitch )
			{
                for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
				{
                    for ( int dimIndex = 0; dimIndex < numPitchDimensions; dimIndex++ )
                    {
                        allVectors[frameIndex].push_back ( pitchMat ( frameIndex, dimIndex ) );
                    }
				}
			}

            if ( dataset.analysisSettings.bLoudness )
			{
                for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
                {
                    for ( int dimIndex = 0; dimIndex < numLoudnessDimensions; dimIndex++ )
                    {
                        allVectors[frameIndex].push_back ( loudnessMat ( frameIndex, dimIndex ) );
                    }
                }
			}

            if ( dataset.analysisSettings.bShape )
            {
                for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
				{
					for ( int dimIndex = 0; dimIndex < numShapeDimensions; dimIndex++ )
					{
						allVectors[frameIndex].push_back ( shapeMat ( frameIndex, dimIndex ) );
					}
				}
            }

            if ( dataset.analysisSettings.bMFCC )
			{
                for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
                {
                    for ( int dimIndex = 0; dimIndex < numMFCCDimensions; dimIndex++ )
					{
						allVectors[frameIndex].push_back ( mfccMat ( frameIndex, dimIndex ) );
					}
                }
			}

            dataset.time.raw.push_back ( allVectors );
            dataset.currentPointCount += nFrames;
        }
        else
        {
            fluid::RealVector pitchStats = ComputeStats ( pitchMat, stats );
            fluid::RealVector loudnessStats = ComputeStats ( loudnessMat, stats );
            fluid::RealVector shapeStats = ComputeStats ( shapeMat, stats );
            fluid::RealVector mfccStats = ComputeStats ( mfccMat, stats );

            dataset.stats.raw.push_back ( std::vector<std::vector<double>> ( ) );

            if ( dataset.analysisSettings.bPitch ) { Push7Stats ( pitchStats, dataset.stats.raw[analysedFileIndex], numPitchDimensions ); }

            if ( dataset.analysisSettings.bLoudness ) { Push7Stats ( loudnessStats, dataset.stats.raw[analysedFileIndex], numLoudnessDimensions ); }

            if ( dataset.analysisSettings.bShape ) { Push7Stats ( shapeStats, dataset.stats.raw[analysedFileIndex], numShapeDimensions ); }

            if ( dataset.analysisSettings.bMFCC ) { Push7Stats ( mfccStats, dataset.stats.raw[analysedFileIndex], numMFCCDimensions ); }

            dataset.currentPointCount++;
        }

        analysedFileIndex++;
        analysedFiles.push_back ( dataset.fileList[fileIndex] );

        { // Progress logging
            fileLengthSumTracker += in.size ( );
            double elapsedTime = ofGetElapsedTimef ( ) - startTime;
            double progress = fileLengthSumTracker / fileLengthSumTotal * 100.0f;
            double eta = (elapsedTime / progress) * (100.0f - progress);
            int etaHours = eta / 3600; int etaMinutes = (eta - (etaHours * 3600)) / 60; int etaSeconds = eta - (etaHours * 3600) - (etaMinutes * 60);
            if ( etaHours > 0 )
            {
                ofLogNotice ( "GenAnalysis" ) << "Progress: " << progress << "% | ETA: " << etaHours << "h " << etaMinutes << "m " << etaSeconds << "s | Analysed " << dataset.fileList[fileIndex];
            }
            else if ( etaMinutes > 0 )
            {
                ofLogNotice ( "GenAnalysis" ) << "Progress: " << progress << "% | ETA: " << etaMinutes << "m " << etaSeconds << "s | Analysed " << dataset.fileList[fileIndex];
            }
            else
            {
                ofLogNotice ( "GenAnalysis" ) << "Progress: " << progress << "% | ETA: " << etaSeconds << "s | Analysed " << dataset.fileList[fileIndex];
            }
        }
    }

    dataset.fileList.clear ( );
    dataset.fileList = analysedFiles;

    return analysedFileIndex;
}

bool Analyser::GenAnalysis::ReadFile ( std::string filename, fluid::RealVector& output, double& sampleRate )
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

void Analyser::GenAnalysis::ReadToMono ( fluid::RealVector& output, htl::in_audio_file& file )
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

void Analyser::GenAnalysis::ReadToMono ( fluid::RealVector& output, ofxAudioFile& file )
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

fluid::RealVector Analyser::GenAnalysis::ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats )
{
    fluid::index      dim = matrix.cols ( );
    fluid::RealMatrix tmp ( dim, 7 );
    fluid::RealVector result ( dim * 7 );
    stats.process ( matrix.transpose ( ), tmp );
    for ( int j = 0; j < dim; j++ )
    {
        result ( fluid::Slice ( j * 7, 7 ) ) <<= tmp.row ( j );
    }
    return result;
}

void Analyser::GenAnalysis::Push7Stats ( fluid::RealVector& stats, std::vector<std::vector<double>>& fileData, int numDimensions )
{
    for ( fluid::index dimension = 0; dimension < numDimensions; dimension++ )
    {
        fileData.push_back ( std::vector<double> ( ) );
        int index = fileData.size ( ) - 1;

        for ( fluid::index statistic = 0; statistic < DATA_NUM_STATS; statistic++ )
		{
            int statsIndex = ( dimension * 7 ) + statistic;
            fileData[index].push_back ( stats[statsIndex] );
		}
    }
}