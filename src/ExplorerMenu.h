#pragma once

#include "Explorer/RawView.h"

namespace Acorex {

class ExplorerMenu {
public:
	ExplorerMenu ( ) { };
	~ExplorerMenu ( ) { };

	void Initialise ( );
	void Show ( );
	void Hide ( );
	void Draw ( );
	void Exit ( );

private:
	void RemoveListeners ( );

	// Main Functions ------------------------------

	void OpenCorpus ( );
	void SwapDimension ( );

	Explorer::RawView mRawView;
};

} // namespace Acorex