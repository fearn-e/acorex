#pragma once

#include "./RawView.h"
#include "Utils/DimensionBounds.h"
#include "Utils/DatasetConversion.h"
#include <algorithms/public/KDTree.hpp>
#include <data/FluidDataSet.hpp>
#include <ofCamera.h>
#include <ofEvents.h>

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

	void SlowUpdate ( );
	void Draw ( );

	void FindNearest ( );

	// Setters & Getters ----------------------------

	void SetCamera ( std::shared_ptr<ofCamera> camera ) { mCamera = camera; }
	void SetNearestCheckNeeded ( ) { bNearestCheckNeeded = true; }

	int GetNearestPointFile ( ) const { return mNearestPointFile; }
	int GetNearestPointTime ( ) const { return mNearestPointTime; }
	double GetNearestDistance ( ) const { return mNearestDistance; }
	bool IsTrained ( ) const { return bTrained; }

private:
	void ScaleDataset ( Utils::DataSet& scaledDataset, const Utils::DimensionBounds& dimensionBounds );

	// Listeners ------------------------------------

	void MouseMoved ( ofMouseEventArgs& args ) { bNearestCheckNeeded = true; }

	// States ---------------------------------------

	bool bDraw = false;
	bool b3D = true;
	bool bTrained = false;
	bool bSkipTraining = true;
	bool bListenersAdded = false;
	bool bNearestCheckNeeded = false;
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
};

} // namespace Explorer
} // namespace Acorex