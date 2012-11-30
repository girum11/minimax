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
/* Initialize static member datum here                                  */
/************************************************************************/
BoardClass CheckersBoard::mClass("CheckersBoard",
                                 &CreateCheckersBoard,
                                 "Checkers",
                                 &CheckersView::mClass,
                                 &CheckersDlg::mClass,
                                 &CheckersBoard::SetOptions,
                                 &CheckersBoard::GetOptions);
// The C++ definition here isn't required in C++11, which I'm using.
// Put it there anyways to force the "static block" to run.
CheckersBoard::CheckersBoardInitializer CheckersBoard::mInitializer;

void CheckersBoard::StaticInit() {
   // TODO: Go back and initialize all of your static data here
   cout << "Static init called!" << endl;



}



CheckersBoard::CheckersBoard() : mWhoseMove(kBPiece) {
   // Just to make sure that I'm covering all my bases with member datum
   assert(mMoveHist.size() == 0);

   // TODO: Go back and initialize all of your nonstatic data here

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


Board *CheckersBoard::Clone() const {
   // [Staley] Think carefully about this one.  You should be able to do it in just
   // [Staley] 5-10 lines.  Don't do needless work

   return NULL;
}


Board::Key *CheckersBoard::GetKey() const {
   return NULL;
}


void *CheckersBoard::GetOptions() {
   // The caller of this method owns the object that is returned here.

   return NULL;
}

void CheckersBoard::SetOptions(const void *opts) {
   // The caller of this method owns the object that is returned here.


}

istream &CheckersBoard::Read(istream &is) {

   return is;
}

ostream &CheckersBoard::Write(ostream &os) const { 
   
   return os;
}
