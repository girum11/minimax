#include <assert.h>
#include <iostream>
#include <fstream>
#include <list>
#include "MyLib.h"
#include "Board.h"
#include "View.h"
#include "Dialog.h"
#include <exception>

// Add more includes, possibly, but not board-specific ones
// TODO: Remove these includes once you get reflection working
#include "OthelloBoard.h"
#include "OthelloMove.h"
#include "OthelloDlg.h"
#include "OthelloView.h"
#include "PylosBoard.h"
#include "PylosMove.h"
#include "PylosDlg.h"
#include "PylosView.h"

using namespace std;

int main(int argc, char **argv) {
	Board *board = 0, *cmpBoard;
	Board::Move *move, *cmpMove;
	int argLen, count;
	string command, cArg;
	// Many more locals needed

	// Sample of some Class code
	// const Class *viewCls, *dlgCls;
	// const BoardClass *brdCls;

	// Set up Class objects based on commandline args, with appropriate
	// error handling, so that this works...
	// board = dynamic_cast<Board *>(brdCls->NewInstance());

	/// TODO: Hack for now - I'm avoiding reflection
	board = new PylosBoard();
	PylosView *view = new PylosView();

	// enterMove issue is probably here... but I have no clue why
	move = board->CreateMove();

	// Just a sampling of the main scaffold-loop.  You'll make yours a lot longer,
	// will need to use a try/catch block, and are welcome to violate the function
	// line limit rule for this one method.
	while (cin >> command) {

		try {
			if (command.compare("undoLastMove") == 0) {
				cin >> count;

				if (count > board->GetMoveHist().size())
					count = board->GetMoveHist().size();

				while (count-- > 0)
					board->UndoLastMove();
			} else if (command.compare("saveBoard") == 0) {
				cin >> cArg;
				ofstream out(cArg.c_str());
				out << *board;
			} else if (command.compare("showVal") == 0) {
				cout << "Value: " << board->GetValue() << endl;
			} else if (command.compare("showBoard") == 0) {
				// Show the board
				view->SetModel(board);
				view->Draw(cout);

				// Print out all possible moves
				cout << "All Moves: ";
				
				list<Board::Move *> listOfMoves;
				board->GetAllMoves(&listOfMoves);
				list<Board::Move *>::const_iterator listIter;
				for (listIter = listOfMoves.begin(); listIter != listOfMoves.end(); ++listIter) {
					cout << (string) **listIter << " ";
				}
			} else if (command.compare("enterMove") == 0) {
				// The exception is the exception that's thrown in the Board::Move member function.
				// It's this reason why we need to wrap this test scaffold up in a try-catch block.
				getline(cin, cArg);
				*move = cArg.c_str();
			} else if (command.compare("showMove") == 0) {
				cout << (string) *move << endl;
			} else if (command.compare("applyMove") == 0) {
				// applyMove code
				board->ApplyMove(move);
			} else if (command.compare("doMove") == 0) {
				// enterMove code (can abstract out if you want)
				getline(cin, cArg);
				*move = cArg.c_str();

				// applyMove code
				board->ApplyMove(move);
			} else if (command.compare("quit") == 0)
				break;
			else
				cout << "Unknown command: " << command << endl;

			cout << endl;
		} catch (BaseException exc) {
			cout << "Got exception... " << exc.what() << endl;
		}

	}

	delete board;

	return 0;
}