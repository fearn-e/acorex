#include "./ExplorerMenu.h"
#include <ofUtils.h>

using namespace Acorex;

void ExplorerMenu::Initialise ( )
{
	// Pointer Sharing -----------------------------
	{
		if ( !bViewPointerShared )
		{
			mRawView = std::make_shared<Explorer::RawView> ( );
			mLiveView.SetRawView ( mRawView );
			bViewPointerShared = true;
		}
	}

	// Clear --------------------------------------
	{
		RemoveListeners ( );

		mMainPanel.clear ( );

		mLiveView.Initialise ( );
	}

	// Variables ----------------------------------
	{
		mLastUpdateTime = 0;
		mSlowUpdateInterval = 100;

		mOpenCorpusButtonClickTime = 0;
		mOpenCorpusButtonTimeout = 3000;
	}

	// States ------------------------------------
	{
		bIsCorpusOpen = false;
		bOpenCorpusDrawWarning = false;
	}

	// Main Panel --------------------------------
	{
		int dropdownScrollSpeed = 32;

		mMainPanel.setup ( );

		mMainPanel.add ( mCorpusNameLabel.setup ( "", bInitialiseShouldLoad ? mRawView->GetCorpusName ( ) : "No Corpus Loaded" ) );
		mMainPanel.add ( mOpenCorpusButton.setup ( "Open Corpus" ) );

		mCorpusNameLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mOpenCorpusButton.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mDimensionDropdownX.reset ( );
		mDimensionDropdownY.reset ( );
		mDimensionDropdownZ.reset ( );

		mDimensionDropdownX = make_unique<ofxDropdown> ( "X Dimension", dropdownScrollSpeed );
		mDimensionDropdownY = make_unique<ofxDropdown> ( "Y Dimension", dropdownScrollSpeed );
		mDimensionDropdownZ = make_unique<ofxDropdown> ( "Z Dimension", dropdownScrollSpeed );

		if ( bInitialiseShouldLoad )
		{
			for ( auto& dimension : mRawView->GetDimensions ( ) )
			{
				mDimensionDropdownX->add ( dimension );
				mDimensionDropdownY->add ( dimension );
				mDimensionDropdownZ->add ( dimension );
			}

			mMainPanel.add ( mDimensionDropdownX.get ( ) );
			mMainPanel.add ( mDimensionDropdownY.get ( ) );
			mMainPanel.add ( mDimensionDropdownZ.get ( ) );
		}

		mDimensionDropdownX->disableMultipleSelection ( );
		mDimensionDropdownY->disableMultipleSelection ( );
		mDimensionDropdownZ->disableMultipleSelection ( );

		mDimensionDropdownX->enableCollapseOnSelection ( );
		mDimensionDropdownY->enableCollapseOnSelection ( );
		mDimensionDropdownZ->enableCollapseOnSelection ( );

		mDimensionDropdownX->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownY->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownZ->setDropDownPosition ( ofxDropdown::DD_LEFT );

		mDimensionDropdownX->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownY->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownZ->setBackgroundColor ( mColors.interfaceBackgroundColor );

		mMainPanel.setPosition ( mLayout.explorePanelOriginX, mLayout.explorePanelOriginY );
		mMainPanel.setWidthElements ( mLayout.explorePanelWidth );
		mMainPanel.disableHeader ( );
	}

	// Listeners --------------------------------
	{
		mOpenCorpusButton.addListener ( this, &ExplorerMenu::OpenCorpus );
		mDimensionDropdownX->addListener ( this, &ExplorerMenu::SwapDimensionX );
		mDimensionDropdownY->addListener ( this, &ExplorerMenu::SwapDimensionY );
		mDimensionDropdownZ->addListener ( this, &ExplorerMenu::SwapDimensionZ );
		bListenersAdded = true;
	}

	bInitialiseShouldLoad = false;
}

void ExplorerMenu::Show ( )
{
	bDraw = true;
}

// hides menu without resetting
void ExplorerMenu::Hide ( )
{
	bDraw = false;
}

void ExplorerMenu::Draw ( )
{
	if ( !bDraw ) { return; }

	// Draw live view

	mMainPanel.draw ( );
}

void ExplorerMenu::Update ( )
{
	if ( ofGetElapsedTimeMillis ( ) - mLastUpdateTime > mSlowUpdateInterval )
	{
		mLastUpdateTime = ofGetElapsedTimeMillis ( );
		SlowUpdate ( );
	}
}

void ExplorerMenu::SlowUpdate ( )
{
	if ( bOpenCorpusDrawWarning && ofGetElapsedTimeMillis ( ) - mOpenCorpusButtonClickTime > mOpenCorpusButtonTimeout )
	{
		bOpenCorpusDrawWarning = false;
		mOpenCorpusButton.setName ( "Open Corpus" );
	}
}

void ExplorerMenu::Exit ( )
{
	RemoveListeners ( );
}

void ExplorerMenu::RemoveListeners ( )
{
	if ( !bListenersAdded ) { return; }
	mOpenCorpusButton.removeListener ( this, &ExplorerMenu::OpenCorpus );
	mDimensionDropdownX->removeListener ( this, &ExplorerMenu::SwapDimensionX );
	mDimensionDropdownY->removeListener ( this, &ExplorerMenu::SwapDimensionY );
	mDimensionDropdownZ->removeListener ( this, &ExplorerMenu::SwapDimensionZ );
	bListenersAdded = false;
}

// Main Functions ------------------------------

void ExplorerMenu::OpenCorpus ( )
{
	if ( bIsCorpusOpen && !bOpenCorpusDrawWarning )
	{
		bOpenCorpusDrawWarning = true;
		mOpenCorpusButtonClickTime = ofGetElapsedTimeMillis ( );
		mOpenCorpusButton.setName ( "!! Close Current? !!" );
		return;
	}

	bOpenCorpusDrawWarning = false;
	mOpenCorpusButton.setName ( "Open Corpus" );

	bool success = mRawView->LoadCorpus ( );
	if ( !success ) { return; }
	
	bInitialiseShouldLoad = true;
	Initialise ( );

	bIsCorpusOpen = true;

	mLiveView.CreatePoints ( );
	mDimensionDropdownX->setSelectedValueByIndex ( 0, true );
	mDimensionDropdownY->setSelectedValueByIndex ( 1, true );
	if ( mRawView->GetDimensions ( ).size ( ) > 2 )
	{
		mDimensionDropdownZ->setSelectedValueByIndex ( 2, true );
	}
}

// TODO - retrain point picker at the end of each SwapDimension call

void ExplorerMenu::SwapDimensionX ( string& dimension )
{
	if ( !bIsCorpusOpen ) { return; }

	ofLogNotice ( "ExplorerMenu" ) << "Swapping X dimension to " << dimension;
	mLiveView.FillDimension ( dimension, Explorer::LiveView::Axis::X );
}

void ExplorerMenu::SwapDimensionY ( string& dimension )
{
	if ( !bIsCorpusOpen ) { return; }

	ofLogNotice ( "ExplorerMenu" ) << "Swapping Y dimension to " << dimension;
	mLiveView.FillDimension ( dimension, Explorer::LiveView::Axis::Y );
}

void ExplorerMenu::SwapDimensionZ ( string& dimension )
{
	if ( !bIsCorpusOpen ) { return; }

	ofLogNotice ( "ExplorerMenu" ) << "Swapping Z dimension to " << dimension;
	mLiveView.FillDimension ( dimension, Explorer::LiveView::Axis::Z );
}
