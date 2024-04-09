#pragma once

namespace Acorex {

class ExplorerMenu {
public:
	ExplorerMenu ( ) { };
	~ExplorerMenu ( ) { };

	void Show ( );
	void Reset ( );
	void Draw ( );
	void Exit ( );

private:
	void RemoveListeners ( );

	// Main Functions ------------------------------

	void OpenCorpus ( );
	void SwapDimension ( );

};

} // namespace Acorex