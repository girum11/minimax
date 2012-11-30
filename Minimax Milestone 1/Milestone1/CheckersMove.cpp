#include "MyLib.h"
#include "CheckersMove.h"
#include <iostream>

using namespace std;

vector<CheckersMove *> CheckersMove::mFreeList;

void *CheckersMove::operator new(size_t sz) {
   void *temp;

   if (mFreeList.size()) {
      temp = mFreeList.back();
      mFreeList.pop_back();
   } else {
      temp = ::new char[sz];
   }

   mOutstanding++;
   return temp;
}

void CheckersMove::operator delete(void *p) {
   mFreeList.push_back((CheckersMove *)p);

   mOutstanding--;
}

bool CheckersMove::operator==(const Board::Move &rhs) const {
   const CheckersMove &oRhs = dynamic_cast<const CheckersMove &>(rhs);

   return mFrom == oRhs.mFrom && mTo == oRhs.mTo;
}

bool CheckersMove::operator<(const Board::Move &rhs) const {
   const CheckersMove &oRhs = dynamic_cast<const CheckersMove &>(rhs);

   return mFrom < oRhs.mFrom || mFrom == oRhs.mFrom && mTo < oRhs.mTo;
}

CheckersMove::operator string() const {
   // TODO: Write this method out.  It's not as trivial as Othello though.

   // TODO: Be sure to handle the case when the move is equal to the
   // NULL_MOVE macro.  Clint's executable has a blank line output for
   // the first move in the Checkers game.

   return "\n";
}

void CheckersMove::operator=(const string &src) {

}

Board::Move *CheckersMove::Clone() const {
   return new CheckersMove(*this);
}

istream &CheckersMove::Read(istream &is) {
   // TODO:
   return is;
}

ostream &CheckersMove::Write(ostream &os) const {
   // TODO:
   return os;
}

