#pragma once

#include "Explorer/RawView.h"
#include "Utils/InterfaceDefs.h"
#include <ofxGui.h>
#include <ofxDropdown.h>

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
	void SwapDimensionX ( string& dimension );
	void SwapDimensionY ( string& dimension );
	void SwapDimensionZ ( string& dimension );

	// States --------------------------------------

	bool bDraw = false;

	bool bIsCorpusOpen = false;
	bool bOpenCorpusDrawWarning = false;
	bool bInitialiseShouldLoad = false;

	// Timing --------------------------------------

	int mLastUpdateTime = 0;
	int mSlowUpdateInterval = 100;

	int mOpenCorpusButtonClickTime = 0;
	int mOpenCorpusButtonTimeout = 3000;

	// Panels --------------------------------------

	ofxPanel mMainPanel;
	ofxLabel mCorpusNameLabel;
	ofxButton mOpenCorpusButton;
	unique_ptr<ofxDropdown> mDimensionDropdownX;
	unique_ptr<ofxDropdown> mDimensionDropdownY;
	unique_ptr<ofxDropdown> mDimensionDropdownZ;

	// Acorex Objects ------------------------------

	Explorer::RawView mRawView;
	Utils::Colors mColors;
	Utils::MenuLayout mLayout;
};

} // namespace Acorex