#include "MyLib.h"
#include "OthelloMove.h"
#include "OthelloBoard.h"

using namespace std;

vector<OthelloMove *> OthelloMove::mFreeList;

void *OthelloMove::operator new(size_t sz)
{
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

void OthelloMove::operator delete(void *p)
{
   mFreeList.push_back((OthelloMove *)p);

   mOutstanding--;
}

bool OthelloMove::operator==(const Board::Move &rhs) const
{
   const OthelloMove &oRhs = dynamic_cast<const OthelloMove &>(rhs);

   return mRow == oRhs.mRow && mCol == oRhs.mCol;
}

bool OthelloMove::operator<(const Board::Move &rhs) const
{
   const OthelloMove &oRhs = dynamic_cast<const OthelloMove &>(rhs);

   return mRow < oRhs.mRow || mRow == oRhs.mRow && mCol < oRhs.mCol;
}

OthelloMove::operator string() const
{
   return mRow == -1 && mCol == -1 ? string("Pass") : FString("[%d, %d]", mRow, mCol);
}

void OthelloMove::operator=(const string &src)
{
   int start = src.find_first_not_of(" \t"), end = src.find_last_not_of(" \t");
   char tkn, tkn2;
   short tRow, tCol;

   if (start != string::npos &&
    src.substr(start, end-start+1).compare("Pass") == 0)
      tRow = tCol = -1;
   else if (3 != sscanf(src.c_str(),
    " [ %hd , %hd %c %c", &tRow, &tCol, &tkn, &tkn2) || tkn != ']')
      throw BaseException(FString("Bad Othello move: %s", src.c_str()));
   else if (!InRange<short>(0, tRow, OthelloBoard::dim)
    || !InRange<short>(0, tCol, OthelloBoard::dim))
      throw BaseException(FString("Out of bounds Othello move: %s",
       src.c_str()));

   mRow = (char)tRow;
   mCol = (char)tCol;
}

Board::Move *OthelloMove::Clone() const 
{
   return new OthelloMove(*this);
}

istream &OthelloMove::Read(istream &is)
{
   char size, count, dirNum;
   
   mFlipSets.clear();
   is.read(&mRow, sizeof(mRow));
   is.read(&mCol, sizeof(mCol));
   is.read((char *)&size, sizeof(size));
   while (is && size--) {
      is.read(&count, sizeof(count)).read(&dirNum, sizeof(dirNum));
      mFlipSets.push_back(
       FlipSet(count, OthelloBoard::mDirs + dirNum));
   }
      
   return is;
}

ostream &OthelloMove::Write(ostream &os) const
{
   char count, dirNum, size = mFlipSets.size();
   FlipList::const_iterator itr;

   os.write(&mRow, sizeof(mRow));
   os.write(&mCol, sizeof(mCol));
   os.write((char *)&size, sizeof(size));

   for (itr = mFlipSets.begin(); itr != mFlipSets.end(); itr++) {
      dirNum = (*itr).dir - OthelloBoard::mDirs;
      count = (*itr).count;
      os.write(&count, sizeof(count)).write(&dirNum, sizeof(dirNum));
   }

   return os;
}
