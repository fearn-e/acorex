/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

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