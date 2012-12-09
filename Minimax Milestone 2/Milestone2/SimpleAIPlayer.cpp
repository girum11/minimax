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
// 'minimaxLevel' is always >= 1 and indicates how deeply to probe. (Note that this 
// means you should evaluate leaf boards directly as they are generated, rather 
// than making a FindBestMove call with minimaxLevel of 0 for each leaf board.  This is 
// important for efficient running.)
// 
// 'tTable' points to a tTable of previously computed BestMoves, or NULL.
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
// *tTable will contain new entries for any board configurations that were 
// computed during the call, with depth greater than or equal to SAVE_LEVEL, 
// and will have updated any entries for which deeper results were computed 
// during the call.

/* 
Hint: 
   The conditional "min < max" probably appears twice in the code. 
*/

/*
Hint:
   If you are having trouble debugging, then recall that the MakeBook.cpp spec
   comments describe how you can set up a "debug" flag to help you debug.  It's
   not required, but it will help if you get stuck debugging.
*/

void SimpleAIPlayer::Minimax(Board *board, int minimaxLevel, long min, long max,
 BestMove *bestMove, Book *tTable, int debugFlag) {
   list<Board::Move *> moves;
   list<Board::Move *>::iterator moveIter;
   BestMove subBestMove(NULL, NULL, 0, minimaxLevel, 1);
   const Board::Key *key = 0;
   Book::iterator bookIter;
   pair<Book::iterator, bool> insRes;

   // [Staley] Level 0 computations aren’t worth it since a call of GetValue is 
   // [Staley] usually quicker than a tTable lookup.
   // [Me] So, ensure that MakeBook doesn't call this method with minimaxLevel == 0.
   assert(minimaxLevel >= 1);

   // Before we begin "exploring" this node, first consult the transposition 
   // table to see if we already have a precomputed best move for its
   // board configuration [Filled blank] "with minimaxLevel at least as deep as the 
   // one you need."
   if (tTable && (bookIter = tTable->find(key = board->GetKey())) != tTable->end()
    && (*bookIter).second.depth >= minimaxLevel) {
      // [Filled blank] If we find the bestMove in the transposition table,
      // then set the bestMove straightaway.
      *bestMove = (*bookIter).second;
      bestMove->numBoards = 1;
   }
   else {
      // To begin "exploring" this node, first figure out what the list of
      // possible moves is, so that you can construct the nodes at the minimaxLevel
      // below you (one node created per Move).
      board->GetAllMoves(&moves);

      // Fill up bestMove -- assume that the bestMove for this node is an empty 
      // BestMove.
      *bestMove = subBestMove;

      // Edge case: If this node is an end-game node, then set this bestMove's 
      // value to be the appropriate kWinVal.
      // [Filled blank] Otherwise, bestMove->value should just be the current
      // value of the board.
      bestMove->value = moves.size() == 0 ? board->GetValue() :
       (board->GetWhoseMove() ? Board::kWinVal - 1 : -Board::kWinVal + 1);

      // Iterate through each of the possible moves, [Filled blank] provided
      // that the limits for this node haven't collided yet.
      for (moveIter = moves.begin(); min < max && moveIter != moves.end(); 
       moveIter++) {

         board->ApplyMove(*moveIter);

         // Base case.  If the minimax recursion can't possibly go up another
         // level because you're at Level 1, then stop recursing up.
         if (minimaxLevel == 1)
            subBestMove.value = board->GetValue();
         else
            Minimax(board, minimaxLevel-1, min, max, &subBestMove, tTable, debugFlag);

         // TODO: Ensure that each of my Milestone 1 Board's GetWhoseMove() 
         // calls comply with this code.  They don't right now.
         // [Filled blank] White pulls the floor up.
         if (board->GetWhoseMove() == 1 && subBestMove.value > min) {
            bestMove->value = min = subBestMove.value;
            bestMove->SetBestMove((*moveIter)->Clone());
            bestMove->SetReplyMove(subBestMove.move);
         }
         // [Filled blank] Black pushes the ceiling down.
         else if (board->GetWhoseMove() == 0 && subBestMove.value < max) {
            bestMove->value = max = subBestMove.value;
            bestMove->SetBestMove((*moveIter)->Clone());
            bestMove->SetReplyMove(subBestMove.move);
         }

         if (debugFlag > 0) {
            for (int cnt = minimaxLevel-1; cnt > 0; cnt--)
               cout << "   ";
            cout << "Move " << (string)**moveIter << " nets " << subBestMove.value
             << " min/max is " << min << "/" << max << endl;
         }

         board->UndoLastMove();
         bestMove->numBoards += subBestMove.numBoards;
      }

      // [Filled blank] Clean up after your GetAllMoves() call.
      for (; moveIter != moves.end(); moveIter++)
         delete *moveIter;

      // [Filled blank] From the loop before, a min/max limits collision doesn't
      // return, but breaks instead (to provide time to clean up the 
      // GetAllMoves() call.  Thus, you have to ensure that the tTable isn't
      // added if you had a min/max collision.
      if (tTable && minimaxLevel >= SAVE_LEVEL && min < max && bestMove->move) {
         // [Filled blank] Insert the key->bestMove mapping into the map.
         insRes = tTable->insert(pair<const Board::Key *, BestMove>(key, *bestMove));
         
         // If you successfully inserted the key, then nil out the pointer to
         // it before you accidentally delete it after this else{} finishes.
         if (insRes.second)
            key = 0;
         // [Filled blank] "And, very importantly, we update the table 
         // even if it already has a key for the board you’re computing, if 
         // your new computation is for a deeper lookahead minimaxLevel than 
         // the one in the tTable."
         else if ((*insRes.first).second.depth < minimaxLevel) {
            (*insRes.first).second = *bestMove;
         } 
      }
   }
   delete key;
}


