#include <assert.h>
#include <iostream>
#include <fstream>
#include <list>
#include "MyLib.h"
#include "Board.h"
#include "View.h"
#include "Dialog.h"
// Add more includes, possibly, but not board-specific ones

using namespace std;

int main(int argc, char **argv) {
	Board *board = 0, *cmpBoard;
	Board::Move *move, *cmpMove;
	int argLen, count;
	string command, cArg;
	// Many more locals needed

	// Sample of some Class code
	const Class *viewCls, *dlgCls;
	const BoardClass *brdCls;

	// Set up Class objects based on commandline args, with appropriate
	// error handling, so that this works...
	// board = dynamic_cast<Board *>(brdCls->NewInstance());

	// Just a sampling of the main scaffold-loop.  You'll make yours a lot longer,
	// will need to use a try/catch block, and are welcome to violate the function
	// line limit rule for this one method.
	while (cin >> command) {
		if (command.compare("undoLastMove") == 0) {
			cin >> count;

			if (count > board->GetMoveHist().size())
				count = board->GetMoveHist().size();

			while (count-- > 0)
				board->UndoLastMove();
		} else if (command.compare("enterMove") == 0) {
			getline(cin, cArg);
			*move = cArg.c_str();
		} else if (command.compare("saveBoard") == 0) {
			cin >> cArg;
			ofstream out(cArg.c_str());
			out << *board;
		} else if (command.compare("showVal") == 0) {
			cout << "Value: " << board->GetValue() << endl;
		} else if (command.compare("quit") == 0)
			break;
		else
			cout << "Unknown command: " << command << endl;

		cout << endl;
	}

	delete board;

	return 0;
}
