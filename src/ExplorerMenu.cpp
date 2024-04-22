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
		mDimensionDropdownColor.reset ( );

		mDimensionDropdownX = make_unique<ofxDropdown> ( "X Dimension", dropdownScrollSpeed );
		mDimensionDropdownY = make_unique<ofxDropdown> ( "Y Dimension", dropdownScrollSpeed );
		mDimensionDropdownZ = make_unique<ofxDropdown> ( "Z Dimension", dropdownScrollSpeed );
		mDimensionDropdownColor = make_unique<ofxDropdown> ( "Color Dimension", dropdownScrollSpeed );

		if ( bInitialiseShouldLoad )
		{
			mDimensionDropdownX->add ( "None" );
			mDimensionDropdownY->add ( "None" );
			mDimensionDropdownZ->add ( "None" );
			mDimensionDropdownColor->add ( "None" );

			if ( mRawView->IsTimeAnalysis ( ) )
			{
				mDimensionDropdownX->add ( "Time" );
				mDimensionDropdownY->add ( "Time" );
				mDimensionDropdownZ->add ( "Time" );
				mDimensionDropdownColor->add ( "Time" );
			}

			for ( auto& dimension : mRawView->GetDimensions ( ) )
			{
				mDimensionDropdownX->add ( dimension );
				mDimensionDropdownY->add ( dimension );
				mDimensionDropdownZ->add ( dimension );
				mDimensionDropdownColor->add ( dimension );
			}

			bool needStatisticDropdowns = !mRawView->IsTimeAnalysis ( ) && !mRawView->IsReduction ( );

			mMainPanel.add ( mDimensionDropdownX.get ( ) );
			mMainPanel.add ( mDimensionDropdownY.get ( ) );
			mMainPanel.add ( mDimensionDropdownZ.get ( ) );
			mMainPanel.add ( mDimensionDropdownColor.get ( ) );
		}

		mDimensionDropdownX->disableMultipleSelection ( );
		mDimensionDropdownY->disableMultipleSelection ( );
		mDimensionDropdownZ->disableMultipleSelection ( );
		mDimensionDropdownColor->disableMultipleSelection ( );

		mDimensionDropdownX->enableCollapseOnSelection ( );
		mDimensionDropdownY->enableCollapseOnSelection ( );
		mDimensionDropdownZ->enableCollapseOnSelection ( );
		mDimensionDropdownColor->enableCollapseOnSelection ( );

		mDimensionDropdownX->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownY->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownZ->setDropDownPosition ( ofxDropdown::DD_LEFT );
		mDimensionDropdownColor->setDropDownPosition ( ofxDropdown::DD_LEFT );

		mDimensionDropdownX->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownY->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownZ->setBackgroundColor ( mColors.interfaceBackgroundColor );
		mDimensionDropdownColor->setBackgroundColor ( mColors.interfaceBackgroundColor );

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
		mDimensionDropdownColor->addListener ( this, &ExplorerMenu::SwapDimensionColor );
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

	mLiveView.Draw ( );

	// call pointpicker draw

	mMainPanel.draw ( );
}

void ExplorerMenu::Update ( )
{
	mLiveView.Update ( );

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
	mDimensionDropdownColor->removeListener ( this, &ExplorerMenu::SwapDimensionColor );
	bListenersAdded = false;
}

// Main Functions ------------------------------

void ExplorerMenu::OpenCorpus ( )
{
	bBlockDimensionFilling = true;

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

	mLiveView.CreatePoints ( );

	// set default dropdown values
	std::string xDimension = "Time", yDimension = "None", zDimension = "None", colorDimension = "None";
	{
		int dimensionCount = mRawView->GetDimensions ( ).size ( );
		if ( mRawView->IsTimeAnalysis ( ) )
		{
			xDimension = "Time";
			yDimension = mRawView->GetDimensions ( ).size ( ) > 0 ? mRawView->GetDimensions ( )[0] : "None";
			zDimension = mRawView->GetDimensions ( ).size ( ) > 1 ? mRawView->GetDimensions ( )[1] : "None";
			colorDimension = mRawView->GetDimensions ( ).size ( ) > 2 ? mRawView->GetDimensions ( )[2] : "None";
		}
		else if ( mRawView->IsReduction ( ) )
		{
			xDimension = mRawView->GetDimensions ( ).size ( ) > 0 ? mRawView->GetDimensions ( )[0] : "None";
			yDimension = mRawView->GetDimensions ( ).size ( ) > 1 ? mRawView->GetDimensions ( )[1] : "None";
			zDimension = mRawView->GetDimensions ( ).size ( ) > 2 ? mRawView->GetDimensions ( )[2] : "None";
			colorDimension = mRawView->GetDimensions ( ).size ( ) > 3 ? mRawView->GetDimensions ( )[3] : "None";
		}
		else
		{
			xDimension = mRawView->GetDimensions ( ).size ( ) > 0 ? mRawView->GetDimensions ( )[0] : "None";
			yDimension = mRawView->GetDimensions ( ).size ( ) > 7 ? mRawView->GetDimensions ( )[7] : "None";
			zDimension = mRawView->GetDimensions ( ).size ( ) > 14 ? mRawView->GetDimensions ( )[14] : "None";
			colorDimension = mRawView->GetDimensions ( ).size ( ) > 21 ? mRawView->GetDimensions ( )[21] : "None";
		}
		
	}

	mDimensionDropdownX->setSelectedValueByName ( xDimension, true );
	mDimensionDropdownY->setSelectedValueByName ( yDimension, true );
	mDimensionDropdownZ->setSelectedValueByName ( zDimension, true );
	mDimensionDropdownColor->setSelectedValueByName ( colorDimension, true );

	bBlockDimensionFilling = false;

	SwapDimension ( xDimension, Explorer::LiveView::Axis::X );
	SwapDimension ( yDimension, Explorer::LiveView::Axis::Y );
	SwapDimension ( zDimension, Explorer::LiveView::Axis::Z );

	bIsCorpusOpen = true;

	SwapDimension ( colorDimension, Explorer::LiveView::Axis::COLOR );
}

void ExplorerMenu::SwapDimension ( string dimension, Explorer::LiveView::Axis axis )
{
	if ( bBlockDimensionFilling ) { return; }

	if ( dimension == "None" )					{ mLiveView.FillDimensionNone ( axis ); }
	else if ( dimension == "Time" )				{ mLiveView.FillDimensionTime ( -1, axis ); }
	else
	{
		int dimensionIndex = GetDimensionIndex ( dimension );
		if ( dimensionIndex == -1 ) { return; }

		if ( mRawView->IsTimeAnalysis ( ) )		{ mLiveView.FillDimensionTime ( dimensionIndex, axis ); }
		else if ( !mRawView->IsReduction ( ) )	{ mLiveView.FillDimensionStats ( dimensionIndex, axis ); }
		else									{ mLiveView.FillDimensionStatsReduced ( dimensionIndex, axis ); }
	}
	
	if ( bIsCorpusOpen )
	{
		CameraSwitcher ( );
		// TODO - if axis != COLOR, retrain point picker
	}
}

int ExplorerMenu::GetDimensionIndex ( std::string& dimension )
{
	for ( int i = 0; i < mRawView->GetDimensions ( ).size ( ); i++ )
	{
		if ( mRawView->GetDimensions ( )[i] == dimension )
		{
			return i;
		}
	}
	ofLogWarning ( "LiveView" ) << "Dimension " << dimension << " not found";
	return -1;
}

void ExplorerMenu::CameraSwitcher ( )
{
	bool isXNone = mDimensionDropdownX->getAllSelected ( )[0] == "None";
	bool isYNone = mDimensionDropdownY->getAllSelected ( )[0] == "None";
	bool isZNone = mDimensionDropdownZ->getAllSelected ( )[0] == "None";
	int numDisabledAxes = isXNone + isYNone + isZNone;

	Explorer::LiveView::Axis			  disabledAxis = Explorer::LiveView::Axis::NONE;
	if		( isXNone )					{ disabledAxis = Explorer::LiveView::Axis::X; }
	else if ( isYNone )					{ disabledAxis = Explorer::LiveView::Axis::Y; }
	else if ( isZNone )					{ disabledAxis = Explorer::LiveView::Axis::Z; }
	else if ( numDisabledAxes > 1 )		{ disabledAxis = Explorer::LiveView::Axis::MULTIPLE; }

	bool current3D = mLiveView.Is3D ( );

	if ( disabledAxis == Explorer::LiveView::Axis::NONE || disabledAxis == Explorer::LiveView::Axis::MULTIPLE )
	{
		if ( !mLiveView.Is3D ( ) )
		{
			mLiveView.Set3D ( true );
			mLiveView.Init3DCam ( );
		}
	}
	else
	{
		if ( mLiveView.Is3D ( ) || disabledAxis != mDisabledAxis )
		{
			mLiveView.Set3D ( false );
			mLiveView.Init2DCam ( disabledAxis );
			mDisabledAxis = disabledAxis;
		}
	}
}

// Listener Functions --------------------------

void ExplorerMenu::SwapDimensionX ( string& dimension )
{
	SwapDimension ( dimension, Explorer::LiveView::Axis::X );
}

void ExplorerMenu::SwapDimensionY ( string& dimension )
{
	SwapDimension ( dimension, Explorer::LiveView::Axis::Y );
}

void ExplorerMenu::SwapDimensionZ ( string& dimension )
{
	SwapDimension ( dimension, Explorer::LiveView::Axis::Z );
}

void ExplorerMenu::SwapDimensionColor ( string& dimension )
{
	SwapDimension ( dimension, Explorer::LiveView::Axis::COLOR );
}