#include <fstream>
#include <assert.h>
#include "SimpleAIPlayer.h"
#include "Book.h"

using namespace std;


// Preconditions: 
// bMove points to a BestMove object, which may have 0 for its current bestMove.  
// Any value V such that V <= min or V >= max is "uninteresting" to the caller.  
// 
// Level is always >= 1 and indicates how deeply to probe. (Note that this means
// you should evaluate leaf boards directly as they are generated, rather than 
// making a FindBestMove call with level of 0 for each leaf board.  This is 
// important for efficient running.)  
// 
// 'book' points to a book of previously computed BestMoves, or NULL.
//
// 
// Postconditions: 
// bMove->value indicates the result of the minimax search. If bMove->value <= min 
// or bMove->value >= max then no "interesting" move could be found, and 
// bMove->bestMove will indicate the first move where this fact was uncovered.  
// 
// If board is an endgame board, then  bMove->value will be winVal, 0 or -winVal, 
// and bMove->bestMove will be NULL since there is no move from an endgame 
// position.  Otherwise, bMove->bestMove will provide the best move.  
//
// bMove->bestReply gives the best answering move to bMove->bestMove, or NULL if
// bMove->bestMove ends the game.  
// 
// bMove->numBoards gives the number of boards examined during the minimax 
// computation, including the root board. 
// 
// *board is left in the same state as it was when the call commenced.  
// 
// bMove->value gives the minimax computed value for *board.  
// 
// *book will contain new entries for any board configurations that were 
// computed during the call, with depth greater than or equal to SAVE_LEVEL, 
// and will have updated any entries for which deeper results were computed 
// during the call.

void SimpleAIPlayer::Minimax(Board *board, int level, long min, long max,
 BestMove *bMove, Book *book, int debugFlag) {
//    list<Board::Move *> moves;
//    list<Board::Move *>::iterator moveIter;
//    BestMove subRes(0, 0, level, 1);
//    const Board::Key *key = 0;
//    Book::iterator bookIter;
//    pair<Book::iterator, bool> insRes;
// 
//    assert(level >= 1);
// 
//    // 
//    if (book && (bookIter = book->find(key = board->GetKey())) != book->end()
//     && ________________________________) {
//     
//       *bMove = _____________;
//       bMove->numBoards = 1;
//    }
//    else {
//       board->GetAllMoves(&moves);
//       *bMove = subRes;
//       bMove->value = moves.size() == 0 ? ____________________________ :
//        (board->GetWhoseMove() ? Board::kWinVal - 1 : -Board::kWinVal + 1);
// 
//       for (moveIter = moves.begin(); ____________ && moveIter != moves.end(); moveIter++) {
// 
//          board->ApplyMove(*moveIter);
// 
//          if (level == 1)
//             subRes.value = board->GetValue();
//          else
//             Minimax(board, level-1, min, max, &subRes, book, debugFlag);
// 
//          if (board->GetWhoseMove() == 1 && ___________________) {
//             bMove->value = min = subRes.value;
//             bMove->SetBestMove(_______________________);
//          }
//          else if (board->GetWhoseMove() == 0 && __________________) {
//             bMove->value = max = subRes.value;
//             bMove->SetBestMove(_______________________);
//          }
// 
//          if (debugFlag > 0) {
//             for (int cnt = level-1; cnt > 0; cnt--)
//                cout << "   ";
//             cout << "Move " << (string)**moveIter << " nets " << subRes.value
//              << " min/max is " << min << "/" << max << endl;
//          }
// 
//          board->UndoLastMove();
//          bMove->numBoards += subRes.numBoards;
//       }
// 
//       for (; moveIter != moves.end(); moveIter++)
//       
//          _______________________;
// 
//       if (book && level >= SAVE_LEVEL && _________________ && bMove->bestMove) {
//          insRes = book->insert(_____________________________);
//          if (insRes.second)
//             key = 0;
//          else if (_______________________________________) {
//             (*insRes.first).second = *bMove;
//          } 
//       }
//    }
//    delete key;
}


