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

   return mRow == oRhs.mRow && mCol == oRhs.mCol;
}

bool CheckersMove::operator<(const Board::Move &rhs) const {
   const CheckersMove &oRhs = dynamic_cast<const CheckersMove &>(rhs);

   return mRow < oRhs.mRow || mRow == oRhs.mRow && mCol < oRhs.mCol;
}

CheckersMove::operator string() const {
   // TODO: Write this method out.  It's not as trivial as Othello though.
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

