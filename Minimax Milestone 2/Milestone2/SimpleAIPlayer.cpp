#include <fstream>
#include <assert.h>
#include "SimpleAIPlayer.h"
#include "Book.h"

using namespace std;

// ***************************************************************************
// Preconditions: 
// res points to a BestMove object, which may have NULL for its current 
// bestMove.  Any value V such that V <= min or V >= max is
// "uninteresting" to the caller.  
// 
// Level is always >= 1 and indicates how
// deeply to probe. (Note that this means you should evaluate leaf
// boards directly as they are generated, rather than making a FindBestMove
// call with level of 0 for each leaf board.  This is important for efficient
// running.)  
// 
// If book is not NULL, it points to a book of previously computed
// BestMoves, 
//
// 
// ***************************************************************************
// Postconditions: 
// res->value indicates the result of the minimax search.
// 
// If res->value <= min or res->value >= max then no "interesting" move could
// be found, and res->bestMove will indicate the first move where this fact
// was uncovered.
// 
// If board is an endgame board, then  res->value will
// be winVal, 0 or -winVal, and res->bestMove will be NULL since there is no move
// from an endgame position.  Otherwise, res->bestMove will provide the best
// move.  
// 
// res->numBoards gives the number of boards examined during the minimax 
// computation, including the root board. 
// 
// *board is left in the same state as it was when the call commenced.  
// 
// res->value gives the minimax computed value for *board.  
// 
// *book will contain new entries for any board configurations that were 
// computed during the call, with depth greater than or equal to SAVE_LEVEL, 
// and will have updated any entries for which
// deeper results were computed during the call.
// 
// ***************************************************************************
// (for Part D) 
// 
// res->bestReply gives the best answering move to res->bestMove, or NULL if 
// res->bestMove ends the game. 
// ***************************************************************************
// 
// [Me] This has double-depth recursion somewhere.

// TODO: Replace this code with the original SimpleAIPlayer from before,
// before you start to fill in the blanks.
void SimpleAIPlayer::Minimax(Board *board, // The original board you're
                             int level, 
                             long min, // The min before you're "uninterested" 
                             long max, // The max before you're "uninterested"
                             BestMove *res, // Current bestMove object that you fill
                             Book *book = NULL, // Map<Key
                             int dbg = 0) {
   list<Board::Move *> moves;
   list<Board::Move *>::iterator mItr;
   BestMove subRes(0, 0, level, 1);
   const Board::Key *key = 0;
   Book::iterator itr;
   pair<Book::iterator, bool> insRes;

   assert(level >= 1);

   // (The base case)
//    if (book && (itr = book->find(key = board->GetKey())) != book->end()
//     && ________________________________) {
//     
//       *res = _____________;
//       res->numBoards = 1;
//    }
//    else {
      board->GetAllMoves(&moves);
      *res = subRes;
      res->value = moves.size() == 0 ? ____________________________ :
       (board->GetWhoseMove() ? Board::kWinVal - 1 : -Board::kWinVal + 1);

      for (mItr = moves.begin(); ____________ && mItr != moves.end(); mItr++) {

         board->ApplyMove(*mItr);

         if (level == 1)
            subRes.value = board->GetValue();
         else
            Minimax(board, level-1, min, max, &subRes, book, dbg);

         if (board->GetWhoseMove() == 1 && ___________________) {
            res->value = min = subRes.value;
            res->SetBestMove(_______________________);
         }
         else if (board->GetWhoseMove() == 0 && __________________) {
            res->value = max = subRes.value;
            res->SetBestMove(_______________________);
         }

         if (dbg > 0) {
            for (int cnt = level-1; cnt > 0; cnt--)
               cout << "   ";
            cout << "Move " << (string)**mItr << " nets " << subRes.value
             << " min/max is " << min << "/" << max << endl;
         }

         board->UndoLastMove();
         res->numBoards += subRes.numBoards;
//      }

      for (; mItr != moves.end(); mItr++)
      
         _______________________;

//       if (book && level >= SAVE_LEVEL && _________________ && res->bestMove) {
//          insRes = book->insert(_____________________________);
//          if (insRes.second)
//             key = 0;
//          else if (_______________________________________) {
//             (*insRes.first).second = *res;
//          } 
//       }
   }
   delete key;
}


