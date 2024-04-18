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
		COLOR = 3,
		NONE = 4,
		MULTIPLE = 5
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

	void Set3D ( bool is3D ) { b3D = is3D; }
	void Init3DCam ( );
	void Init2DCam ( Axis disabledAxis );

	bool Is3D ( ) const { return b3D; }

private:
	bool bDraw = false;
	bool b3D = true;

	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
	std::vector<ofMesh> mTimeCorpus;
	ofMesh mStatsCorpus;

	ofEasyCam m3DCam;
	ofEasyCam m2DCam;
	int midSpacePoint = 500;
};

} // namespace Explorer
} // namespace Acorex