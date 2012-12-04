#ifndef CHECKERSMOVE_H
#define CHECKERSMOVE_H

#include <iostream>
#include <list>
#include <vector>
#include "MyLib.h"
#include <assert.h>
#include "Board.h"

class CheckersMove : public Board::Move {
public:

   enum {};

   typedef std::pair<char, unsigned int> Location;
   typedef std::vector<Location> LocVector;

   // TODO: Inner classes go here if you need them.

   friend class CheckersBoard;

   CheckersMove(const LocVector &locs, bool isJump)
    : mLocs(locs), mIsJumpMove(isJump), mIsKingMeMove(false) {
      AssertMe();
   }
   virtual ~CheckersMove() {}


   // TODO: Extra member functions go here if you need them.


   bool operator==(const Board::Move &rhs) const; 
   bool operator<(const Board::Move &rhs) const;
   operator std::string() const;
   void operator=(const std::string &src);
   Board::Move *Clone() const;
   
   void operator delete(void *p);
   void *operator new(size_t sz);

protected:

   std::istream &Read(std::istream &is);
   std::ostream &Write(std::ostream &) const;

   enum {};

   LocVector mLocs;
   bool mIsJumpMove;
   bool mIsKingMeMove;

   static std::vector<CheckersMove *> mFreeList;


   void AssertMe() {
      // Ensure that each Location used in this move is inbounds.
      for (LocVector::const_iterator locIter = mLocs.begin();
       locIter != mLocs.end(); locIter++) {
         assert(InRange<char>('A', locIter->first, 'I'));
         assert(InRange<unsigned int>(1, locIter->second, 9));
      }
   }

   static inline bool IsJump(const Location &loc1, const Location &loc2) {
      return abs(loc1.first - loc2.first) == 2 &&
       abs((int)loc1.second - (int)loc2.second) == 2;
   }

};

#endif