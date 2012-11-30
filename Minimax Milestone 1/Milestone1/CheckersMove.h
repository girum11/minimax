#ifndef CHECKERSMOVE_H
#define CHECKERSMOVE_H

#include <iostream>
#include <list>
#include <vector>
#include <cctype>
#include <assert.h>
#include "Board.h"

#define NULL_CHAR -1
#define NULL_PAIR std::pair<char,char>(NULL_CHAR, NULL_CHAR)

class CheckersMove : public Board::Move {
public:

   enum {};

   typedef std::pair<char,char> CharPair;

   // TODO: Inner classes go here if you need them.

   friend class CheckersBoard;

   CheckersMove(CharPair from, CharPair to) : 
    mFrom(from), mTo(to) {
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

   CharPair mFrom;
   CharPair mTo;

   static std::vector<CheckersMove *> mFreeList;


   void AssertMe() {
      // Ensure that this move is valid.
      //
      // If it's a null move though, don't bounce it.
      if (this->mFrom == NULL_PAIR && this->mTo == NULL_PAIR) return;

      assert(isalpha(mFrom.first) && isdigit(mFrom.second));
      assert(isalpha(mTo.first) && isdigit(mTo.second));
   }

};

#endif