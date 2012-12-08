#include <fstream>
#include <assert.h>
#include "SimpleAIPlayer.h"
#include "Book.h"

using namespace std;

// Preconditions: 
// bestMove points to a BestMove object, which may have NULL for its current 
// move.  Any 'bestMove->value' V such that V <= min or V >= max is 
// "uninteresting" to the caller.  
// 
// 'level' is always >= 1 and indicates how deeply to probe. (Note that this 
// means you should evaluate leaf boards directly as they are generated, rather 
// than making a FindBestMove call with level of 0 for each leaf board.  This is 
// important for efficient running.)  
// 
// 'book' points to a book of previously computed BestMoves, or NULL.
//
// 
// Postconditions: 
// 'bestMove->value' indicates the result of the minimax search. If 
// bestMove->value <= min or bestMove->value >= max then no "interesting" move could 
// be found, and bestMove->move will indicate the first move where this fact 
// was uncovered.
// 
// If 'board' is an endgame board, then  bestMove->value will be winVal, 0 or 
// -winVal, and bestMove->move will be NULL since there is no move from an 
// endgame position.  Otherwise, bestMove->move will provide the best move.  
//
// 'bestMove->bestReply' gives the best answering move to bestMove->move, or NULL 
// if bestMove->move ends the game.  
// 
// 'bestMove->numBoards' gives the number of boards examined during the minimax 
// computation, including the root board. 
// 
// *board is left in the same state as it was when the call commenced.  
// 
// 'bestMove->value' gives the minimax computed value for *board.  
// 
// *book will contain new entries for any board configurations that were 
// computed during the call, with depth greater than or equal to SAVE_LEVEL, 
// and will have updated any entries for which deeper results were computed 
// during the call.

/* 
Emailed question:
   I'm a little confused about the transposition table for the minimax 
   milestone.
   Namely, when do we "create a new empty transposition table (Book) at the top  
   of each minimax exploration" (does this happen within the Minimax call, or 
   from MakeBook right before we call Minimax). When would we create another 
   transposition table?
   I think I'm mostly confused how that table interacts (or doesn't interact)    
   with the Book that MakeBook will eventually write out. There's only one 
   "MakeBook Book" ever, right?

Response:
   The transposition table is distinct from the book, and it should be 
   created at the top (before topmost call) of each recursive minimmax 
   computation.
*/

/* 
Hint: 
   The conditional "min < max" probably appears twice in the code. 
*/

/*
Hint:
   The optimal move for each board is determined by calling Minimax with the 
   lookahead specified, so in fact MakeBook uses double-recursion. 
*/

/*
Hint:
   If you are having trouble debugging, then recall that the MakeBook.cpp spec
   comments describe how you can set up a "debug" flag to help you debug.  It's
   not required, but it will help if you get stuck debugging.
*/

void SimpleAIPlayer::Minimax(Board *board, int level, long min, long max,
 BestMove *bestMove, Book *book, int debugFlag) {
   list<Board::Move *> moves;
   list<Board::Move *>::iterator moveIter;
   BestMove subBestMove(NULL, 0, level, 1);
   const Board::Key *key = 0;
   Book::iterator bookIter;
   pair<Book::iterator, bool> insRes;

   // [Staley] Level 0 computations aren’t worth it since a call of GetValue is 
   // [Staley] usually quicker than a book lookup.
   // [Me] So, ensure that MakeBook doesn't call this method with level == 0.
   assert(level >= 1);

// [Staley] On each minimax call, we consult the transposition table for any 
// [Staley] lookahead level above 0 (which is always done by directly calling 
// [Staley] GetValue, as in the code skeleton I supplied). If the table has a 
// [Staley] precomputed best move with level at least as deep as the one you 
// [Staley] need, then use it. 
// [Staley] Likewise, save the result of any minimax computations of level 1 
// [Staley] or greater in the book . And, 
// [Staley] very importantly, we update the table even if it already has a key 
// [Staley] for the board you’re computing, if your new computation is for a 
// [Staley] deeper lookahead level than the one in the book.

   // [Filled blank] Consult the transposition table to see if we already
   // have a precomputed best move "with level at least as deep as the one
   // you need."
   if (book && (bookIter = book->find(key = board->GetKey())) != book->end()
    && (*bookIter).second.depth >= level) {
    
      // [Filled blank] If we find the bestMove in the transposition table,
      // then set the bestMove straightaway.
      *bestMove = (*bookIter).second; 
      bestMove->numBoards = 1;
   }
   else {

      board->GetAllMoves(&moves);

      // Fill up bestMove -- assume that the bestMove for this node is
      // an empty BestMove.
      *bestMove = subBestMove;

      // If there are no more moves for this node, then 
      bestMove->value = moves.size() == 0 ? ____________________________ :
       (board->GetWhoseMove() ? Board::kWinVal - 1 : -Board::kWinVal + 1);

      for (moveIter = moves.begin(); ____________ && moveIter != moves.end(); 
       moveIter++) {

         board->ApplyMove(*moveIter);

         if (level == 1)
            subBestMove.value = board->GetValue();
         else
            Minimax(board, level-1, min, max, &subBestMove, book, debugFlag);

         if (board->GetWhoseMove() == 1 && ___________________) {
            bestMove->value = min = subBestMove.value;
            bestMove->SetBestMove(_______________________);
         }
         else if (board->GetWhoseMove() == 0 && __________________) {
            bestMove->value = max = subBestMove.value;
            bestMove->SetBestMove(_______________________);
         }

         if (debugFlag > 0) {
            for (int cnt = level-1; cnt > 0; cnt--)
               cout << "   ";
            cout << "Move " << (string)**moveIter << " nets " << subBestMove.value
             << " min/max is " << min << "/" << max << endl;
         }

         board->UndoLastMove();
         bestMove->numBoards += subBestMove.numBoards;
      }

      for (; moveIter != moves.end(); moveIter++)
      
         _______________________;

      if (book && level >= SAVE_LEVEL && _________________ && bestMove->move) {
         insRes = book->insert(_____________________________);
         if (insRes.second)
            key = 0;
         else if (_______________________________________) {
            (*insRes.first).second = *bestMove;
         } 
      }
   }
   delete key;
}


