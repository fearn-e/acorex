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

		//explore panel widths

		int interPanelSpacing = 5;
		int panelBackgroundMargin = 5;

		int analysePanelOriginX = 0; int analysePanelOriginY = topBarHeight + interPanelSpacing;
		// int explorePanelOriginX = ofGetWidth ( ) - explorePanelWidth; int explorePanelOriginY = topBarHeight + interPanelSpacing;
		glm::vec3 hiddenPanelPosition = { -1000, -1000, 0 };
	};
} // namespace Utils
} // namespace Acorex