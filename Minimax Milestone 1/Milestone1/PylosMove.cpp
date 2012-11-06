#include "MyLib.h"
#include "PylosMove.h"
#include "PylosBoard.h"

using namespace std;

void *PylosMove::operator new(size_t sz)
{
   // Return next node from freelist, or allocate one
}

void PylosMove::operator delete(void *p)
{
   // release node pointed to by p to the freelist
}

bool PylosMove::operator==(const Board::Move &rhs) const
{
   const PylosMove &oRhs = dynamic_cast<const PylosMove &>(rhs);

   return mType == oRhs.mType && mLocs.size() == oRhs.mLocs.size();
//   Finish on this one line.  Use STL "equal" function template.
}

// Sort by target loc, then by move type, then source locs
// by lexicographic sort
bool PylosMove::operator<(const Board::Move &rhs) const
{
}

PylosMove::operator string() const
{
   string str;
   LocVector::const_iterator itr;
   
   if (mType == kReserve) {
      str = FString("Play at [%d, %d]", mLocs[0].first, mLocs[0].second);
      itr = mLocs.begin() + 1;
   }
   else {
      // Fill in
   }
   
   if (itr != mLocs.end()) {
      str += FString(" taking [%d, %d]", (*itr).first, (*itr).second);
      itr++;
      // Another couple lines here
   }
   
   return str;
}

void PylosMove::operator=(const string &src)
{
   char wd1[11], wd2[11], wd3[11], brack1, brack2, brack3;
   pair<short, short> p1, p2, p3, p4;
   int res;
   short type;
   LocVector temp;

   sscanf(src.c_str(), "%10s", wd1);
   if (!strcmp(wd1, "Play")) {
      type = kReserve;
      res = sscanf(src.c_str(), " Play at [ %hd , %hd %c %6s [ %hd , %hd %c %3s "
       "[ %hd , %hd %c %1s", &p1.first, &p1.second, &brack1, wd1, &p2.first);
       // Fill in rest of sscanf.  Just one more line here is all that's needed.
      
      // Test result of scanf for good format.  Had a total of 7 terms in this test.
      //if () {
      //    Fill in temp
      //}
      //else
      //   throw BaseException(FString("Bad Pylos move: %s", src.c_str()));
   }
   else if (!strcmp(wd1, "Promote")) {
      // Similar logic for Promote
   }
   else
      throw BaseException(FString("Bad Pylos move: %s", src.c_str()));

   // About 5 lines of wrapup logic and final error checks needed here.
}

Board::Move *PylosMove::Clone() const
{
   // Make this just one line long, a single relatively short "return" statement.
}


// release node pointed to by p to the freelist
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

// Make this work with the (unchanged) Write method above.
istream &PylosMove::Read(istream &is)
{
   return is;
}
