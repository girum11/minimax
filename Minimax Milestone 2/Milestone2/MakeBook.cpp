
#include "Class.h"
#include "Board.h"

using namespace std;

// [Staley] Write a program “MakeBook” that works like the sample executable 
// [Staley] provided.  MakeBook prompts for and accepts a single line of input 
// [Staley] of the form:
// [Staley] 
// [Staley] BoardClass minimaxLevel bookDepth fileName
// [Staley] 
// [Staley] MinimaxLevel is the minimax lookahead to use when determining a 
// [Staley] best move for each board key.  BookDepth indicates how many boards 
// [Staley] to generate.  Specifically, it tells how many halfmoves from the 
// [Staley] starting board should be covered by the book file.  A minimaxLevel 
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
// [Staley] When the book is complete, write it to a binary "book file" having 
// [Staley] the specified fileName.  In milestone 3  you’ll use this book file 
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
// [Staley] thus far as you fill in the book, so as to avoid entering the same 
// [Staley] Key twice in the book (two different sequences of moves might 
// [Staley] generate the same board).  Note the "Duplicate. No book entry." 
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
   // for a given book generation.
   // TODO: Pylos and Checkers need to incrementally increment the board value.
   BoardClass *boardClass = NULL;
   Board *board = NULL;
   int minimaxLevel = -1, bookDepth = -1;
   string fileName("");

   // First, prompt the user for a SINGLE LINE OF INPUT of the following usage:
   //       BoardClass minimaxLevel bookDepth fileName


   // Use reflection to figure out which Board you have, or give an error
   // message if that BoardClass type doesn't exist.

   // Then, figure out if the board that you were given can benefit from using a
   // transposition table.  Use the UseTranspositionTable() method from
   // BoardClass.

   // Create the "book file."  This file is simply the initial 'dictionary' of
   // good initial moves to play.
   // To construct it, you should do a DFS on the initial Board configuration,
   // where each node is a Board configuration, and "exploring" the node
   // consists of calling the Minimax() method on that node to fill in what
   // that configuration's bestMove is, using the lookahead specified from the
   // user in our prompt from the beginning.  Run the DFS of Board configs 
   // down to depth of 'bookDepth' (also specified by the user from before).


   // When the book is complete (after you finish running the DFS), write it
   // to a binary "book file" having the specified fileName.






   // More hints for this are given in the spec.  E.g: Be sure to keep track of 
   // all the Keys you've collected thus far to avoid entering the same Key 
   // twice into the book.
   


   return 0;
}