#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <list>
#include "MyLib.h"
#include "Board.h"
#include "View.h"
#include "Dialog.h"
#include <exception>
#include <climits>
#include <cstdlib>

// Add more includes, possibly, but not board-specific ones
using namespace std;

// Prints a list, but does NOT delete it when it's done.
void PrintList(const list<const Board::Move *> *moves) {

   // If the list is empty, then drop what you're doing.
   if (moves->size() == 0) {
      return;
   }

   list<const Board::Move *>::const_iterator listIter;

   // Figure out the max move length
   unsigned maxCharLength = 0;
   for (listIter = moves->begin(); listIter != moves->end(); listIter++) {
      string moveString = (string) **listIter;
               
      if (moveString.size() > maxCharLength) {
         maxCharLength = moveString.size();
      }
   }

   // Print out all the moves
   unsigned col = 1;
   unsigned MAX_COLUMNS = 80 / (maxCharLength + 1);
   for (listIter = moves->begin(); listIter != moves->end(); listIter++) {
      // If adding this next move puts you over LINE_LENGTH,
      // then start a newline
      if (col > MAX_COLUMNS) {
         cout << endl;
         col = 1;
      }
               
      string moveString = (string) **listIter;
      cout << left << setw(maxCharLength) << moveString << " ";

      col++;
   }
   cout << endl;
}

void ApplyMove(Board *board, Board::Move *move) {
   // Verify that the move is in GetAllMoves() before applying it.
   list<Board::Move *> allMoves;
   board->GetAllMoves(&allMoves);
   bool foundMove = false;

   for (list<Board::Move *>::iterator listIter = allMoves.begin();
    listIter != allMoves.end(); listIter++) {
      if ((*move).operator==(**listIter)) {
         foundMove = true;
      }
      // Clean up after your GetAllMoves() call.
      delete *listIter;
   }

   if (!foundMove) {
      board->ApplyMove(move->Clone());
   } else {
      cout << "TRYING TO APPLY AN INVALID MOVE TO THE BOARD!!" << endl;
   }
}

int main(int argc, char **argv) {
	Board *board = NULL, *cmpBoard = NULL;
	Board::Move *move = NULL, *cmpMove = NULL, *temp = NULL;
	unsigned count = 0;
	string command, cArg;
	// Many more locals needed
   View *view = NULL;
   Dialog *dialog = NULL;
   list<Board::Move *> allMoves;
   const Board::Key *key = NULL, *cmpKey = NULL;

   // TODO: WARNING: This should be 80... but Clint insists that I have
   // something messed up on my end -- he thinks that I might not be using the 
   // max length of the strings.  Schedule office hours for this if necessary.
   const unsigned int LINE_LENGTH = 80;

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
   assert(board != NULL && view != NULL && dialog != NULL);
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

				// Retrieve all possible moves, mutable
				cout << "\nAll Moves: \n";
				board->GetAllMoves(&allMoves);

            // Print an immutable version of those moves
            PrintList((std::list<const Board::Move *> *)&allMoves);
            
            // TODO: Refactor this out.
            // Clean up after your GetAllMoves() call
            for (list<Board::Move *>::iterator listIter =  allMoves.begin();
             listIter != allMoves.end(); listIter++) {
                delete *listIter;
            }

            allMoves.clear();
			} else if (command.compare("enterMove") == 0) {
            getline(cin, cArg);

            // Make this move point to a new object.  Use a temporary
            // move in between as to prevent changing the default
            // move if operator=() fails.
            temp = board->CreateMove();
            (*temp).operator=(cArg.c_str());
            
            // Delete the old move
            delete move;
            move = temp->Clone();

            delete temp;
			} else if (command.compare("showMove") == 0) {
				cout << (string) *move << endl;
			} else if (command.compare("applyMove") == 0) {

            // TODO: Make your BoardTest check your GetAllMoves() list before
            // trying to applyMove, to ensure that you don't have bad test
            // cases.  Print out the c_str() that you tried to apply and
            // put it in all caps.  Be sure to comment it out before you handin
            // though, since checking "GetAllMoves()" every time you applyMove
            // is slow as balls.

            // This version of applyMove gives a clone of the default
            // move to the board.
            ApplyMove(board, move);
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
            boardClass->SetOptions(options);

            // TODO: delete options
            //free(options);

         } else if (command.compare("showVal") == 0) {
            cout << "Value: " << board->GetValue() << endl;
         } else if (command.compare("doMove") == 0) {
            getline(cin, cArg);

            // Make this move point to a new object.  Use a temporary
            // move in between as to prevent changing the default
            // move if operator=() fails.
            temp = board->CreateMove();
            (*temp).operator=(cArg.c_str());
            
            // Delete the old move
            delete move;
            move = temp->Clone();

            delete temp;

            ApplyMove(board, move);
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
            cout << "\nMove History: " << endl;
            const list<const Board::Move *> moveHist(board->GetMoveHist());
            PrintList(&moveHist);
         } else if (command.compare("compareKeys") == 0) {
            cmpBoard = dynamic_cast<Board *>(boardClass->NewInstance());
            cin >> cArg;
            ifstream in(cArg.c_str());
            assert(in.is_open());
            in >> *cmpBoard;

            // Grab references to the keys so that you can delete them after
            // this call
            key = board->GetKey();
            cmpKey = cmpBoard->GetKey();

            if (*key == *cmpKey) {
               cout << "Board keys are equal" << endl;
            } else {
               cout << "Board keys are unequal" << endl;

               if (*key < *cmpKey) {
                  cout << "Current board is less than " << cArg << endl;
               } else {
                  cout << "Current board is greater than " << cArg << endl;
               }
            }

            // Clean up what you left behind
            delete key;
            delete cmpKey;
            delete cmpBoard;
         } else if (command.compare("testPlay") == 0) {
            int seed = 0;
            unsigned moveCount = 0, selectedMove = 0;
            
            // Take a seed and moveCount, and apply the seed
            cin >> seed >> moveCount;
            srand(seed);

            while (moveCount-- > 0) {
               // Grab all possible moves
               assert(allMoves.size() == 0);
               board->GetAllMoves(&allMoves);

               // Ensure that the game is still going.
               if (allMoves.size() == 0) {
                  break;
               }

               // Pick and choose a move, if the game is still going
               selectedMove = rand() % allMoves.size();
               
               // Iterate over to that randomly selected move
               list<Board::Move *>::const_iterator iter = allMoves.begin();
               for (unsigned i = 0; i < selectedMove; i++) iter++;
               assert(iter != allMoves.end());

               // Apply the selected move to the game
               // WARNING: In case BoardTest.cpp is being tested by Bender,
               // watch that the default move isn't being modified by
               // this command.
               board->ApplyMove((*iter)->Clone());

               // Clean up after your GetAllMoves() call
               for (iter = allMoves.begin(); iter != allMoves.end(); iter++)
                   delete *iter;
               allMoves.clear();
            }
         } else if (command.compare("testRun") == 0) {
            int seed = 0;
            unsigned stepCount = 0, selectedMove = 0, movesToRetract = 0;
            
            // Take a seed and moveCount, and apply the seed
            cin >> seed >> stepCount;
            srand(seed);

            while (stepCount-- > 0) {
               // Grab all possible moves
               assert(allMoves.size() == 0);
               board->GetAllMoves(&allMoves);

               // Ensure that the game is still going.
               // If the game is over, then retract a random number of moves,
               // between [1,n] where n is the number of moves that you even 
               // have to retract (the move history).
               if (allMoves.size() == 0) {
                  movesToRetract = rand() % board->GetMoveHist().size() + 1;
                  
                  while (movesToRetract-- > 0)
                     board->UndoLastMove();
                  
                  continue;
               }

               // Pick and choose a move, if the game is still going
               selectedMove = rand() % allMoves.size();
               
               // Iterate over to that randomly selected move
               list<Board::Move *>::const_iterator iter = allMoves.begin();
               for (unsigned i = 0; i < selectedMove; i++) iter++;
               assert(iter != allMoves.end());

               // Apply the selected move to the game
               // WARNING: In case BoardTest.cpp is being tested by Bender,
               // watch that the default move isn't being modified by
               // this command.
               board->ApplyMove((*iter)->Clone());

               // Clean up after your GetAllMoves() call
               for (iter = allMoves.begin(); iter != allMoves.end(); iter++)
                   delete *iter;
               allMoves.clear();
            }

         } else if (command.compare("keyMoveCount") == 0) {
            cout << "Moves/Keys: " << Board::Move::GetOutstanding()
             << "/" << Board::Key::GetOutstanding() << endl;
         } else if (command.compare("quit") == 0) {
				break;
         }
			else {
				cout << "Unknown command: " << command << endl;
            cin.ignore(INT_MAX, '\n');
         }
		} catch (BaseException &exc) {
			cout << "Error: " << exc.what() << endl;
         
         // clean up after temp in case operator=() caught an error
         // TODO: Do I even need temp?
         delete temp;

		} catch (...) {
         cout << "Got some other exception... " << endl;
      }

      // Every command that you input, regardless of success or not,
      // should print a newline.
      cout << endl;
	}

	delete board;

   // TODO: If "quit" is the last line of the file, I stil output
   // this error message, even though I shouldn't.
   if (cin.eof())
      cout << "Error: Unexpected EOF" << endl << endl;

	return 0;
}
