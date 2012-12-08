
#include "Class.h"
#include "Book.h"
#include "Board.h"

using namespace std;



// [Staley] Write a program �MakeBook� that works like the sample executable 
// [Staley] provided.  MakeBook prompts for and accepts a single line of input 
// [Staley] of the form:
// [Staley] 
// [Staley] BoardClass minimaxLevel bookDepth fileName
// [Staley] 
// [Staley] MinimaxLevel is the minimax lookahead to use when determining a 
// [Staley] best move for each board key.  BookDepth indicates how many boards 
// [Staley] to generate.  Specifically, it tells how many halfmoves from the 
// [Staley] starting board should be covered by the bookFile file.  A minimaxLevel 
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
// [Staley] the specified fileName.  In milestone 3  you�ll use this bookFile file 
// [Staley] to avoid minimax computations for common board positions early in 
// [Staley] the game as described above. 
// [Staley] 
// [Staley] My MakeBook executable accepts a single commandline argument giving 
// [Staley] a �debugging log level�.  (Yours need not do so.)  The only allowed 
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
int main(int argc, char **argv) {
   // Restrictions:
   // 1. Your entire MakeBook.cpp can be at most 90 lines  (mine is 75).
   // Use the 'smartcount' bin in your CPE305 folder to measure this.
   // 
   // 2. No game name may appear in any of your source except for files with 
   // that game in their name.
   // 
   // TODO:
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
   // TODO: Pylos and Checkers need to incrementally increment the board value.
   const BoardClass *boardClass = NULL;
   Board *board = NULL;
   int minimaxLevel = -1, bookDepth = -1;
   string boardType(""), filename("");
   Book bookFile;
   
   // First, prompt the user for commands of the following usage:
   cout << "Enter boardType, level, depth, and filename: ";
   cin >> boardType >> minimaxLevel, bookDepth, filename;

   // Use reflection to instantiate the correct board, or give an error
   // message if that BoardClass type doesn't exist.
   if ((boardClass = BoardClass::ForName(argv[1])) == NULL
    || (board = dynamic_cast<Board *>(boardClass->NewInstance())) == NULL) {
      cout << "Unknown type " << argv[1] << endl;
      return -1;
   }

   // Create the "bookFile file."
   ConstructBookFileDFS(board, &bookFile, boardClass->UseTransposition(), 
    0, bookDepth);

   // When the bookFile is complete (after you finish running the DFS), write it
   // to a binary "bookFile file" having the specified fileName.
   


   // More hints for this are given in the spec.  E.g: Be sure to keep track of 
   // all the Keys you've collected thus far to avoid entering the same Key 
   // twice into the bookFile.
   


   return 0;
}


// A DFS on the initial Board configuration, where each node 
// is a Board configuration, and "exploring" the node consists of calling 
// the Minimax() method on that node to fill in what that configuration's 
// bestMove is, using the lookahead specified from the user in our prompt 
// from the beginning.
void ConstructBookFileDFS(Board *board,
                          Book *bookFile,
                          bool useX, // True if this BoardClass uses an XTable.
                          int currentDepth, 
                          int bookDepth) 
{
   // Get all of the children nodes.
   list<Board::Move *> allMoves;
   board->GetAllMoves(&allMoves);
   
   
   for (list<Board::Move *>::iterator moveIter = allMoves.begin();
    moveIter != allMoves.end(); moveIter++) {
      // The DFS should call itself until (currentDepth > bookDepth), not calling 
      // Minimax() until this is true.
      if (currentDepth < bookDepth) {

      }
      else {
         // Once you're ready to call Minimax(), create a new tTable for that
         // particular Minimax call (quoted from "Transposition Table" email).

      }
   }
   
   
   
   // TODO: Should it call Minimax() at leaf nodes?  That is, should I be
   // using GetAllMoves() to calculate if a node is a leaf node here in the DFS?
   // Probably not, but be sure about it first.


   // Clean up memory after your GetAllMoves() call.
   for (list<Board::Move *>::iterator moveIter = allMoves.begin(); 
    moveIter != allMoves.end(); moveIter++) {
      delete *moveIter;
   }
}