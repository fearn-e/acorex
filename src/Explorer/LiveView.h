#pragma once

#include "./RawView.h"
#include <ofMesh.h>
namespace Acorex {
namespace Explorer {

class LiveView {
public:
	enum Axis {
		X = 0,
		Y = 1,
		Z = 2
	};

	LiveView ( ) { }
	~LiveView ( ) { }

	void SetRawView ( std::shared_ptr<RawView>& rawPointer );
	void Initialise ( );

	void CreatePoints ( );

	void FillDimension ( std::string& dimension, Axis axis );

private:
	int GetDimensionIndex ( std::string& dimension );

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
	std::vector<ofMesh> mTimeCorpus;
	ofMesh mStatsCorpus;
};

} // namespace Explorer
} // namespace Acorex