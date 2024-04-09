#include "./ExplorerMenu.h"

using namespace Acorex;

void ExplorerMenu::Initialise ( )
{
}

void ExplorerMenu::Show ( )
{
}

// hides menu without resetting
void ExplorerMenu::Hide ( )
{
}

void ExplorerMenu::Draw ( )
{
}

void ExplorerMenu::Exit ( )
{
	RemoveListeners ( );
}

void ExplorerMenu::RemoveListeners ( )
{
}

// Main Functions ------------------------------

void ExplorerMenu::OpenCorpus ( )
{
	if ( bIsCorpusOpen )
	{
		; // TODO - warn user that they are about to close the current corpus
	}

	mRawView.LoadCorpus ( );

	bIsCorpusOpen = true;
}

void ExplorerMenu::SwapDimension ( )
{
}