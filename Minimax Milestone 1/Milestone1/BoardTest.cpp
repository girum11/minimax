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
using namespace std;

int main(int argc, char **argv) {
	Board *board = NULL, *cmpBoard = NULL;
	Board::Move *move = NULL, *cmpMove = NULL;
	unsigned count = 0;
	string command, cArg;
	// Many more locals needed
   View *view = NULL;
   Dialog *dialog = NULL;

	// Sample of some Class code
	const Class *viewClass, *dialogClass;
	const BoardClass *boardClass;

   // Verify the number of arguments is correct
   if (argc != 2 && argc != 3) {
      cout << "Usage: BoardTest BoardClass [runLimit]" << endl;
      return -1;
   }

   // Attempt to instantiate the boardClass object
   boardClass = BoardClass::ForName(argv[1]);
   if (boardClass == NULL) {
      cout << "Failed to create classes or objects" << endl;
      return -1;
   }
   
   viewClass = boardClass->GetViewClass();
   dialogClass = boardClass->GetDlgClass();

   if (viewClass == NULL || dialogClass == NULL) {
      cout << "Failed to create classes or objects" << endl;
      return -1;
   }

   // Retrieve the instances of the objects from the Class objects
   board = dynamic_cast<Board *>(boardClass->NewInstance());
   view = dynamic_cast<View *>(viewClass->NewInstance());
   dialog = dynamic_cast<Dialog *>(dialogClass->NewInstance());
   move = board->CreateMove();

   if (board == NULL || view == NULL || dialog == NULL) {
      cout << "Failed to create classes or objects" << endl;
      return -1;
   }

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
					cout << (string) **listIter << " \n";
				}
			} else if (command.compare("enterMove") == 0) {
				// The exception is the exception that's thrown in the Board::Move member function.
				// It's this reason why we need to wrap this test scaffold up in a try-catch block.
				getline(cin, cArg);
            move = board->CreateMove();
				*move = cArg.c_str();
			} else if (command.compare("showMove") == 0) {
				cout << (string) *move << endl;
			} else if (command.compare("applyMove") == 0) {
				// applyMove code
				board->ApplyMove(move);
			} else if (command.compare("loadBoard") == 0) {
            cin >> cArg;
            ifstream in(cArg.c_str());
            assert(in.is_open());
            in >> *board;
         } else if (command.compare("saveMove") == 0) {
            cin >> cArg;
            ofstream out(cArg.c_str());
            assert(out.is_open());
            out << *move;
         } else if (command.compare("loadMove") == 0) {
            cin >> cArg;
            ifstream in(cArg.c_str());
            assert(in.is_open());
            in >> *move;
         } else if (command.compare("setOptions") == 0) {
            // Flush until '\n'
            cin.ignore(INT_MAX, '\n');

            // Run the dialog, using the Options object
            void *options = boardClass->GetOptions();
            dialog->Run(cin, cout, options);
            delete options;
         } else if (command.compare("showVal") == 0) {
            cout << "Value: " << board->GetValue() << endl;
         } else if (command.compare("doMove") == 0) {
				// enterMove code (can abstract out if you want)
            getline(cin, cArg);
            move = board->CreateMove();
				*move = cArg.c_str();

				// applyMove code
				board->ApplyMove(move);
			} else if (command.compare("compareMove") == 0) { 
            getline(cin, cArg);
            cmpMove = board->CreateMove();
            *cmpMove = cArg.c_str();

            // Figure out which move is greater
            string result;
            if (*move == *cmpMove)
               result = "Moves are equal";
            else if (*move < *cmpMove)
               result = "Current move is less than entered move";
            else if (!(*move < *cmpMove || *move == *cmpMove))
               result = "Current move is greater than entered move";
            else assert(false);

            cout << result << endl;

            // Clean up
            delete cmpMove;

         } else if (command.compare("showMoveHist") == 0) {
            cout << "Move History: " << endl;
            const list<const Board::Move *> moveHist = board->GetMoveHist();
            for (list<const Board::Move *>::const_iterator iter = moveHist.begin();
             iter != moveHist.end(); iter++) {
                cout << (string) **iter << ' ';
            }
            cout << endl;
         } else if (command.compare("compareKeys") == 0) {
            cmpBoard = dynamic_cast<Board *>(boardClass->NewInstance());
            cin >> cArg;
            ifstream in(cArg.c_str());
            assert(in.is_open());
            in >> *cmpBoard;

            if (*board->GetKey() == *cmpBoard->GetKey()) {
               cout << "Board keys are equal" << endl;
            } else {
               cout << "Board keys are unequal" << endl;

               if (*board->GetKey() < *cmpBoard->GetKey()) {
                  cout << "Current board is less than " << cArg << endl;
               } else {
                  cout << "Current board is greater than " << cArg << endl;
               }
            }
         } else if (command.compare("testPlay") == 0) {
            int seed = 0;
            unsigned moveCount = 0, selectedMove = 0;
            
            // Take a seed and moveCount, and apply the seed
            cin >> seed >> moveCount;
            srand(seed);

            while (moveCount-- > 0) {
               // Grab all possible moves
               list<Board::Move *> listOfMoves;
               board->GetAllMoves(&listOfMoves);

               // Pick one randomly (ensuring that the game is still going)
               if (listOfMoves.size() == 0) break;
               selectedMove = rand() % listOfMoves.size();
               
               // Iterate to that move
               list<Board::Move *>::const_iterator iter = listOfMoves.begin();
               for (int i = 0; i < selectedMove; i++) {
                  iter++;
               }
               assert(iter != listOfMoves.end());

               // WARNING: Bender watch -- should I set this move to be the
               // default move? (that is, set it to be the variable 'move' up
               // top...)
               // Apply the selected move to the game
               cout << "Move " << 1000 - moveCount << ": "
                << (string) **iter << endl;
               board->ApplyMove(*iter);
            }
         } else if (command.compare("testRun") == 0) {
           
            /*
            // TODO: testRun
            int seed = 0;
            unsigned stepCount = 0, selectedMove = 0;
            
            // Take a seed and moveCount, and apply the seed
            cin >> seed >> stepCount;
            srand(seed);

            while (stepCount-- > 0) {
               // Grab all possible moves
               // WARNING: Speedcheck -- maybe using instance variables
               // like this is a bad idea?  Maybe I should declare them up top?
               list<Board::Move *> listOfMoves;
               board->GetAllMoves(&listOfMoves);

               // If the game is over, retract 1-rand() steps backwards
               if (listOfMoves.size() == 0) {                  
                  count = rand() % listOfMoves.size() + 1;
                                 
				      while (count-- > 0)
					      board->UndoLastMove();

                  continue;
               }

               // Otherwise, pick a random move to do just like testPlay
               selectedMove = rand() % listOfMoves.size();
               list<Board::Move *>::const_iterator iter = listOfMoves.begin();
               for (int i = 0; i < selectedMove; i++) {
                  iter++;
               }
               assert(iter != listOfMoves.end());

               // WARNING: Bender watch -- should I set this move to be the
               // default move? (that is, set it to be the variable 'move' up
               // top...)
               // Apply the selected move to the game
               board->ApplyMove(*iter);
            } */
         } else if (command.compare("keyMoveCount") == 0) {
            // TODO: keyMoveCount
         } else if (command.compare("quit") == 0)
				break;
			else {
				cout << "Unknown command: " << command << endl;
            cin.ignore(INT_MAX, '\n');
			   cout << endl;
         }
		} catch (BaseException &exc) {
			cout << "BaseException: " << exc.what() << endl;
		} catch (...) {
         cout << "Got some other exception... " << endl;
      }

	}

	delete board;

	return 0;
}