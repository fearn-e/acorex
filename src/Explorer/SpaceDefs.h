#pragma once

namespace Acorex {
namespace Explorer {

namespace SpaceDefs {

	// Camera Adjustments --------------------

	static float mCamZoomSpeed3D = 200;		static float mCamZoomSpeed2D = 0.1;
	static float mZoomMin3D = 50.0;			static float mZoomMax3D = 10000.0;
	static float mZoomMin2D = 0.1;			static float mZoomMax2D = 10.0;
	static float mCamMoveSpeed = 1.0;
	static float mCamRotateSpeed = 0.006;
	static float mKeyboardMoveSpeed = 3000.0;
	static float mKeyboardRotateSpeed = 500.0;
	static float mKeyboardZoomSpeed = 50.0;

	// Space and Color Limits --------------------

	static double mSpaceMin = 0; static double mSpaceMax = 5000;
	static double mColorMin = 0; static double mColorMax = 215;
	static double mColorBlue = 170; static double mColorRed = 255;

} // namespace SpaceDefs

} // namespace Utils
} // namespace Acorex