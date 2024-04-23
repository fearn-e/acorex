#pragma once

#include "./RawView.h"
#include "Utils/DatasetConversion.h"
#include <algorithms/public/KDTree.hpp>
#include <data/FluidDataSet.hpp>
#include <algorithms/public/DataSetQuery.hpp>

namespace Acorex {
namespace Explorer {

class PointPicker {
public:
	PointPicker ( ) { }
	~PointPicker ( ) { }

	void Initialise ( const Utils::DataSet& dataset );

	void FindNearest ( );
	void Train ( int dimensionIndex, Utils::Axis axis, bool none );


	// Setters & Getters ----------------------------

	void SetCamera ( std::shared_ptr<ofCamera> camera ) { mCamera = camera; }

	int GetNearestPoint ( ) const { return mNearestPoint; }
	double GetNearestDistance ( ) const { return mNearestDistance; }
	bool IsTrained ( ) const { return bTrained; }

private:
	bool bTrained = false;
	bool bSkipTraining = true;
	
	bool mDimensionsFilled[3] = { false, false, false };
	int mDimensionsIndices[3] = { -1, -1, -1 };
	
	int mNearestPoint = -1;
	double mNearestDistance = -1;

	double maxAllowedDistance = 50;

	fluid::algorithm::KDTree mKDTree;

	fluid::FluidDataSet<std::string, double, 1> mFullFluidSet;
	fluid::FluidDataSet<std::string, double, 1> mLiveFluidSet;
	fluid::algorithm::DataSetQuery mFluidSetQuery;
	
	std::shared_ptr<ofCamera> mCamera;

	Utils::DatasetConversion mDatasetConversion;
};

} // namespace Explorer
} // namespace Acorex