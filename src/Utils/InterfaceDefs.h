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

#include <ofxGui.h>

#define TOP_BAR_HEIGHT					40
#define TOP_BAR_BUTTON_WIDTH			100
#define ANALYSE_MAIN_PANEL_WIDTH		200
#define ANALYSE_ANALYSIS_PANEL_WIDTH	315
#define ANALYSE_REDUCTION_PANEL_WIDTH	300
#define EXPLORE_PANEL_WIDTH				315
#define INTER_PANEL_SPACING				5
#define PANEL_BACKGROUND_MARGIN			5

namespace Acorex {
namespace Utils {

	struct Colors {
		// normal and locked text colour
		ofColor normalTextColor = 255;
		ofColor lockedTextColor = 130;

		// interface background colour
		ofColor interfaceBackgroundColor = { 0, 0, 0, 150 };
		ofColor transparent = { 0, 0, 0, 0 };
	};

	struct MenuLayout {
		bool HiDpi = false;

		int topBarHeight = TOP_BAR_HEIGHT;
		int topBarButtonWidth = TOP_BAR_BUTTON_WIDTH;

		int analyseMainPanelWidth = ANALYSE_MAIN_PANEL_WIDTH;
		int analyseAnalysisPanelWidth = ANALYSE_ANALYSIS_PANEL_WIDTH;
		int analyseReductionPanelWidth = ANALYSE_REDUCTION_PANEL_WIDTH;

		int explorePanelWidth = EXPLORE_PANEL_WIDTH;

		int interPanelSpacing = INTER_PANEL_SPACING;
		int panelBackgroundMargin = PANEL_BACKGROUND_MARGIN;

		int analysePanelOriginX = 0; int analysePanelOriginY = topBarHeight + interPanelSpacing;
		int explorePanelOriginY = topBarHeight + interPanelSpacing;
		glm::vec3 hiddenPanelPosition = { -1000, -1000, 0 };

		void disableHiDpi ( )
		{
			HiDpi = false;
			topBarHeight = TOP_BAR_HEIGHT;
			topBarButtonWidth = TOP_BAR_BUTTON_WIDTH;
			analyseMainPanelWidth = ANALYSE_MAIN_PANEL_WIDTH;
			analyseAnalysisPanelWidth = ANALYSE_ANALYSIS_PANEL_WIDTH;
			analyseReductionPanelWidth = ANALYSE_REDUCTION_PANEL_WIDTH;
			explorePanelWidth = EXPLORE_PANEL_WIDTH;
			interPanelSpacing = INTER_PANEL_SPACING;
			panelBackgroundMargin = PANEL_BACKGROUND_MARGIN;
			analysePanelOriginX = 0; analysePanelOriginY = topBarHeight + interPanelSpacing;
			explorePanelOriginY = topBarHeight + interPanelSpacing;
		}

		void enableHiDpi ( )
		{
			HiDpi = true;
			topBarHeight = TOP_BAR_HEIGHT * 2;
			topBarButtonWidth = TOP_BAR_BUTTON_WIDTH * 2;
			analyseMainPanelWidth = ANALYSE_MAIN_PANEL_WIDTH * 2;
			analyseAnalysisPanelWidth = ANALYSE_ANALYSIS_PANEL_WIDTH * 2;
			analyseReductionPanelWidth = ANALYSE_REDUCTION_PANEL_WIDTH * 2;
			explorePanelWidth = EXPLORE_PANEL_WIDTH * 2;
			interPanelSpacing = INTER_PANEL_SPACING * 2;
			panelBackgroundMargin = PANEL_BACKGROUND_MARGIN * 2;
			analysePanelOriginX = 0; analysePanelOriginY = topBarHeight + interPanelSpacing;
			explorePanelOriginY = topBarHeight + interPanelSpacing;
		}
	};
} // namespace Utils
} // namespace Acorex

#undef TOP_BAR_HEIGHT
#undef TOP_BAR_BUTTON_WIDTH
#undef ANALYSE_MAIN_PANEL_WIDTH
#undef ANALYSE_ANALYSIS_PANEL_WIDTH
#undef ANALYSE_REDUCTION_PANEL_WIDTH
#undef EXPLORE_PANEL_WIDTH
#undef INTER_PANEL_SPACING
#undef PANEL_BACKGROUND_MARGIN