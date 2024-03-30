#pragma once

#include "Corpus/Analyse.h"
#include <ofLog.h>

#ifndef DATA_CHANGE_CHECK_3
#error "Check if dataset is still used correctly"
#endif

int AcorexCorpus::Analyse::ProcessFiles ( AcorexCorpus::DataSet& dataset )
{  
    int analysedFileIndex = 0;
    std::vector<std::string> analysedFiles;

    fluid::index numPitchDimensions = dataset.analysisSettings.bPitch       ? 2 : 0;
    fluid::index numLoudnessDimensions = dataset.analysisSettings.bLoudness ? 2 : 0;
    fluid::index numShapeDimensions = dataset.analysisSettings.bShape       ? 7 : 0;
    fluid::index numMFCCDimensions = dataset.analysisSettings.bMFCC         ? dataset.analysisSettings.nCoefs : 0;

    fluid::index numDimensions = numPitchDimensions + numLoudnessDimensions + numShapeDimensions + numMFCCDimensions;

    if ( dataset.currentDimensionCount > 0 )
    {
        assert ( numDimensions == dataset.currentDimensionCount );
    }
    else
    {
		dataset.currentDimensionCount = numDimensions;
	}

    fluid::index nBins = dataset.analysisSettings.windowFFTSize / 2 + 1;
    fluid::index hopSize = dataset.analysisSettings.windowFFTSize / dataset.analysisSettings.hopFraction;
    fluid::index halfWindow = dataset.analysisSettings.windowFFTSize / 2;
    
    if ( dataset.isTimeAnalysis )
    {
        int reserveSize = 0;
        for ( auto each : dataset.fileList )
        {
            htl::in_audio_file file ( each.c_str ( ) );
            reserveSize += floor ( (file.frames ( ) + hopSize) / hopSize );
        }
        reserveSize *= numDimensions;
        dataset.data.reserve ( reserveSize );
    }
    else
    {
        int reserveSize = dataset.fileList.size ( ) * numDimensions;
        dataset.meanData.reserve ( reserveSize );
        dataset.standardDeviationData.reserve ( reserveSize );
        dataset.skewnessData.reserve ( reserveSize );
        dataset.kurtosisData.reserve ( reserveSize );
        dataset.lowerQuartileData.reserve ( reserveSize );
        dataset.medianData.reserve ( reserveSize );
        dataset.upperQuartileData.reserve ( reserveSize );
    }

    for ( int fileIndex = 0; fileIndex < dataset.fileList.size ( ); fileIndex++ )
    {
        const char* inputFileName = dataset.fileList[fileIndex].c_str ( );
        htl::in_audio_file file ( inputFileName );

        if ( !file.is_open ( ) )
        {
            ofLogError ( "Analyse" ) << "input file " << inputFileName << " could not be opened";
            continue;
        }

        if ( file.is_error ( ) )
        {
            ofLogError ( "Analyse" ) << "input file " << inputFileName << " is not supported";
            continue;
        }

        fluid::index nSamples = file.frames ( );
        auto samplingRate = file.sampling_rate ( );

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

        fluid::RealVector in ( nSamples );
        file.read_channel ( in.data ( ), nSamples, 0 );
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

        if ( dataset.isTimeAnalysis )
        {
            /*
            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                dataset.timePointsSamples[fileIndex].push_back ( frameIndex * hopSize );
                dataset.timePointsSeconds[fileIndex].push_back ( (frameIndex * hopSize) / samplingRate );

                fluid::RealVector allVectors ( numDimensions );
                int currentDimTracker = 0;

                if ( dataset.analysisSettings.bPitch )
                {
                    allVectors ( fluid::Slice ( currentDimTracker, numPitchDimensions ) ) <<= pitchMat.row ( frameIndex );
                    currentDimTracker += numPitchDimensions;
                }

                if ( dataset.analysisSettings.bLoudness )
                {
                    allVectors ( fluid::Slice ( currentDimTracker, numLoudnessDimensions ) ) <<= loudnessMat.row ( frameIndex );
                    currentDimTracker += numLoudnessDimensions;
                }

                if ( dataset.analysisSettings.bShape )
                {
					allVectors ( fluid::Slice ( currentDimTracker, numShapeDimensions ) ) <<= shapeMat.row ( frameIndex );
					currentDimTracker += numShapeDimensions;
				}

                if ( dataset.analysisSettings.bMFCC )
                {
					allVectors ( fluid::Slice ( currentDimTracker, numMFCCDimensions ) ) <<= mfccMat.row ( frameIndex );
					currentDimTracker += numMFCCDimensions;
				}

                dataset.data[fileIndex][frameIndex] = std::vector<double> ( allVectors.begin ( ), allVectors.end ( ) );
            }
            */

            dataset.timePointsSamples.push_back ( std::vector<double> ( ) );
            for ( int frameIndex = 0; frameIndex < nFrames; frameIndex++ )
            {
                dataset.timePointsSamples[analysedFileIndex].push_back ( frameIndex * hopSize );
                dataset.timePointsSeconds[analysedFileIndex].push_back ( (frameIndex * hopSize) / samplingRate );
            }

            fluid::RealMatrix allVectors ( nFrames, numDimensions );
            int currentDimTracker = 0;

			if ( dataset.analysisSettings.bPitch )
			{
				allVectors ( fluid::Slice ( 0, numPitchDimensions ) ) <<= pitchMat;
				currentDimTracker += numPitchDimensions;
			}

            if ( dataset.analysisSettings.bLoudness )
			{
				allVectors ( fluid::Slice ( currentDimTracker, numLoudnessDimensions ) ) <<= loudnessMat;
				currentDimTracker += numLoudnessDimensions;
			}

            if ( dataset.analysisSettings.bShape )
            {
                allVectors ( fluid::Slice ( currentDimTracker, numShapeDimensions ) ) <<= shapeMat;
                currentDimTracker += numShapeDimensions;
            }

            if ( dataset.analysisSettings.bMFCC )
			{
				allVectors ( fluid::Slice ( currentDimTracker, numMFCCDimensions ) ) <<= mfccMat;
				currentDimTracker += numMFCCDimensions;
			}

            dataset.data.push_back ( std::vector<std::vector<double>> ( allVectors.begin ( ), allVectors.end ( ) ) );
        }
        else
        {
            fluid::RealVector pitchStats = ComputeStats ( pitchMat, stats );
            fluid::RealVector loudnessStats = ComputeStats ( loudnessMat, stats );
            fluid::RealVector shapeStats = ComputeStats ( shapeMat, stats );
            fluid::RealVector mfccStats = ComputeStats ( mfccMat, stats );

            dataset.meanData.push_back ( std::vector<double> ( ) );
            dataset.standardDeviationData.push_back ( std::vector<double> ( ) );
            dataset.skewnessData.push_back ( std::vector<double> ( ) );
            dataset.kurtosisData.push_back ( std::vector<double> ( ) );
            dataset.lowerQuartileData.push_back ( std::vector<double> ( ) );
            dataset.medianData.push_back ( std::vector<double> ( ) );
            dataset.upperQuartileData.push_back ( std::vector<double> ( ) );

            if ( dataset.analysisSettings.bPitch ) { Push7Stats ( analysedFileIndex, pitchStats, dataset, numPitchDimensions ); }

            if ( dataset.analysisSettings.bLoudness ) { Push7Stats ( analysedFileIndex, loudnessStats, dataset, numLoudnessDimensions ); }

            if ( dataset.analysisSettings.bShape ) { Push7Stats ( analysedFileIndex, shapeStats, dataset, numShapeDimensions ); }

            if ( dataset.analysisSettings.bMFCC ) { Push7Stats ( analysedFileIndex, mfccStats, dataset, numMFCCDimensions ); }
        }

        analysedFileIndex++;
        analysedFiles.push_back ( inputFileName );
    }

    dataset.fileList.clear ( );
    dataset.fileList = analysedFiles;

    return analysedFileIndex;
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

void AcorexCorpus::Analyse::Push7Stats ( int index, fluid::RealVector& stats, AcorexCorpus::DataSet& dataset, int numDimensions )
{
    numDimensions *= 7;

    for ( fluid::index i = 0; i < numDimensions; i += 7 )
    {
        dataset.meanData[index].push_back ( stats[i] );
        dataset.standardDeviationData[index].push_back ( stats[i + 1] );
        dataset.skewnessData[index].push_back ( stats[i + 2] );
        dataset.kurtosisData[index].push_back ( stats[i + 3] );
        dataset.lowerQuartileData[index].push_back ( stats[i + 4] );
        dataset.medianData[index].push_back ( stats[i + 5] );
        dataset.upperQuartileData[index].push_back ( stats[i + 6] );
    }
}