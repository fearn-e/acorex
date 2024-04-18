#pragma once

#include "./RawView.h"
#include <ofMesh.h>
#include <ofEasyCam.h>

namespace Acorex {
namespace Explorer {

class LiveView {
public:
	enum Axis {
		X = 0,
		Y = 1,
		Z = 2,
		COLOR = 3
	};

	LiveView ( ) { }
	~LiveView ( ) { }

	void SetRawView ( std::shared_ptr<RawView>& rawPointer );
	void Initialise ( );

	void Draw ( );

	void CreatePoints ( );

	void FillDimensionTime ( int dimensionIndex, Axis axis );
	void FillDimensionStats ( int dimensionIndex, Axis axis );
	void FillDimensionStatsReduced ( int dimensionIndex, Axis axis );
	void FillDimensionNone ( Axis axis );

private:
	bool bDraw = false;

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
	std::vector<ofMesh> mTimeCorpus;
	ofMesh mStatsCorpus;

	ofEasyCam mTemporaryCam;
};

} // namespace Explorer
} // namespace Acorex