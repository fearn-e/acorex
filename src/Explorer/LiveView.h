#pragma once

#include "./RawView.h"
namespace Acorex {
namespace Explorer {

class LiveView {
public:
	LiveView ( ) { }
	~LiveView ( ) { }

	void SetRawView ( std::shared_ptr<RawView>& rawPointer );
	void Initialise ( );

private:
	std::shared_ptr<RawView> mRawView; // might need to be weak_ptr?
};

} // namespace Explorer
} // namespace Acorex