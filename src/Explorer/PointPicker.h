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

#include "Explorer/RawView.h"
#include "Utilities/DimensionBounds.h"
#include "Utilities/DatasetConversion.h"

#include <flucoma/algorithms/public/KDTree.hpp>
#include <flucoma/data/FluidDataSet.hpp>
#include <ofCamera.h>
#include <ofEvents.h>
#include <mutex>
#include <atomic>

namespace Acorex {
namespace Explorer {

class PointPicker {
public:
    PointPicker ( );
    ~PointPicker ( ) { }

    void Initialise ( const Utilities::DataSet& dataset, const Utilities::DimensionBounds& dimensionBounds );
    void Clear ( );

    void Train ( int dimensionIndex, Utilities::Axis axis, bool none );

    void Exit ( );

    void Draw ( );

    void FindNearestToMouse ( );
    bool FindNearestToPosition (	const glm::vec3& position, Utilities::PointFT& nearestPoint, Utilities::PointFT currentPoint, 
                                    int maxAllowedDistanceSpaceX1000, int maxAllowedTargets, bool sameFileAllowed, 
                                    int minTimeDiffSameFile, int remainingSamplesRequired, const Utilities::AudioData& audioSet, size_t hopSize );

    // Setters & Getters ----------------------------

    void SetCamera ( std::shared_ptr<ofCamera> camera ) { mCamera = camera; }
    void SetNearestCheckNeeded ( ) { bNearestMouseCheckNeeded = true; }

    int GetNearestMousePointFile ( ) const { return mNearestPointFile; }
    int GetNearestMousePointTime ( ) const { return mNearestPointTime; }
    double GetNearestMouseDistance ( ) const { return mNearestDistance; }
    bool IsTrained ( ) const { return bTrained; }

private:
    void ScaleDataset ( Utilities::DataSet& scaledDataset, const Utilities::DimensionBounds& dimensionBounds );

    // Listeners ------------------------------------

    void AddListeners ( );
    void RemoveListeners ( );

    bool bListenersAdded;

    // Listener Functions ---------------------------

    void MouseMoved ( ofMouseEventArgs& args ) { bNearestMouseCheckNeeded = true; }
    void KeyEvent ( ofKeyEventArgs& args );
    void MouseReleased ( ofMouseEventArgs& args );

    // States ---------------------------------------

    bool bDebug;

    std::atomic<bool> bTrained;
    bool bSkipTraining;

    bool b3D;
    bool bPicker;
    bool bClicked;
    bool bNearestMouseCheckNeeded;

    bool bDimensionsFilled[3];

    // Variables ------------------------------------

    std::shared_ptr<ofCamera> mCamera;

    int mDimensionsIndices[3];

    int mNearestPoint;
    double mNearestDistance;

    double maxAllowedDistanceFar;
    double maxAllowedDistanceNear;

    fluid::algorithm::KDTree mKDTree;

    fluid::FluidDataSet<std::string, double, 1> mFullFluidSet;
    fluid::FluidDataSet<std::string, double, 1> mLiveFluidSet;
    std::vector<int> mCorpusFileLookUp; int mNearestPointFile;
    std::vector<int> mCorpusTimeLookUp; int mNearestPointTime;
    
    Utilities::DatasetConversion mDatasetConversion;

    std::vector<glm::vec3> testPoints;
    std::vector<float> testRadii;
    std::vector<glm::vec3> testPointsOutOfRange;
    std::vector<float> testRadiiOutOfRange;

    // Thread safety --------------------------------

    std::mutex mPointPickerMutex;
};

} // namespace Explorer
} // namespace Acorex