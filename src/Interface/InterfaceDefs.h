#pragma once

#include <ofxGui.h>

namespace AcorexInterface {

	struct Colors {
		// normal and locked text colour
		ofColor normalTextColor = 255;
		ofColor lockedTextColor = 130;

	};

	struct ControllerUILayout {
		glm::vec3 defaultPanelPosition = { 40, 40, 0 };
		glm::vec3 hiddenPanelPosition = { -1000, -1000, 0 };

		int interPanelSpacing = 5;

		int mainPanelWidth = 200;
		int analysisPanelWidth = 315;
		int reductionPanelWidth = 300;
	};
} // namespace interface