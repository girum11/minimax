#ifndef SIMPLEAIPLAYER_H
#define SIMPLEAIPLAYER_H

#include "BestMove.h"
#include "limits.h"

class Book;

class SimpleAIPlayer {
public:
   // Deepest allowed minimax level
   enum {SAVE_LEVEL = 1};


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
   // move.  Res->numBoards gives the number of boards examined during the
   // minimax computation, including the root board. *brd is left in the same
   // state as it was when the call commenced.  Res->value gives the
   // minimax computed value for *brd. *bk will contain new entries for any board 
   // configurations that were computed during the call, with depth greater than or 
   // equal to SAVE_LEVEL (currently 1), and will have updated any entries for 
   // which deeper results were computed during the call.
   static void Minimax(Board *brd, int lvl, long min, long max, BestMove *res,
    Book *bk, int debugLvl = 0);
};

#endif