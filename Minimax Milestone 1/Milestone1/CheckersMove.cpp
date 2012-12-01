#include "MyLib.h"
#include "CheckersMove.h"
#include <iostream>
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

   // return mFrom == oRhs.mFrom && mTo == oRhs.mTo;
   return false;
}

bool CheckersMove::operator<(const Board::Move &rhs) const {
   const CheckersMove &oRhs = dynamic_cast<const CheckersMove &>(rhs);

   // return mFrom < oRhs.mFrom || mFrom == oRhs.mFrom && mTo < oRhs.mTo;
   return false;
}

CheckersMove::operator string() const {
   // TODO: Write this method out.  It's not as trivial as Othello though.

   // TODO: Be sure to handle the case when the move is equal to the
   // NULL_MOVE macro.  Clint's executable has a blank line output for
   // the first move in the Checkers game.

   return "\n";
}

void CheckersMove::operator=(const string &src) {
   string copy = src;
   unsigned start = copy.find_first_not_of(" \t"), 
    end = copy.find_last_not_of(" \t");
   LocVector locs;
   int index = 0, charsRead = 0;
   static const int kChunkSize = 10;
   bool readAllLocations = false;
   char arrow[2], trailingGarbage;
   
   // Strip all whitespace from the string
   copy.erase(remove_if(copy.begin(), copy.end(), isspace), copy.end());

   // Reserve a chunk of memory so that we can dynamically resize later
   locs.reserve(kChunkSize);
   locs.resize(1);

   // Read the first location of this move, throwing an exception if there's an
   // error.  You have to read the first location separately because the 
   // expected string doesn't have the "->" in it on the first one
   if (sscanf(copy.c_str(), "%c%u%n", &locs[0].first, &locs[0].second, &charsRead) != 2)
      throw BaseException(FString("Bad Checkers move: %s", copy.c_str()));

   // Erase the characters that we read
   copy.erase(0, charsRead);

   // Read all the rest of the locations of this move (it can chain jumps in
   // the case of a multiple jump).
   for (int index = 1; !readAllLocations; index++) {

      // If you need to, resize the vector by a chunk of memory.
      if (index >= locs.capacity()) {
         locs.reserve(locs.capacity() + kChunkSize);
      }

      // Then, resize the vector just enough to fit this new Location
      locs.resize(locs.size() + 1);

      // Scan the next Location
      // TODO: Deal with trailing garbage
      int res = sscanf(copy.c_str(), "%2c%c%u%n",
       arrow, &locs[index].first, &locs[index].second, &charsRead);

      // Erase the characters that we read
      copy.erase(0, charsRead);

      // If there is nothing left to scan, then stop reading in.
      if (res == 0) {
         readAllLocations = true;
      }
      // Otherwise, you read a partial move (or had a bad arrow token).
      else if (res != 3 || arrow[0] != '-' || arrow[1] != '>') {
          throw BaseException(FString("Bad Checkers move: %s", copy.c_str()));
      }
   }

   // Verify that none of the Locations are out of bounds
   for (LocVector::const_iterator locIter = locs.begin(); 
    locIter != locs.end(); locIter++) {

      // TODO: Magic number
      if (!InRange<char>('A', locIter->first, 'I') ||
       !InRange<unsigned int>(1, locIter->second, 9)) {
          throw BaseException(FString("Out of bounds Checkers move: %s", 
           copy.c_str()));
      }
   }

   // At the end (if you made it there without exception), copy the private
   // locs variable over into the mLocs of this object.
   mLocs = locs;
   AssertMe();
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

