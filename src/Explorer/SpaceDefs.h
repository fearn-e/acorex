#pragma once

namespace Acorex {
namespace Explorer {

namespace SpaceDefs {

	// Camera Adjustments --------------------

	static float mCamZoomSpeed3D = 200;		static float mCamZoomSpeed2D = 0.1;
	static float mZoomMin3D = 50.0;			static float mZoomMax3D = 5000.0;
	static float mZoomMin2D = 0.1;			static float mZoomMax2D = 10.0;
	static float mCamMoveSpeed = 1.0;
	static float mCamRotateSpeed = 0.006;
	static float mKeyboardSpeedMulti = 800.0;

	// Space and Color Limits --------------------

	static double mSpaceMin = 0; static double mSpaceMax = 5000;
	static double mColorMin = 0; static double mColorMax = 215;

} // namespace SpaceDefs

} // namespace Utils
} // namespace Acorex