#include <fstream>
#include <assert.h>
#include "SimpleAIPlayer.h"
#include "Book.h"

using namespace std;


// Preconditions: res points to a BestMove object, which may have 0 for
// its current bestMove.  Any value V such that V <= min or V >= max is
// "uninteresting" to the caller.  Level is always >= 1 and indicates how
// deeply to probe. (Note that this means you should evaluate leaf
// boards directly as they are generated, rather than making a FindBestMove
// call with level of 0 for each leaf board.  This is important for efficient
// running.)  If bk is not 0, it points to a book of previously computed
// BestMoves, 
//
// Postconditions: res->value indicates the result of the minimax search.
// If res->value <= min or res->value >= max then no "interesting" move could
// be found, and res->bestMove will indicate the first move where this fact
// was uncovered.  If brd is an endgame board, then  res->value will
// be winVal, 0 or -winVal, and res->bestMove will be 0 since there is no move
// from an endgame position.  Otherwise, res->bestMove will provide the best
// move.  Res->bestReply gives the best answering move to res->bestMove, or 0 if
// res->bestMove ends the game.  Res->numBoards gives the number of boards 
// examined during the minimax computation, including the root board. *brd is
// left in the same state as it was when the call commenced.  Res->value gives
// the minimax computed value for *brd.  *bk will contain new entries for any
// board configurations that were computed during the call, with depth greater
// than or equal to SAVE_LEVEL, and will have updated any entries for which
// deeper results were computed during the call.

void SimpleAIPlayer::Minimax(Board *brd, int level, long min, long max,
 BestMove *res, Book *bk, int dbg)
{
   list<Board::Move *> moves;
   list<Board::Move *>::iterator mItr;
   BestMove subRes(0, 0, level, 1);
   const Board::Key *key = 0;
   Book::iterator itr;
   pair<Book::iterator, bool> insRes;

   assert(level >= 1);

   if (bk && (itr = bk->find(key = brd->GetKey())) != bk->end()
    && ________________________________) {
    
      *res = _____________;
      res->numBoards = 1;
   }
   else {
      brd->GetAllMoves(&moves);
      *res = subRes;
      res->value = moves.size() == 0 ? ____________________________ :
       (brd->GetWhoseMove() ? Board::kWinVal - 1 : -Board::kWinVal + 1);

      for (mItr = moves.begin(); ____________ && mItr != moves.end(); mItr++) {

         brd->ApplyMove(*mItr);

         if (level == 1)
            subRes.value = brd->GetValue();
         else
            Minimax(brd, level-1, min, max, &subRes, bk, dbg);

         if (brd->GetWhoseMove() == 1 && ___________________) {
            res->value = min = subRes.value;
            res->SetBestMove(_______________________);
         }
         else if (brd->GetWhoseMove() == 0 && __________________) {
            res->value = max = subRes.value;
            res->SetBestMove(_______________________);
         }

         if (dbg > 0) {
            for (int cnt = level-1; cnt > 0; cnt--)
               cout << "   ";
            cout << "Move " << (string)**mItr << " nets " << subRes.value
             << " min/max is " << min << "/" << max << endl;
         }

         brd->UndoLastMove();
         res->numBoards += subRes.numBoards;
      }

      for (; mItr != moves.end(); mItr++)
      
         _______________________;

      if (bk && level >= SAVE_LEVEL && _________________ && res->bestMove) {
         insRes = bk->insert(_____________________________);
         if (insRes.second)
            key = 0;
         else if (_______________________________________) {
            (*insRes.first).second = *res;
         } 
      }
   }
   delete key;
}


