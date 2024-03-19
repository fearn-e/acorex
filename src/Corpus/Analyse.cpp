#pragma once

#include "Corpus/Analyse.h"
#include <ofLog.h>

int corpus::Analyse::ProcessFiles ( std::vector<std::string>& files, fluid::FluidDataSet<std::string, double, 1>& dataset, bool timeDimension)
{
    int filesFailed = 0;

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

        fluid::index nBins = 513;
        fluid::index fftSize = 2 * (nBins - 1);
        fluid::index hopSize = 1024;
        fluid::index windowSize = 1024;
        fluid::index halfWindow = windowSize / 2;
        fluid::index nBands = 40;
        fluid::index nCoefs = 13;
        fluid::index minFreq = 20;
        fluid::index maxFreq = 5000;

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
            fluid::ComplexVector  frame ( nBins );
            fluid::RealVector     magnitude ( nBins );
            fluid::RealVector     mels ( nBands );
            fluid::RealVector     mfccs ( nCoefs );
            fluid::RealVector     pitch ( 2 );
            fluid::RealVector     shapeDesc ( 7 );
            fluid::RealVector     loudnessDesc ( 2 );
            fluid::RealVectorView window = padded ( fluid::Slice ( frameIndex * hopSize, windowSize ) );
            stft.processFrame ( window, frame );
            stft.magnitude ( frame, magnitude );
            bands.processFrame ( magnitude, mels, false, false, true,
                fluid::FluidDefaultAllocator ( ) );
            dct.processFrame ( mels, mfccs );
            mfccMat.row ( frameIndex ) <<= mfccs;
            yin.processFrame ( magnitude, pitch, minFreq, maxFreq, samplingRate );
            pitchMat.row ( frameIndex ) <<= pitch;
            shape.processFrame ( magnitude, shapeDesc, samplingRate, 0, -1, 0.95, false,
                false, fluid::FluidDefaultAllocator ( ) );
            shapeMat.row ( frameIndex ) <<= shapeDesc;
            loudness.processFrame ( window, loudnessDesc, true, true );
            loudnessMat.row ( frameIndex ) <<= loudnessDesc;
        }

        if ( timeDimension )
        {
            int maxDigits = (std::to_string ( nFrames )).length ( );
            std::string curFileName = files[fileIndex];

            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                fluid::RealVector allVectors ( 26 ); // 2 for time (samples, seconds), 2 for pitch, 2 for loudness, 7 for shape, 13 for mfcc

                double sampleIndex = frameIndex * hopSize;

                allVectors ( fluid::Slice ( 0, 2 ) ) <<= fluid::RealVector { sampleIndex, sampleIndex / samplingRate };
                allVectors ( fluid::Slice ( 2, 2 ) ) <<= pitchMat.row ( frameIndex );
                allVectors ( fluid::Slice ( 4, 2 ) ) <<= loudnessMat.row ( frameIndex );
                allVectors ( fluid::Slice ( 6, 7 ) ) <<= shapeMat.row ( frameIndex );
                allVectors ( fluid::Slice ( 13, 13 ) ) <<= mfccMat.row ( frameIndex );

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

            fluid::RealVector allStats ( 168 );

            allStats ( fluid::Slice ( 0, 14 ) ) <<= pitchStats;
            allStats ( fluid::Slice ( 14, 14 ) ) <<= loudnessStats;
            allStats ( fluid::Slice ( 28, 49 ) ) <<= shapeStats;
            allStats ( fluid::Slice ( 77, 91 ) ) <<= mfccStats;

            dataset.add ( inputFileName, allStats );
        }
    }

    return filesFailed;
}

fluid::RealVector corpus::Analyse::ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats )
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