#ifndef CHECKERSMOVE_H
#define CHECKERSMOVE_H

#include <iostream>
#include <list>
#include <vector>
#include <cctype>
#include <assert.h>
#include "Board.h"


class CheckersMove : public Board::Move {
public:

   enum {};

   typedef std::vector<std::pair<char,unsigned int> > LocVector;

   // TODO: Inner classes go here if you need them.

   friend class CheckersBoard;

   CheckersMove(const LocVector &locs) : mLocs(locs) {
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
//    Location mFrom;
//    Location mTo;

   static std::vector<CheckersMove *> mFreeList;


   void AssertMe() {

      // Ensure that this move is valid.
      for (LocVector::const_iterator locIter = mLocs.begin();
       locIter != mLocs.end(); locIter++) {

          // If it's a null move though, don't bounce it.
          if (locIter->first == -1 && locIter->second == -1) {
             return;
          }
          
         assert(isalpha(locIter->first) && isdigit(locIter->second));
      }
   }

};

#endif