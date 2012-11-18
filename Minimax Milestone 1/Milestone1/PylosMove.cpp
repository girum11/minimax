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

   // [Staley] Finish on this one line.  Use STL "equal" function template.
   return mType == oRhs.mType && mLocs.size() == oRhs.mLocs.size() 
      && mLocs == oRhs.mLocs;
}

// [*Staley] Sort by target loc, then by move type, then source locs
// [*Staley] by lexicographic sort
bool PylosMove::operator<(const Board::Move &rhs) const
{
   const PylosMove &oRhs = dynamic_cast<const PylosMove &>(rhs);
   
   if (mLocs[0] < oRhs.mLocs[0])
      return true;
   else if (mLocs[0] > oRhs.mLocs[0])
      return false;
   else {
      if (mType < oRhs.mType)
         return true;
      else if (mType > oRhs.mType)
         return false;
      else {
		  for (int i = 1; i < PylosBoard::kSqr; i++) {
			  // Same size comparisons
			  if (mLocs.size() > i && oRhs.mLocs.size() > i && mLocs[i] <= oRhs.mLocs[i])
				  return true;
			  else if (mLocs.size() > i && oRhs.mLocs.size() > i && mLocs[i] > oRhs.mLocs[i])
				  return false;
			  // Different size comparisons
			  else if (mLocs.size() <= i && oRhs.mLocs.size() > i)
				  return true;
			  else if (mLocs.size() > i && oRhs.mLocs.size() <= i)
				  return false;
           
         }
      }
   }

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
       // [Staley] Fill 
      str = FString("Promote from [%d, %d] to [%d, %d]", mLocs[1].first, 
         mLocs[1].second, mLocs[0].first, mLocs[0].second);
      itr = mLocs.begin() + 2;

   } else {
	   assert(false);
   }
   
   if (itr != mLocs.end()) {
      str += FString(" taking [%d, %d]", (*itr).first, (*itr).second);
      itr++;
      // [Staley] Another couple lines here
      if (itr != mLocs.end()) {
         str += FString(" and [%d, %d]", (*itr).first, (*itr).second);
         itr++;
      }
   }
   
   return str;
}

void PylosMove::operator=(const string &src)
{
   static const int kPlayOneParam = 3, kPlayTwoParams = 7, kPlayThreeParams = 11;
   static const int kPromoteTwoParams = 5, kPromoteThreeParams = 9, kPromoteFourParams = 13;
   
   char wd1[11], wd2[11], wd3[11], brack1, brack2, brack3;
   pair<short, short> p1, p2, p3, p4;
   int res;
   char type;
   LocVector temp;

   sscanf(src.c_str(), "%10s", wd1);
   if (!strcmp(wd1, "Play")) {
      type = kReserve;
      res = sscanf(src.c_str(), " Play at [ %hd , %hd %c %6s [ %hd , %hd %c %3s "
       "[ %hd , %hd %c %1s", &p1.first, &p1.second, &brack1, wd1, &p2.first,
         &p2.second, &brack2, wd2, &p3.first, &p3.second, &brack3, wd3);

      // [Staley] Test result of scanf for good format.  Had a total of 7 terms in this test.
      if (res == kPlayOneParam && brack1 == ']') {
      // [Staley] Fill in temp
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
      // [Staley] Similar logic for Promote
      type = kPromote;
      res = sscanf(src.c_str(), " Promote from [ %hd, %hd ] to [ %hd, %hd %c "
       "%6s [ %hd , %hd %c %3s [ %hd, %hd %c %1s", &p2.first, &p2.second, &p1.first,
       &p1.second, &brack1, wd1, &p3.first, &p3.second, &brack2, wd2, &p4.first,
       &p4.second, &brack3, wd3);

      // Test result of scanf for good format
      if (res == kPromoteTwoParams && brack1 == ']') {
         temp.push_back(p1);
         temp.push_back(p2);
      } else if (res == kPromoteThreeParams && brack1 == ']' && brack2 == ']') {
         temp.push_back(p1); 
         temp.push_back(p2);
         temp.push_back(p3);
      } else if (res == kPromoteFourParams && brack1 == ']' && brack2 == ']'
       && brack3 == ']') {
          temp.push_back(p1);
          temp.push_back(p2);
          temp.push_back(p3);
          temp.push_back(p4);
      } else
         throw BaseException(FString("Bad Pylos move: %s", src.c_str()));
   }
   else
      throw BaseException(FString("Bad Pylos move: %s", src.c_str()));

   // [Staley] About 5 lines of wrapup logic and final error checks needed here.
   mLocs = temp;
   mType = type;
   AssertMe();
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

void PylosMove::AssertMe() {  
   // Assert that the "doMove ... taking [a,b] and [c,d] cases ensure
   // that [a,b] < [c,d]
   if (mType == kReserve) assert(mLocs.size() <= 3);
   if (mType == kPromote) assert(mLocs.size() <= 4);
   if (mType == kReserve && mLocs.size() == 3) assert(mLocs[1] < mLocs[2]);
   if (mType == kPromote && mLocs.size() == 4) assert(mLocs[2] < mLocs[3]);
}
