#include <assert.h>
#include <memory.h>
#include "CheckersDlg.h"
#include "CheckersView.h"
#include "CheckersMove.h"
#include "CheckersBoard.h"
#include "MyLib.h"
#include "BasicKey.h"

using namespace std;


/************************************************************************/
/* Initialize static member datum                                       */
/************************************************************************/


BoardClass CheckersBoard::mClass("CheckersBoard",
                                 &CreateCheckersBoard,
                                 "Checkers",
                                 &CheckersView::mClass,
                                 &CheckersDlg::mClass,
                                 &CheckersBoard::SetOptions,
                                 &CheckersBoard::GetOptions);

CheckersBoard::CheckersBoard() : mNextMove(mBPiece) {
   // Just to make sure that I'm covering all my bases with member datum
   assert(mMoveHist.size == 0);

   // TODO: Initialize mWhite, mBlack and mKings


}

CheckersBoard::~CheckersBoard() {
   // Handle anything you need to in the destructor

}

long CheckersBoard::GetValue() const {
   return 0;
}

void CheckersBoard::ApplyMove(Move *move) {


}

void CheckersBoard::UndoLastMove() {
   // This should basically be ApplyMove() backwards.

}

void CheckersBoard::GetAllMoves(list<Move *> *moves) const {
   // Don't forget that the caller of this method retains ownership of
   // the list<Move *> * that you pass back.  This means that I have to
   // "new" one.

}

Board::Move *CheckersBoard::CreateMove() const {
   return new CheckersMove(NULL_PAIR, NULL_PAIR);
}




