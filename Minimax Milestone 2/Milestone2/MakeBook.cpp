

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




   return 0;
}