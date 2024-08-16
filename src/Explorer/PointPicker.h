#pragma once

#include "./RawView.h"
#include "Utils/DimensionBounds.h"
#include "Utils/DatasetConversion.h"
#include <algorithms/public/KDTree.hpp>
#include <data/FluidDataSet.hpp>
#include <ofCamera.h>
#include <ofEvents.h>
#include <mutex>
#include <atomic>

namespace Acorex {
namespace Explorer {

class PointPicker {
public:
	PointPicker ( ) { }
	~PointPicker ( ) { }

	void Initialise ( const Utils::DataSet& dataset, const Utils::DimensionBounds& dimensionBounds );

	void Train ( int dimensionIndex, Utils::Axis axis, bool none );

	void Exit ( );
	void RemoveListeners ( );

	void Draw ( );

	void FindNearestToMouse ( );
	bool FindNearestToPosition ( const glm::vec3& position, Utils::PointFT& nearestPoint, Utils::PointFT currentPoint, double maxAllowedDistanceSpace );

	// Setters & Getters ----------------------------

	void SetCamera ( std::shared_ptr<ofCamera> camera ) { mCamera = camera; }
	void SetNearestCheckNeeded ( ) { bNearestMouseCheckNeeded = true; }

	int GetNearestMousePointFile ( ) const { return mNearestPointFile; }
	int GetNearestMousePointTime ( ) const { return mNearestPointTime; }
	double GetNearestMouseDistance ( ) const { return mNearestDistance; }
	bool IsTrained ( ) const { return bTrained; }

private:
	void ScaleDataset ( Utils::DataSet& scaledDataset, const Utils::DimensionBounds& dimensionBounds );

	// Listeners ------------------------------------

	void MouseMoved ( ofMouseEventArgs& args ) { bNearestMouseCheckNeeded = true; }
	void KeyEvent ( ofKeyEventArgs& args );
	void MouseReleased ( ofMouseEventArgs& args );

	// States ---------------------------------------

	bool bClicked = false;
	bool bPicker = false;
	bool bDebug = false;
	bool bDraw = false;
	bool b3D = true;
	std::atomic<bool> bTrained = false;
	bool bSkipTraining = true;
	bool bListenersAdded = false;
	bool bNearestMouseCheckNeeded = false;
	bool bDimensionsFilled[3] = { false, false, false };

	// Variables ------------------------------------

	std::shared_ptr<ofCamera> mCamera;

	int mDimensionsIndices[3] = { -1, -1, -1 };

	int mNearestPoint = -1;
	double mNearestDistance = -1;

	double maxAllowedDistanceFar = 0.05;
	double maxAllowedDistanceNear = 0.01;

	fluid::algorithm::KDTree mKDTree;

	fluid::FluidDataSet<std::string, double, 1> mFullFluidSet;
	fluid::FluidDataSet<std::string, double, 1> mLiveFluidSet;
	std::vector<int> mCorpusFileLookUp; int mNearestPointFile = -1;
	std::vector<int> mCorpusTimeLookUp; int mNearestPointTime = -1;
	
	Utils::DatasetConversion mDatasetConversion;

	std::vector<glm::vec3> testPoints;
	std::vector<float> testRadii;
	std::vector<glm::vec3> testPointsOutOfRange;
	std::vector<float> testRadiiOutOfRange;

	// Thread safety --------------------------------

	std::mutex mPointPickerMutex;
};

} // namespace Explorer
} // namespace Acorex