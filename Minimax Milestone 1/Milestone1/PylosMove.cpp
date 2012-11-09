#include "MyLib.h"
#include "PylosMove.h"
#include "PylosBoard.h"
#include <assert.h>

using namespace std;

vector<PylosMove *> PylosMove::mFreeList;


void *PylosMove::operator new(size_t sz)
{
   // [Staley] Return next node from freelist, or allocate one
	void *temp;

	if (mFreeList.size()) {
	  temp = mFreeList.back();
	  mFreeList.pop_back();
	} else {
	  temp = ::new char[sz];
	}

	return temp;
}

void PylosMove::operator delete(void *p)
{
   // [Staley?] release node pointed to by p to the freelist
	mFreeList.push_back((PylosMove *)p);
}

bool PylosMove::operator==(const Board::Move &rhs) const
{
   const PylosMove &oRhs = dynamic_cast<const PylosMove &>(rhs);

   return mType == oRhs.mType && mLocs.size() == oRhs.mLocs.size();
// [Staley?]   Finish on this one line.  Use STL "equal" function template.
}

// [*Staley] Sort by target loc, then by move type, then source locs
// [*Staley] by lexicographic sort
bool PylosMove::operator<(const Board::Move &rhs) const
{
	const PylosMove &oRhs = dynamic_cast<const PylosMove &>(rhs);

	

	return false;
}

PylosMove::operator string() const
{
   string str;
   LocVector::const_iterator itr;
   
   if (mType == kReserve) {
      str = FString("Play at [%d, %d]", mLocs[0].first, mLocs[0].second);
      itr = mLocs.begin() + 1;
   }
   else if (mType == kPromote) {
       // [*Staley] Fill 

   } else {
	   // If mType isn't one of the enum types, then exit the program
	   assert(false);
   }
   
   if (itr != mLocs.end()) {
      str += FString(" taking [%d, %d]", (*itr).first, (*itr).second);
      itr++;
      // [*Staley] Another couple lines here
   }
   
   return str;
}

void PylosMove::operator=(const string &src)
{
   static const int kPlayOneParam = 3, kPlayTwoParams = 7, kPlayThreeParams = 11;
   
   char wd1[11], wd2[11], wd3[11], brack1, brack2, brack3;
   pair<short, short> p1, p2, p3, p4;
   int res;
   short type;
   LocVector temp;

   sscanf(src.c_str(), "%10s", wd1);
   if (!strcmp(wd1, "Play")) {
      type = kReserve;
      res = sscanf(src.c_str(), " Play at [ %hd , %hd %c %6s [ %hd , %hd %c %3s "
       "[ %hd , %hd %c %1s", &p1.first, &p1.second, &brack1, wd1, &p2.first,
         &p2.second, &brack2, wd2, &p3.first, &p3.second, &brack3);

      // *Staley] Test result of scanf for good format.  Had a total of 7 terms in this test.
      if (res == kPlayOneParam && brack1 == ']') {
      // [Staley]    Fill in temp
         temp.push_back(p1);
      } else if (res == kPlayTwoParams && brack1 == ']' && brack2 == ']') {
         temp.push_back(p1);
         temp.push_back(p2);
      } else if (res == kPlayThreeParams && 
       brack1 == ']' && brack2 == ']' && brack3 == ']') {
         temp.push_back(p1);
         temp.push_back(p2);
         temp.push_back(p3);
      }
      else
         throw BaseException(FString("Bad Pylos move: %s", src.c_str()));
   }
   else if (!strcmp(wd1, "Promote")) {
      // [*Staley] Similar logic for Promote
   }
   else
      throw BaseException(FString("Bad Pylos move: %s", src.c_str()));

   // [*Staley] About 5 lines of wrapup logic and final error checks needed here.
   mLocs = temp;
}

Board::Move *PylosMove::Clone() const
{
    // [Staley] Make this just one line long, a single relatively short 
	// [Staley] "return" statement.
	return 0;
}


// [Staley] release node pointed to by p to the freelist
ostream &PylosMove::Write(ostream &os) const
{
   char size = mLocs.size();
   short temp;
   int ndx;
   
   os.write((char *)&mType, sizeof(mType));
   os.write((char *)&size, sizeof(size));
   for (ndx = 0; ndx < size; ndx++) {
      temp = EndianXfer(mLocs[ndx].first);
      os.write((char *)&temp, sizeof(short));
      temp = EndianXfer(mLocs[ndx].second);
      os.write((char *)&temp, sizeof(short));
   }      
   return os;
}

// [Staley] Make this work with the (unchanged) Write method above.
istream &PylosMove::Read(istream &is)
{
   return is;
}
