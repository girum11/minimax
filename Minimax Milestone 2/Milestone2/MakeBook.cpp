
#include <assert.h>
#include <climits>
#include <iostream>
#include <fstream>
#include "Class.h"
#include "Book.h"
#include "Board.h"
#include "SimpleAIPlayer.h"
#include "View.h"

using namespace std;

void ConstructBookFileDFS(Board *board,
                          View *view,
                          Book *bookFile,
                          bool useX, // True if this BoardClass uses an XTable.
                          int minimaxDepth,
                          int bookDepth);

// [Staley] Write a program “MakeBook” that works like the sample executable 
// [Staley] provided.  MakeBook prompts for and accepts a single line of input 
// [Staley] of the form:
// [Staley] 
// [Staley] BoardClass minimaxDepth bookDepth fileName
// [Staley] 
// [Staley] MinimaxLevel is the minimax lookahead to use when determining a 
// [Staley] best move for each board key.  BookDepth indicates how many boards 
// [Staley] to generate.  Specifically, it tells how many halfmoves from the 
// [Staley] starting board should be covered by the bookFile file.  A minimaxDepth 
// [Staley] of 5 and a bookDepth of 2 indicate that you should generate a best 
// [Staley] move for the initial board and all boards reachable from it in two 
// [Staley] halfmoves (something like 257 boards for Pylos), and that for each 
// [Staley] such board you should do a lookahead-5 minimax exploration to 
// [Staley] determine the best move. 
// [Staley] 
// [Staley] I generated the boards using recursion, and you'll almost certainly 
// [Staley] have to as well.  The optimal move for each board is determined by 
// [Staley] calling Minimax with the lookahead specified, so in fact MakeBook 
// [Staley] uses double-recursion. 
// [Staley] 
// [Staley] When the bookFile is complete, write it to a binary "bookFile file" having 
// [Staley] the specified fileName.  In milestone 3  you’ll use this bookFile file 
// [Staley] to avoid minimax computations for common board positions early in 
// [Staley] the game as described above. 
// [Staley] 
// [Staley] My MakeBook executable accepts a single commandline argument giving 
// [Staley] a “debugging log level”.  (Yours need not do so.)  The only allowed 
// [Staley] value presently is 1.  If you run MakeBook with no argument, you 
// [Staley] get the output that you must match.  However, if you run it thus:
// [Staley] 
// [Staley] MakeBook 1
// [Staley] 
// [Staley] then you get a blow-by-blow description of the minimax run, with 
// [Staley] each subcall indented according to its level.  The leaf calls 
// [Staley] are the leftmost, the root calls the rightmost.  Each call gets one 
// [Staley] line of output showing the move it evaluated, and the results it 
// [Staley] arrived at for that move.
// [Staley] 
// [Staley] You can differentiate between testing your MakeBook and your 
// [Staley] transposition table by using a non-transposition board type like 
// [Staley] MancalaBoard to test just the MakeBook and non-transposition 
// [Staley] minimax,
// [Staley] 
// [Staley] You will find it necessary to track all the Keys you've generated 
// [Staley] thus far as you fill in the bookFile, so as to avoid entering the same 
// [Staley] Key twice in the bookFile (two different sequences of moves might 
// [Staley] generate the same board).  Note the "Duplicate. No bookFile entry." 
// [Staley] announcements in my output.  You must have these in yours as well.
// [Staley] 
// [Staley] A hint on how to duplicate my key count:  When I analyze a board, I 
// [Staley] first get a key for it.  Then I output the current move/key count, 
// [Staley] and then I analyze the board using the key.
int main() {
   // Restrictions:
   // 1. Your entire MakeBook.cpp can be at most 90 lines  (mine is 75).
   // Use the 'smartcount' bin in your CPE305 folder to measure this.
   // 
   // 2. No game name may appear in any of your source except for files with 
   // that game in their name.
   // 
   // 3. Your MakeBook must work correctly with my Othello classes, object 
   // files for which are supplied.  I will require an exact match with my 
   // binary file only for Othello, not for the other games.  Exact match 
   // of my text output is required for all games.
   // 
   // 4. MakeBook.cpp may only use "new" for creating a Book object.  Boards, 
   // Views, etc. are created using Class and BoardClass.
   // 
   // 5. Your MakeBook executable must run in at most twice the time mine takes
   // for a given bookFile generation.
   const BoardClass *boardClass = NULL;
   View *view = NULL;
   Board *board = NULL;
   int minimaxDepth = -1, bookDepth = -1;
   string boardType(""), filename("");
   Book *bookFile = new Book();
   ofstream out;
   
   // First, prompt the user for commands of the following usage:
   cout << "Enter boardType, level, depth, and filename: ";
   cin >> boardType >> minimaxDepth >> bookDepth >> filename;

   // Use reflection to instantiate the correct board, or give an error
   // message if that BoardClass type doesn't exist.
   if ((boardClass = BoardClass::ForName(boardType)) == NULL
    || (board = dynamic_cast<Board *>(boardClass->NewInstance())) == NULL
    || (view = dynamic_cast<View *>(
    boardClass->GetViewClass()->NewInstance())) == NULL) {
      cout << "Unknown type " << boardType << endl;
      return -1;
   }
   bookFile->SetLevel(minimaxDepth);
   view->SetModel(board);

   // Create the "bookFile file".  This is where all the work happens.
   ConstructBookFileDFS(board, view, bookFile, boardClass->UseTransposition(), 
    minimaxDepth, bookDepth);

   // When the bookFile is complete (after you finish running the DFS), write it
   // to a binary "bookFile file" having the specified fileName.
   cout << "Writing book... ";
   out.open(filename.c_str());
   bookFile->Write(out);
   cout << "done" << endl;

   cout << "Before clearing book, moves/keys: " << Board::Move::GetOutstanding()
    << "/" << Board::Key::GetOutstanding() << endl;
   delete bookFile;
   cout << "Final count, moves/keys: " << Board::Move::GetOutstanding() << "/"
    << Board::Key::GetOutstanding() << endl;

   return 0;
}


// A DFS on the initial Board configuration, where each node 
// is a Board configuration, and "exploring" the node consists of calling 
// the Minimax() method on that node to fill in what that configuration's 
// bestMove is, using the lookahead specified from the user in our prompt 
// from the beginning.
void ConstructBookFileDFS(Board *board,
                          View *view,
                          Book *bookFile,
                          bool useX, // True if this BoardClass uses an XTable.
                          int minimaxDepth,
                          int bookDepth) 
{
   list<Board::Move *> allMoves;
   list<Board::Move *>::iterator moveIter = allMoves.begin();
   const Board::Key *key = NULL;
   BestMove bestMove;
   Book *tTable = new Book();

   // Output the current key/move count.
   // [Staley] A hint on how to duplicate my key count:  When I analyze a 
   // [Staley] board, I first get a key for it.  Then I output the current 
   // [Staley] move/key count, and then I analyze the board using the key.
   key = board->GetKey();
   view->Draw(cout);
   cout << "Moves/Keys: " << Board::Move::GetOutstanding() << "/" <<
    Board::Key::GetOutstanding() << endl;

   // Sanity check on pre-existing book -- won't do DFS in this case.
   if (bookFile->find(key) != bookFile->end()) {
      cout << "Duplicate. No book entry." << endl;
      delete key;
      return;
   }

   // Once you're ready to call Minimax(), create a new tTable for that
   // particular Minimax call (quoted from "Transposition Table" email).
   // TODO: Should this really be LONG_MIN and LONG_MAX?
   SimpleAIPlayer::Minimax(board, minimaxDepth, LONG_MIN, LONG_MAX, 
    &bestMove, useX ? tTable : NULL);

   // Report current stats to the user.
   cout << "Best move: " << (string) *bestMove.move << " with reply ";
   cout << (bestMove.replyMove ? (string) *bestMove.replyMove : "unknown") 
    << endl;
   cout << " Boards examined: " << bestMove.numBoards;
   cout << " Value: " << bestMove.value << endl;

   // Once you finish running the Minimax for that node, add the
   // bestMove that you got into your bookFile.
   bookFile->insert(pair<const Board::Key *, BestMove>(key, bestMove)).second;

   if (bookDepth > 0) {
      // Grab all of the possible moves (each move corresponds to a child node)
      board->GetAllMoves(&allMoves);

      for (moveIter = allMoves.begin(); moveIter != allMoves.end(); moveIter++) {
         board->ApplyMove(*moveIter);

         // The DFS should step down the tree in a depth-first manner until 
         // it reaches its desired bookDepth level.
         ConstructBookFileDFS(board, view, bookFile, useX, minimaxDepth, bookDepth-1);

         board->UndoLastMove();
      }  
   }
   
   // Clean up afterwards
   // TODO? Clean up after your GetAllMoves() call by iterating and deleting?
   // The last time I tried that though, my code broke down on me.
//    for (moveIter = allMoves.begin(); moveIter != allMoves.end(); moveIter++) {
//       delete *moveIter;
//    }
   delete tTable;
}