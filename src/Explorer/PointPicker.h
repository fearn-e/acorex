#pragma once

#include "./RawView.h"
#include <algorithms/public/KDTree.hpp>
//#include <clients/nrt/KDTreeClient.hpp>

namespace Acorex {
namespace Explorer {

class PointPicker {
public:
	PointPicker ( ) { }
	~PointPicker ( ) { }

	void Train ( );

	void FindNearest ( );

	// Setters & Getters ----------------------------

	void SetRawView ( std::shared_ptr<RawView>& rawPointer ) { mRawView = rawPointer; }

	int GetNearestPoint ( ) const { return mNearestPoint; }
	double GetNearestDistance ( ) const { return mNearestDistance; }
	bool IsTrained ( ) const { return bTrained; }

private:
	bool bTrained = false;
	
	int mNearestPoint = -1;
	double mNearestDistance = -1;

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
};

} // namespace Explorer
} // namespace Acorex