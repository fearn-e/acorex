#pragma once

#include "ofSystemUtils.h"

#include <Eigen/Core>
#include <algorithms/public/DCT.hpp>
#include <algorithms/public/Loudness.hpp>
#include <algorithms/public/MelBands.hpp>
#include <algorithms/public/MultiStats.hpp>
#include <algorithms/public/STFT.hpp>
#include <algorithms/public/SpectralShape.hpp>
#include <algorithms/public/YINFFT.hpp>
#include <audio_file/in_file.hpp>
#include <data/FluidDataSet.hpp>
#include <data/FluidIndex.hpp>
#include <data/FluidJSON.hpp>
#include <data/FluidMemory.hpp>
#include <data/TensorTypes.hpp>
#include <vector>

#include <string>
#include <filesystem>

class AnalyseCorpus {
public:

    std::vector<std::string> SearchDirectory ( const std::string& directory )
    {
        std::vector<std::string> files;

        using namespace std::filesystem;
        for ( const auto& entry : recursive_directory_iterator ( directory ) )
        {
            if ( is_directory ( entry.path ( ) ) ) { continue; }

            if ( !is_regular_file ( entry.path ( ) ) ) { continue; }
            
            if ( entry.path ( ).extension ( ) != ".wav" &&
                entry.path ( ).extension ( ) != ".aiff" &&
                entry.path ( ).extension ( ) != ".flac" &&
                entry.path ( ).extension ( ) != ".ogg" &&
                entry.path ( ).extension ( ) != ".mp3" ) { continue; }

            files.push_back ( entry.path ( ).string ( ) );
        }
        return files;
    }

    void Analyse ( std::vector<std::string>& files, std::string outputFile )
    {
        using namespace fluid;
        using namespace fluid::algorithm;
        using fluid::index;

        const char* outFile = outputFile.c_str ( );

        FluidDataSet<std::string, double, 1> dataset ( 168 );

        for ( int i = 0; i < files.size ( ); i++ )
        {
            const char* inFile = files[i].c_str ( );
            htl::in_audio_file file ( inFile );

            if ( !file.is_open ( ) )
            {
                ofLogError ( "AnalyseCorpus" ) << "input file " << inFile << " could not be opened";
                continue;
            }

            if ( file.is_error ( ) )
            {
                ofLogError ( "AnalyseCorpus" ) << "input file " << inFile << " is not supported";
                continue;
            }

            index nSamples = file.frames ( );
            auto  samplingRate = file.sampling_rate ( );

            index nBins = 513;
            index fftSize = 2 * (nBins - 1);
            index hopSize = 1024;
            index windowSize = 1024;
            index halfWindow = windowSize / 2;
            index nBands = 40;
            index nCoefs = 13;
            index minFreq = 20;
            index maxFreq = 5000;

            STFT          stft { windowSize, fftSize, hopSize };
            MelBands      bands { nBands, fftSize };
            DCT           dct { nBands, nCoefs };
            YINFFT        yin { nBins, FluidDefaultAllocator ( ) };
            SpectralShape shape ( FluidDefaultAllocator ( ) );
            Loudness      loudness { windowSize };
            MultiStats    stats;

            bands.init ( minFreq, maxFreq, nBands, nBins, samplingRate, windowSize );
            dct.init ( nBands, nCoefs );
            stats.init ( 0, 0, 50, 100 );
            loudness.init ( windowSize, samplingRate );

            RealVector in ( nSamples );
            file.read_channel ( in.data ( ), nSamples, 0 );
            RealVector padded ( in.size ( ) + windowSize + hopSize );
            index      nFrames = floor ( (padded.size ( ) - windowSize) / hopSize );
            RealMatrix pitchMat ( nFrames, 2 );
            RealMatrix loudnessMat ( nFrames, 2 );
            RealMatrix mfccMat ( nFrames, nCoefs );
            RealMatrix shapeMat ( nFrames, 7 );
            std::fill ( padded.begin ( ), padded.end ( ), 0 );
            padded ( Slice ( halfWindow, in.size ( ) ) ) <<= in;

            for ( int i = 0; i < nFrames; i++ )
            {
                ComplexVector  frame ( nBins );
                RealVector     magnitude ( nBins );
                RealVector     mels ( nBands );
                RealVector     mfccs ( nCoefs );
                RealVector     pitch ( 2 );
                RealVector     shapeDesc ( 7 );
                RealVector     loudnessDesc ( 2 );
                RealVectorView window = padded ( fluid::Slice ( i * hopSize, windowSize ) );
                stft.processFrame ( window, frame );
                stft.magnitude ( frame, magnitude );
                bands.processFrame ( magnitude, mels, false, false, true,
                    FluidDefaultAllocator ( ) );
                dct.processFrame ( mels, mfccs );
                mfccMat.row ( i ) <<= mfccs;
                yin.processFrame ( magnitude, pitch, minFreq, maxFreq, samplingRate );
                pitchMat.row ( i ) <<= pitch;
                shape.processFrame ( magnitude, shapeDesc, samplingRate, 0, -1, 0.95, false,
                    false, FluidDefaultAllocator ( ) );
                shapeMat.row ( i ) <<= shapeDesc;
                loudness.processFrame ( window, loudnessDesc, true, true );
                loudnessMat.row ( i ) <<= loudnessDesc;

                // for OverTime analysis versions, computestats and add dataset points inside this for loop here
            }

            RealVector pitchStats = ComputeStats ( pitchMat, stats );
            RealVector loudnessStats = ComputeStats ( loudnessMat, stats );
            RealVector shapeStats = ComputeStats ( shapeMat, stats );
            RealVector mfccStats = ComputeStats ( mfccMat, stats );

            RealVector allStats ( 168 );

            allStats ( fluid::Slice ( 0, 14 ) ) <<= pitchStats;
            allStats ( fluid::Slice ( 14, 14 ) ) <<= loudnessStats;
            allStats ( fluid::Slice ( 28, 49 ) ) <<= shapeStats;
            allStats ( fluid::Slice ( 77, 91 ) ) <<= mfccStats;

            dataset.add ( inFile, allStats );
        }

        auto outputJSON = JSONFile ( outFile, "w" );
        outputJSON.write ( dataset );

        if ( !outputJSON.ok ( ) )
        {
            ofLogError ( "AnalyseCorpus" ) << "failed to write output to " << outFile;
        }
    }

    fluid::RealVector ComputeStats ( fluid::RealMatrixView matrix, fluid::algorithm::MultiStats stats )
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

};