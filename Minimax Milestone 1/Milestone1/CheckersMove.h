#ifndef CHECKERSMOVE_H
#define CHECKERSMOVE_H

#include <iostream>
#include <cstdlib>
#include <cctype>
#include <list>
#include <vector>
#include "MyLib.h"
#include <assert.h>
#include "Board.h"

class CheckersMove : public Board::Move {
public:
   typedef std::pair<char, unsigned int> Location;
   typedef std::vector<Location> LocVector;

   friend class CheckersBoard;

   CheckersMove(const LocVector &locs, bool isJump)
    : mLocs(locs), mIsJumpMove(isJump), mIsKingMeMove(false) {}
   virtual ~CheckersMove() {}

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

   LocVector mLocs;
   bool mIsJumpMove, mIsKingMeMove;

   static std::vector<CheckersMove *> mFreeList;

   inline void CastToUpperAndVerify(Location *loc, std::string src);

   static inline bool IsJump(const Location &loc1, const Location &loc2) {
      return std::abs((int) ((int)loc1.first - (int)loc2.first)) == 2 &&
       std::abs((int) ((int)loc1.second - (int)loc2.second)) == 2;
   }

};

#endif