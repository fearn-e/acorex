/*
The MIT License (MIT)

Copyright (c) 2024-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Utilities/TemporaryDefaults.h"

#include <ofSoundBuffer.h>
#include <string>
#include <vector>
#include <queue>
#include <nlohmann/json.hpp>
#include <ofColor.h>
#include <ofRectangle.h>
#include <algorithm>

#define DATA_CHANGE_CHECK_1

namespace Acorex {
namespace Utilities {

enum class Axis : int {
    X = 0,
    Y = 1,
    Z = 2,
    COLOR = 3,
    NONE = 4,
    MULTIPLE = 5,
    DYNAMIC_PAN = 6
};

struct DimensionBoundsData {
    std::vector<double> min; // [dimension]
    std::vector<double> max; // [dimension]
};

struct AudioData {
    std::vector<bool> loaded; // [file]
    std::vector<ofSoundBuffer> raw; // [file]
};

struct TrailData {
    std::vector<std::vector<std::vector<double>>> raw; // [file][timepoint][dimension] (first dimension is always time)
};

struct ExploreSettings {
private:
    int maxHopSize = 32;

    std::string dimensionX = "None";
    std::string dimensionY = "None";
    std::string dimensionZ = "None";

    std::string dimensionColor = "None";
    bool colorSpectrum = DEFAULT_COLOR_SPECTRUM;

    bool loopPlayheads = DEFAULT_LOOP_PLAYHEADS;
    bool jumpSameFileAllowed = DEFAULT_JUMP_SAME_FILE_ALLOWED;
    int jumpSameFileMinTimeDiff = DEFAULT_JUMP_SAME_FILE_MIN_DIFF;
    int crossoverJumpChanceX1000 = DEFAULT_CROSSOVER_JUMP_CHANCE_X1000;
    int crossfadeSampleLength = DEFAULT_CROSSFADE_SAMPLE_LENGTH;
    int maxJumpDistanceSpaceX1000 = DEFAULT_MAX_JUMP_DISTANCE_SPACE_X1000;
    int maxJumpTargets = DEFAULT_MAX_JUMP_TARGETS;

    int volumeX1000 = DEFAULT_VOLUME_X1000;
    std::string dimensionDynamicPan = "None";
    int panningStrengthX1000 = DEFAULT_PANNING_STRENGTH_X1000;

public:
    //setters
    void SetHopSize ( int hopSize ) { maxHopSize = hopSize; }

    void SetDimensionX ( const std::string& dimension ) { dimensionX = dimension; }
    void SetDimensionY ( const std::string& dimension ) { dimensionY = dimension; }
    void SetDimensionZ ( const std::string& dimension ) { dimensionZ = dimension; }

    void SetDimensionColor ( const std::string& dimension ) { dimensionColor = dimension; }
    void SetColorSpectrum ( bool colorVariant ) { colorSpectrum = colorVariant; }

    void SetLoopPlayheads ( bool loop ) { loopPlayheads = loop; }
    void SetJumpSameFileAllowed ( bool allowed ) { jumpSameFileAllowed = allowed; }
    void SetJumpSameFileMinTimeDiff ( int timeDiff ) { jumpSameFileMinTimeDiff = timeDiff; }
    void SetCrossoverJumpChanceX1000 ( int jumpsInAThousand ) { crossoverJumpChanceX1000 = jumpsInAThousand; }
    void SetCrossfadeSampleLength ( int length ) { crossfadeSampleLength = length; }
    void SetMaxJumpDistanceSpaceX1000 ( int distanceX1000 ) { maxJumpDistanceSpaceX1000 = distanceX1000; }
    void SetMaxJumpTargets ( int targets ) { maxJumpTargets = targets; }

    void SetVolumeX1000 ( int volumeX1000 ) { this->volumeX1000 = volumeX1000; }
    void SetDimensionDynamicPan ( const std::string& dimension ) { dimensionDynamicPan = dimension; }
    void SetPanningStrengthX1000 ( int panStrengthX1000 ) { panningStrengthX1000 = panStrengthX1000; }
    
    //getters
    int GetHopSize ( ) const { return maxHopSize; }

    const std::string& GetDimensionX ( ) const { return dimensionX; }
    const std::string& GetDimensionY ( ) const { return dimensionY; }
    const std::string& GetDimensionZ ( ) const { return dimensionZ; }

    const std::string& GetDimensionColor ( ) const { return dimensionColor; }
    bool GetColorSpectrum ( ) const { return colorSpectrum; }

    bool GetLoopPlayheads ( ) const { return loopPlayheads; }
    bool GetJumpSameFileAllowed ( ) const { return jumpSameFileAllowed; }
    int GetJumpSameFileMinTimeDiff ( ) const { return jumpSameFileMinTimeDiff; }
    int GetCrossoverJumpChanceX1000 ( ) const { return crossoverJumpChanceX1000; }
        float GetCrossoverJumpChance ( ) const { return static_cast<float>(crossoverJumpChanceX1000) / 1000.0; }
    int GetCrossfadeSampleLengthLimitedByHopSize ( ) const { return std::min( crossfadeSampleLength, maxHopSize ); }
    int GetMaxJumpDistanceSpaceX1000 ( ) const { return maxJumpDistanceSpaceX1000; }
        float GetMaxJumpDistanceSpace ( ) const { return static_cast<float>(maxJumpDistanceSpaceX1000) / 1000.0; }
    int GetMaxJumpTargets ( ) const { return maxJumpTargets; }

    int GetVolumeX1000 ( ) const { return volumeX1000; }
        float GetVolume ( ) const { return static_cast<float>(volumeX1000) / 1000.0; }
    const std::string& GetDimensionDynamicPan ( ) const { return dimensionDynamicPan; }
    int GetPanningStrengthX1000 ( ) const { return panningStrengthX1000; }
        float GetPanningStrength ( ) const { return static_cast<float>(panningStrengthX1000) / 1000.0; }
};

struct AnalysisSettings {
    int currentDimensionCount = 0;
    bool bIsReduction = false;
    bool bPitch = false;
    bool bLoudness = false;
    bool bShape = false;
    bool bMFCC = false;
    int sampleRate = DEFAULT_ANALYSE_SAMPLE_RATE;
    int windowFFTSize = DEFAULT_ANALYSE_WINDOW_SIZE;
    int hopFraction = DEFAULT_ANALYSE_HOP_SIZE_FRACTION;
    int nBands = DEFAULT_ANALYSE_MFCC_BANDS;
    int nCoefs = DEFAULT_ANALYSE_MFCC_COEFS;
    int minFreq = DEFAULT_ANALYSE_MIN_FREQ;
    int maxFreq = DEFAULT_ANALYSE_MAX_FREQ;
};

struct ReductionSettings {
    int dimensionReductionTarget = 3;
    int maxIterations = 200;
};

struct DataSet {
    int currentPointCount = 0;

    std::vector<std::string> dimensionNames; // [dimension]
    std::vector<std::string> fileList; // [file]

    AudioData audio;

    TrailData trails;

    AnalysisSettings analysisSettings;
};

struct PointFT {
    size_t file = 0;
    size_t time = 0;
};

struct AudioPlayhead {
    AudioPlayhead ( size_t ID, size_t file, size_t sample ) : playheadID ( ID ), fileIndex ( file ), sampleIndex ( sample ) { }

    size_t playheadID = 0;

    size_t fileIndex = 0;
    size_t sampleIndex = 0;

    bool crossfading = false;
    size_t jumpFileIndex = 0;
    size_t jumpSampleIndex = 0;
    size_t crossfadeCurrentSample = 0;
    size_t crossfadeSampleLength = 0;
    
    std::queue<size_t> triggerSamplePoints;
};

struct VisualPlayhead {
    VisualPlayhead ( size_t ID, size_t file, size_t sample ) : playheadID ( ID ), fileIndex ( file ), sampleIndex ( sample ) { }

    void ResizeBox ( size_t playheadIndexUI, size_t topBarHeight, size_t windowHeight, size_t windowWidth )
    {
        //TODO - put these in InterfaceDefs.h and have them affected by HiDpi
        int rectHeight = windowHeight / 20;
        int rectWidth = rectHeight * 4;

        int rectSpacing = windowWidth / 100;

        float x = 0;
        float y = topBarHeight + rectSpacing + (playheadIndexUI * (rectHeight + rectSpacing));

        panelRect = ofRectangle ( x, y, rectWidth, rectHeight );
        playheadColorRect = ofRectangle ( x, y, rectWidth / 4, rectHeight );
        killButtonRect = ofRectangle ( x + rectWidth - rectHeight, y, rectHeight, rectHeight );
    }

    bool highlight = false;

    size_t playheadID = 0;

    size_t fileIndex = 0;
    size_t sampleIndex = 0;

    float position[3] = { 0.0, 0.0, 0.0 };

    ofColor color = ofColor ( 255, 255, 255, 255 );
    ofRectangle panelRect = ofRectangle ( 0, 0, 0, 0 );
    ofRectangle playheadColorRect = ofRectangle ( 0, 0, 0, 0 );
    ofRectangle killButtonRect = ofRectangle ( 0, 0, 0, 0 );
};

} // namespace Utilities
} // namespace Acorex