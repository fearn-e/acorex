#pragma once

#include "Corpus/Analyse.h"
#include <ofLog.h>

int acorex::corpus::Analyse::ProcessFiles ( std::vector<std::string>& files, fluid::FluidDataSet<std::string, double, 1>& dataset, const std::vector<corpus::Metadata>& metaset )
{
    int filesFailed = 0;

    bool timeDimension = false;
    bool analysisPitch = false; bool analysisLoudness = false; bool analysisShape = false; bool analysisMFCC = false;
    fluid::index windowSize = 0; fluid::index hopFraction = 0;
    fluid::index nBands = 0; fluid::index nCoefs = 0; fluid::index minFreq = 0; fluid::index maxFreq = 0;

    for ( auto& meta : metaset )
    {
        switch ( meta.key )
        {
        case META_TIME_DIMENSION:
            timeDimension = meta.boolValue;
            break;
        case META_ANALYSIS_PITCH:
            analysisPitch = meta.boolValue;
            break;
        case META_ANALYSIS_LOUDNESS:
            analysisLoudness = meta.boolValue;
            break;
        case META_ANALYSIS_SHAPE:
            analysisShape = meta.boolValue;
            break;
        case META_ANALYSIS_MFCC:
            analysisMFCC = meta.boolValue;
            break;
        case META_WINDOW_FFT_SIZE:
            windowSize = meta.intValue;
            break;
        case META_HOP_FRACTION:
            hopFraction = meta.intValue;
            break;
        case META_N_BANDS:
            nBands = meta.intValue;
            break;
        case META_N_COEFS:
            nCoefs = meta.intValue;
            break;
        case META_MIN_FREQ:
            minFreq = meta.intValue;
            break;
        case META_MAX_FREQ:
            maxFreq = meta.intValue;
            break;
		}
    }
    
    fluid::index numTimeDimensions = timeDimension ? 2 : 0;
    fluid::index numPitchDimensions = analysisPitch ? 2 : 0;
    fluid::index numLoudnessDimensions = analysisLoudness ? 2 : 0;
    fluid::index numShapeDimensions = analysisShape ? 7 : 0;
    fluid::index numMFCCDimensions = analysisMFCC ? nCoefs : 0;

    if ( !timeDimension )
    {
        numPitchDimensions *= 7;
        numLoudnessDimensions *= 7;
        numShapeDimensions *= 7;
        numMFCCDimensions *= 7;
    }

    fluid::index numDimensions = numTimeDimensions + numPitchDimensions + numLoudnessDimensions + numShapeDimensions + numMFCCDimensions;

    dataset.resize ( numDimensions );

    fluid::index fftSize = windowSize;
    fluid::index nBins = fftSize / 2 + 1;
    fluid::index hopSize = windowSize / hopFraction;
    fluid::index halfWindow = windowSize / 2;

    for ( int fileIndex = 0; fileIndex < files.size ( ); fileIndex++ )
    {
        const char* inputFileName = files[fileIndex].c_str ( );
        htl::in_audio_file file ( inputFileName );

        if ( !file.is_open ( ) )
        {
            ofLogError ( "Analyse" ) << "input file " << inputFileName << " could not be opened";
            filesFailed++;
            continue;
        }

        if ( file.is_error ( ) )
        {
            ofLogError ( "Analyse" ) << "input file " << inputFileName << " is not supported";
            filesFailed++;
            continue;
        }

        fluid::index nSamples = file.frames ( );
        auto samplingRate = file.sampling_rate ( );

        fluid::algorithm::STFT stft { windowSize, fftSize, hopSize };
        fluid::algorithm::MelBands bands { nBands, fftSize };
        fluid::algorithm::DCT dct { nBands, nCoefs };
        fluid::algorithm::YINFFT yin { nBins, fluid::FluidDefaultAllocator ( ) };
        fluid::algorithm::SpectralShape shape ( fluid::FluidDefaultAllocator ( ) );
        fluid::algorithm::Loudness loudness { windowSize };
        fluid::algorithm::MultiStats stats;

        bands.init ( minFreq, maxFreq, nBands, nBins, samplingRate, windowSize );
        dct.init ( nBands, nCoefs );
        stats.init ( 0, 0, 50, 100 );
        loudness.init ( windowSize, samplingRate );

        fluid::RealVector in ( nSamples );
        file.read_channel ( in.data ( ), nSamples, 0 );
        fluid::RealVector padded ( in.size ( ) + windowSize + hopSize );
        fluid::index      nFrames = floor ( (padded.size ( ) - windowSize) / hopSize );
        fluid::RealMatrix pitchMat ( nFrames, 2 );
        fluid::RealMatrix loudnessMat ( nFrames, 2 );
        fluid::RealMatrix mfccMat ( nFrames, nCoefs );
        fluid::RealMatrix shapeMat ( nFrames, 7 );
        std::fill ( padded.begin ( ), padded.end ( ), 0 );
        padded ( fluid::Slice ( halfWindow, in.size ( ) ) ) <<= in;

        for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
        {
            fluid::RealVector     magnitude ( nBins );
            fluid::RealVectorView window = padded ( fluid::Slice ( frameIndex * hopSize, windowSize ) );

            if ( analysisPitch || analysisShape || analysisMFCC )
            {
                fluid::ComplexVector  frame ( nBins );
                stft.processFrame ( window, frame );
                stft.magnitude ( frame, magnitude );
            }

            if ( analysisPitch )
            {
                fluid::RealVector     pitch ( 2 );
                yin.processFrame ( magnitude, pitch, minFreq, maxFreq, samplingRate );
                pitchMat.row ( frameIndex ) <<= pitch;
            }

            if ( analysisLoudness )
            {
                fluid::RealVector     loudnessDesc ( 2 );
                loudness.processFrame ( window, loudnessDesc, true, true );
                loudnessMat.row ( frameIndex ) <<= loudnessDesc;
            }

            if ( analysisShape )
            {
                fluid::RealVector     shapeDesc ( 7 );
                shape.processFrame ( magnitude, shapeDesc, samplingRate, 0, -1, 0.95, false, false, fluid::FluidDefaultAllocator ( ) );
                shapeMat.row ( frameIndex ) <<= shapeDesc;
			}

            if ( analysisMFCC )
            {
                fluid::RealVector     mfccs ( nCoefs );
                fluid::RealVector     mels ( nBands );
                bands.processFrame ( magnitude, mels, false, false, true, fluid::FluidDefaultAllocator ( ) );
                dct.processFrame ( mels, mfccs );
                mfccMat.row ( frameIndex ) <<= mfccs;
            }
        }

        if ( timeDimension )
        {
            int maxDigits = (std::to_string ( nFrames )).length ( );
            std::string curFileName = files[fileIndex];

            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                fluid::RealVector allVectors ( numDimensions );

                fluid::index currentDimTracker = 0;

                if ( timeDimension )
                {
                    double sampleIndex = frameIndex * hopSize;
					allVectors ( fluid::Slice ( currentDimTracker, numTimeDimensions ) ) <<= fluid::RealVector { sampleIndex, sampleIndex / samplingRate };
					currentDimTracker += numTimeDimensions;
				}

                if ( analysisPitch )
                {
                    allVectors ( fluid::Slice ( currentDimTracker, numPitchDimensions ) ) <<= pitchMat.row ( frameIndex );
                    currentDimTracker += numPitchDimensions;
                }

                if ( analysisLoudness )
                {
                    allVectors ( fluid::Slice ( currentDimTracker, numLoudnessDimensions ) ) <<= loudnessMat.row ( frameIndex );
                    currentDimTracker += numLoudnessDimensions;
                }

                if ( analysisShape )
                {
					allVectors ( fluid::Slice ( currentDimTracker, numShapeDimensions ) ) <<= shapeMat.row ( frameIndex );
					currentDimTracker += numShapeDimensions;
				}

                if ( analysisMFCC )
                {
					allVectors ( fluid::Slice ( currentDimTracker, numMFCCDimensions ) ) <<= mfccMat.row ( frameIndex );
					currentDimTracker += numMFCCDimensions;
				}

                std::string indexString = std::to_string ( frameIndex );
                int curDigits = indexString.length ( );
                std::string fullIndexString = std::string ( 1, '#' ) + std::string ( maxDigits - std::min ( maxDigits, curDigits ), '0' ) + indexString;

                dataset.add ( curFileName + fullIndexString, allVectors );
            }
        }
        else
        {
            fluid::RealVector pitchStats = ComputeStats ( pitchMat, stats );
            fluid::RealVector loudnessStats = ComputeStats ( loudnessMat, stats );
            fluid::RealVector shapeStats = ComputeStats ( shapeMat, stats );
            fluid::RealVector mfccStats = ComputeStats ( mfccMat, stats );

            fluid::RealVector allStats ( numDimensions );

            fluid::index currentDimTracker = 0;

            if ( analysisPitch )
            {
				allStats ( fluid::Slice ( currentDimTracker, numPitchDimensions ) ) <<= pitchStats;
			}

            if ( analysisLoudness )
            {
                allStats ( fluid::Slice ( currentDimTracker, numLoudnessDimensions ) ) <<= loudnessStats;
            }

            if ( analysisShape )
            {
				allStats ( fluid::Slice ( currentDimTracker, numShapeDimensions ) ) <<= shapeStats;
			}

            if ( analysisMFCC )
            {
                allStats ( fluid::Slice ( currentDimTracker, numMFCCDimensions ) ) <<= mfccStats;
            }

            dataset.add ( inputFileName, allStats );
        }
    }

    return filesFailed;
}

fluid::RealVector acorex::corpus::Analyse::ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats )
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