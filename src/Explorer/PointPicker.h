#pragma once

#include "./RawView.h"
#include "Utils/DimensionBounds.h"
#include "Utils/DatasetConversion.h"
#include <algorithms/public/KDTree.hpp>
#include <data/FluidDataSet.hpp>
#include <algorithms/public/DataSetQuery.hpp>
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

	int GetNearestPoint ( ) const { return mNearestPoint; }
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

	double maxAllowedDistance = 50;

	fluid::algorithm::KDTree mKDTree;

	fluid::FluidDataSet<std::string, double, 1> mFullFluidSet;
	fluid::FluidDataSet<std::string, double, 1> mLiveFluidSet;
	fluid::algorithm::DataSetQuery mFluidSetQuery;
	std::vector<int> mCorpusFileLookUp;
	std::vector<int> mCorpusTimepointLookUp;
	
	Utils::DatasetConversion mDatasetConversion;
};

} // namespace Explorer
} // namespace Acorex