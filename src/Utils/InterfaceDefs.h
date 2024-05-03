#pragma once

#include <ofxGui.h>

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
		int topBarHeight = 40;

		int analyseMainPanelWidth = 200;
		int analyseAnalysisPanelWidth = 315;
		int analyseReductionPanelWidth = 300;

		int explorePanelWidth = 315;

		int interPanelSpacing = 5;
		int panelBackgroundMargin = 5;

		int analysePanelOriginX = 0; int analysePanelOriginY = topBarHeight + interPanelSpacing;
		int explorePanelOriginY = topBarHeight + interPanelSpacing;
		glm::vec3 hiddenPanelPosition = { -1000, -1000, 0 };

		void disableHiDpi ( )
		{
			topBarHeight = 40;
			analyseMainPanelWidth = 200;
			analyseAnalysisPanelWidth = 315;
			analyseReductionPanelWidth = 300;
			explorePanelWidth = 315;
			interPanelSpacing = 5;
			panelBackgroundMargin = 5;
			analysePanelOriginX = 0; analysePanelOriginY = topBarHeight + interPanelSpacing;
			explorePanelOriginY = topBarHeight + interPanelSpacing;
		}

		void enableHiDpi ( )
		{
			topBarHeight = 40 * 2;
			analyseMainPanelWidth = 200 * 2;
			analyseAnalysisPanelWidth = 315 * 2;
			analyseReductionPanelWidth = 300 * 2;
			explorePanelWidth = 315 * 2;
			interPanelSpacing = 5 * 2;
			panelBackgroundMargin = 5 * 2;
			analysePanelOriginX = 0; analysePanelOriginY = topBarHeight + interPanelSpacing;
			explorePanelOriginY = topBarHeight + interPanelSpacing;
		}
	};
} // namespace Utils
} // namespace Acorex