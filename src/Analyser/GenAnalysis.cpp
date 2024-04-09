#include "./GenAnalysis.h"
#include <ofLog.h>
#include <omp.h>

#ifndef DATA_CHANGE_CHECK_8
#error "Check if dataset is still used correctly"
#endif

using namespace Acorex;

int Analyser::GenAnalysis::ProcessFiles ( Utils::DataSet& dataset )
{  
    if ( dataset.analysisSettings.bTime )
    {
        dataset.time.samples.clear ( );
        dataset.time.seconds.clear ( );
        dataset.time.raw.clear ( );
    }
	else
	{
		dataset.stats.raw.clear ( );
	}

    int analysedFileIndex = 0;
    std::vector<std::string> analysedFiles;

    fluid::index numPitchDimensions = dataset.analysisSettings.bPitch       ? 2 : 0;
    fluid::index numLoudnessDimensions = dataset.analysisSettings.bLoudness ? 2 : 0;
    fluid::index numShapeDimensions = dataset.analysisSettings.bShape       ? 7 : 0;
    fluid::index numMFCCDimensions = dataset.analysisSettings.bMFCC         ? dataset.analysisSettings.nCoefs : 0;

    fluid::index numDimensions = numPitchDimensions + numLoudnessDimensions + numShapeDimensions + numMFCCDimensions;

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

    for ( int fileIndex = 0; fileIndex < dataset.fileList.size ( ); fileIndex++ )
    {
        const char* inputFileName = dataset.fileList[fileIndex].c_str ( );
        htl::in_audio_file file ( inputFileName );

        if ( !file.is_open ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << inputFileName << " could not be opened";
            continue;
        }

        if ( file.is_error ( ) )
        {
            ofLogError ( "GenAnalysis" ) << "input file " << inputFileName << " is not supported";
            continue;
        }

        auto samplingRate = file.sampling_rate ( );
        fluid::RealVector in ( file.frames ( ) );
        ReadMono ( in, file );

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
            dataset.time.samples.push_back ( std::vector<double> ( ) );
            dataset.time.seconds.push_back ( std::vector<double> ( ) );
            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                dataset.time.samples[analysedFileIndex].push_back ( frameIndex * hopSize );
                dataset.time.seconds[analysedFileIndex].push_back ( (frameIndex * hopSize) / samplingRate );
            }

            std::vector<std::vector<double>> allVectors ( nFrames );

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
        analysedFiles.push_back ( inputFileName );
    }

    dataset.fileList.clear ( );
    dataset.fileList = analysedFiles;

    return analysedFileIndex;
}

void Analyser::GenAnalysis::ReadMono ( fluid::RealVector& output, htl::in_audio_file& file )
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

void Analyser::GenAnalysis::Push7Stats ( fluid::RealVector& stats, std::vector<std::vector<double>> fileData, int numDimensions )
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