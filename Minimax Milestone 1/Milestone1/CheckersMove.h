#ifndef CHECKERSMOVE_H
#define CHECKERSMOVE_H

#include <iostream>
#include <list>
#include <vector>
#include "Board.h"


class CheckersMove : public Board::Move {
public:

   // TODO: Inner classes go here if you need them.

   friend class CheckersBoard;

   CheckersMove(char row = 0, char col = 0) : mRow(row), mCol(col) {
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

   char mRow;
   char mCol;

   static std::vector<CheckersMove *> mFreeList;

   // TODO: Write the equivalent of "IAmSane()" for a CheckersMove.
   void AssertMe() {

   }

};

#endif