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
	mRawView.LoadCorpus ( );
}

void ExplorerMenu::SwapDimension ( )
{
}