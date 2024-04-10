#pragma once

#include "Explorer/RawView.h"
#include "Utils/InterfaceDefs.h"
#include <ofxGui.h>

namespace Acorex {

class ExplorerMenu {
public:
	ExplorerMenu ( ) { };
	~ExplorerMenu ( ) { };

	void Initialise ( );
	void Show ( );
	void Hide ( );
	void Draw ( );
	void Update ( );
	void Exit ( );

private:
	void SlowUpdate ( );
	void RemoveListeners ( );

	// Main Functions ------------------------------

	void OpenCorpus ( );
	void SwapDimension ( );

	// States --------------------------------------

	bool bDraw = false;

	bool bIsCorpusOpen = false;
	bool bOpenCorpusDrawWarning = false;

	// Timing --------------------------------------

	int mLastUpdateTime = 0;
	int mSlowUpdateInterval = 100;

	int mOpenCorpusButtonClickTime = 0;
	int mOpenCorpusButtonTimeout = 3000;

	// Panels --------------------------------------

	ofxPanel mMainPanel;
	ofxLabel mCorpusNameLabel;
	ofxButton mOpenCorpusButton;

	// Acorex Objects ------------------------------

	Explorer::RawView mRawView;
	Utils::Colors mColors;
	Utils::MenuLayout mLayout;
};

} // namespace Acorex