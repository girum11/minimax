#include "MyLib.h"
#include "CheckersMove.h"
#include <iostream>
#include <cctype>
#include <algorithm>

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

   return mLocs == oRhs.mLocs;
}

bool CheckersMove::operator<(const Board::Move &rhs) const {
   const CheckersMove &oRhs = dynamic_cast<const CheckersMove &>(rhs);

   // return mFrom < oRhs.mFrom || mFrom == oRhs.mFrom && mTo < oRhs.mTo;
   return false;
}

CheckersMove::operator string() const {
   string rtn = "";

   // Handling the case when the move is empty.  Clint's executable has a blank 
   // line output for the first move in the Checkers game.
   if (mLocs.size() == 0) {
      return rtn;
   }

   LocVector::const_iterator locIter = mLocs.begin();
   rtn += FString("%c%u", locIter->first, locIter->second);
   locIter++;

   for (; locIter != mLocs.end(); locIter++) {
      rtn += FString(" -> %c%u", locIter->first, locIter->second);
   }

   return rtn;
}

void CheckersMove::operator=(const string &src) {
   string copy = src;
   unsigned start = copy.find_first_not_of(" \t"), 
    end = copy.find_last_not_of(" \t");
   LocVector locs;
   unsigned int index = 0, charsRead = 0, res = 0;
   static const int kChunkSize = 10;
   bool readAllLocations = false;
   char arrow[2], trailingGarbage;

   // Reserve a chunk of memory so that we can dynamically resize later
   locs.reserve(kChunkSize);
   locs.resize(1);

   // Read the first location of this move, throwing an exception if there's an
   // error.  You have to read the first location separately because the 
   // expected string doesn't have the "->" in it on the first one
   res = sscanf(copy.c_str(), " %c%1u%n", 
    &locs[0].first, &locs[0].second, &charsRead);
   if (res != 2)
      throw BaseException(FString("Bad Checkers move: %s", src.c_str()));

   // Erase the characters that we read
   copy.erase(0, charsRead);

   // Read all the rest of the locations of this move (it can chain jumps in
   // the case of a multiple jump).
   for (index = 1; !readAllLocations && copy.size(); index++) {

      // If there isn't even an arrow in the remainder of the string, then
      // you certainly have garbage.  If there's an arrow but nothing else
      // for it to point to, then the 'res' check later will pick it up.
      if (copy.find_first_of("->") == string::npos) {
         throw BaseException(FString("Bad Checkers move: %s", src.c_str()));
      }

      // If you need to, resize the vector by a chunk of memory.
      if (index >= locs.capacity()) {
         locs.reserve(locs.capacity() + kChunkSize);
      }

      // Then, resize the vector just enough to fit this new Location.
      // Since I've already made sure that there's at least another "->" left
      // in the string, I am guaranteed not to increment locs more than what is
      // needed for the sscanf on the next line.
      locs.resize(locs.size() + 1);

      // Scan the next Location
      res = sscanf(copy.c_str(), " -> %c%1u%n", 
       &locs[index].first, &locs[index].second, &charsRead);

      // Check if is a malformed location.
      if (res != 2) {
          throw BaseException(FString("Bad Checkers move: %s", src.c_str()));
      }
      
      // Erase the characters that we read
      copy.erase(0, charsRead);
      
      // If there is nothing left to scan (only whitespace left), then break.
      if (copy.find_first_not_of(" \t") == string::npos) {
         readAllLocations = true;
      }

   }

   // If you only read in one location, then this is an invalid move.
   if (locs.size() < 2) {
      throw BaseException(FString("Bad Checkers move: %s", src.c_str()));
   }

   for (LocVector::iterator locIter = locs.begin(); 
    locIter != locs.end(); locIter++) {

      // Ensure that the letters are uppercase
      locIter->first = toupper(locIter->first);
      
      // Verify that none of the Locations are out of bounds
      // TODO: Magic number
      if (!InRange<char>('A', locIter->first, 'I') ||
       !InRange<unsigned int>(1, locIter->second, 9)) {
          throw BaseException(FString("Out of bounds Checkers move: %s", 
           src.c_str()));
      }
   }

   // At the end (if you made it there without exception), copy the private
   // locs variable over into the mLocs of this object.
   mLocs = locs;
   AssertMe();

   // Set whether or not this is a jump move
   if (IsJump(mLocs[0], mLocs[1]))
      mIsJumpMove = true;
   else
      mIsJumpMove = false;
}

Board::Move *CheckersMove::Clone() const {
   return new CheckersMove(this->mLocs, mIsJumpMove);
}

istream &CheckersMove::Read(istream &is) {
   // TODO:
   return is;
}

ostream &CheckersMove::Write(ostream &os) const {
   // TODO:
   return os;
}

