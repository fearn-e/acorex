#pragma once

#include "Corpus/Analyse.h"
#include <ofLog.h>

int AcorexCorpus::Analyse::ProcessFiles ( std::vector<std::string>& files, fluid::FluidDataSet<std::string, double, 1>& dataset, const AcorexCorpus::MetaSetStruct& metaset )
{
    int filesFailed = 0;
    
    fluid::index numTimeDimensions = metaset.isTimeAnalysis ? 2 : 0;
    fluid::index numPitchDimensions = metaset.analysisPitch ? 2 : 0;
    fluid::index numLoudnessDimensions = metaset.analysisLoudness ? 2 : 0;
    fluid::index numShapeDimensions = metaset.analysisShape ? 7 : 0;
    fluid::index numMFCCDimensions = metaset.analysisMFCC ? metaset.nCoefs : 0;

    if ( !metaset.isTimeAnalysis )
    {
        numPitchDimensions *= 7;
        numLoudnessDimensions *= 7;
        numShapeDimensions *= 7;
        numMFCCDimensions *= 7;
    }

    fluid::index numDimensions = numTimeDimensions + numPitchDimensions + numLoudnessDimensions + numShapeDimensions + numMFCCDimensions;

    assert ( numDimensions == metaset.currentDimensionCount );

    dataset.resize ( numDimensions );

    fluid::index nBins = metaset.windowFFTSize / 2 + 1;
    fluid::index hopSize = metaset.windowFFTSize / metaset.hopFraction;
    fluid::index halfWindow = metaset.windowFFTSize / 2;

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

        fluid::algorithm::STFT stft { metaset.windowFFTSize, metaset.windowFFTSize, hopSize };
        fluid::algorithm::MelBands bands { metaset.nBands, metaset.windowFFTSize };
        fluid::algorithm::DCT dct { metaset.nBands, metaset.nCoefs };
        fluid::algorithm::YINFFT yin { nBins, fluid::FluidDefaultAllocator ( ) };
        fluid::algorithm::SpectralShape shape ( fluid::FluidDefaultAllocator ( ) );
        fluid::algorithm::Loudness loudness { metaset.windowFFTSize };
        fluid::algorithm::MultiStats stats;

        bands.init ( metaset.minFreq, metaset.maxFreq, metaset.nBands, nBins, samplingRate, metaset.windowFFTSize );
        dct.init ( metaset.nBands, metaset.nCoefs );
        stats.init ( 0, 0, 50, 100 );
        loudness.init ( metaset.windowFFTSize, samplingRate );

        fluid::RealVector in ( nSamples );
        file.read_channel ( in.data ( ), nSamples, 0 );
        fluid::RealVector padded ( in.size ( ) + metaset.windowFFTSize + hopSize );
        fluid::index      nFrames = floor ( (padded.size ( ) - metaset.windowFFTSize) / hopSize );
        fluid::RealMatrix pitchMat ( nFrames, 2 );
        fluid::RealMatrix loudnessMat ( nFrames, 2 );
        fluid::RealMatrix mfccMat ( nFrames, metaset.nCoefs );
        fluid::RealMatrix shapeMat ( nFrames, 7 );
        std::fill ( padded.begin ( ), padded.end ( ), 0 );
        padded ( fluid::Slice ( halfWindow, in.size ( ) ) ) <<= in;

        for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
        {
            fluid::RealVector     magnitude ( nBins );
            fluid::RealVectorView window = padded ( fluid::Slice ( frameIndex * hopSize, metaset.windowFFTSize ) );

            if ( metaset.analysisPitch || metaset.analysisShape || metaset.analysisMFCC )
            {
                fluid::ComplexVector  frame ( nBins );
                stft.processFrame ( window, frame );
                stft.magnitude ( frame, magnitude );
            }

            if ( metaset.analysisPitch )
            {
                fluid::RealVector     pitch ( 2 );
                yin.processFrame ( magnitude, pitch, metaset.minFreq, metaset.maxFreq, samplingRate );
                pitchMat.row ( frameIndex ) <<= pitch;
            }

            if ( metaset.analysisLoudness )
            {
                fluid::RealVector     loudnessDesc ( 2 );
                loudness.processFrame ( window, loudnessDesc, true, true );
                loudnessMat.row ( frameIndex ) <<= loudnessDesc;
            }

            if ( metaset.analysisShape )
            {
                fluid::RealVector     shapeDesc ( 7 );
                shape.processFrame ( magnitude, shapeDesc, samplingRate, 0, -1, 0.95, false, false, fluid::FluidDefaultAllocator ( ) );
                shapeMat.row ( frameIndex ) <<= shapeDesc;
			}

            if ( metaset.analysisMFCC )
            {
                fluid::RealVector     mfccs ( metaset.nCoefs );
                fluid::RealVector     mels ( metaset.nBands );
                bands.processFrame ( magnitude, mels, false, false, true, fluid::FluidDefaultAllocator ( ) );
                dct.processFrame ( mels, mfccs );
                mfccMat.row ( frameIndex ) <<= mfccs;
            }
        }

        if ( metaset.isTimeAnalysis )
        {
            int maxDigits = (std::to_string ( nFrames )).length ( );
            std::string curFileName = files[fileIndex];

            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                fluid::RealVector allVectors ( numDimensions );

                fluid::index currentDimTracker = 0;

                if ( metaset.isTimeAnalysis )
                {
                    double sampleIndex = frameIndex * hopSize;
					allVectors ( fluid::Slice ( currentDimTracker, numTimeDimensions ) ) <<= fluid::RealVector { sampleIndex, sampleIndex / samplingRate };
					currentDimTracker += numTimeDimensions;
				}

                if ( metaset.analysisPitch )
                {
                    allVectors ( fluid::Slice ( currentDimTracker, numPitchDimensions ) ) <<= pitchMat.row ( frameIndex );
                    currentDimTracker += numPitchDimensions;
                }

                if ( metaset.analysisLoudness )
                {
                    allVectors ( fluid::Slice ( currentDimTracker, numLoudnessDimensions ) ) <<= loudnessMat.row ( frameIndex );
                    currentDimTracker += numLoudnessDimensions;
                }

                if ( metaset.analysisShape )
                {
					allVectors ( fluid::Slice ( currentDimTracker, numShapeDimensions ) ) <<= shapeMat.row ( frameIndex );
					currentDimTracker += numShapeDimensions;
				}

                if ( metaset.analysisMFCC )
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

            if ( metaset.analysisPitch )
            {
				allStats ( fluid::Slice ( currentDimTracker, numPitchDimensions ) ) <<= pitchStats;
                currentDimTracker += numPitchDimensions;
			}

            if ( metaset.analysisLoudness )
            {
                allStats ( fluid::Slice ( currentDimTracker, numLoudnessDimensions ) ) <<= loudnessStats;
                currentDimTracker += numLoudnessDimensions;
            }

            if ( metaset.analysisShape )
            {
				allStats ( fluid::Slice ( currentDimTracker, numShapeDimensions ) ) <<= shapeStats;
                currentDimTracker += numShapeDimensions;
			}

            if ( metaset.analysisMFCC )
            {
                allStats ( fluid::Slice ( currentDimTracker, numMFCCDimensions ) ) <<= mfccStats;
                currentDimTracker += numMFCCDimensions;
            }

            dataset.add ( inputFileName, allStats );
        }
    }

    return filesFailed;
}

fluid::RealVector AcorexCorpus::Analyse::ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats )
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