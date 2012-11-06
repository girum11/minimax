#ifndef OTHELLOMOVE_H
#define OTHELLOMOVE_H

#include <iostream>
#include <list>
#include <vector>
#include "OthelloBoard.h"

class OthelloMove : public Board::Move {
public:
   // Flipset represents a "run" of flips in a given direction.
   struct FlipSet {
      char count;                         // Number of flipped cells 
      const OthelloBoard::Direction *dir; // Direction of run.

      FlipSet(char c = 0, const OthelloBoard::Direction *d = 0)
       : count(c), dir(d) {}
   };

   friend class OthelloBoard;
   typedef std::list<FlipSet> FlipList;
   
   OthelloMove(char row = 0, char col = 0) : mRow(row), mCol(col) {}
   virtual ~OthelloMove() {}

   void AddFlipSet(const FlipSet &fs) 
    {mFlipSets.push_back(fs);}

   void ClearFlipSets()                {mFlipSets.clear();}
   const FlipList &GetFlipSets() const {return mFlipSets;}

   bool operator==(const Board::Move &rhs) const; 
   bool operator<(const Board::Move &rhs) const;
   operator std::string() const;
   void operator=(const std::string &src);
   Board::Move *Clone() const;

   bool IsPass() const {return mRow == -1 && mCol == -1;}

   short GetRow() const {return mRow;}
   short GetCol() const {return mCol;}

   void operator delete(void *p);
   void *operator new(size_t sz);

protected:
   std::istream &Read(std::istream &is);
   std::ostream &Write(std::ostream &) const;

   enum {bitShift = 4, bitMask = 0xF};

   char mRow;
   char mCol;
   FlipList mFlipSets;

   static std::vector<OthelloMove *> mFreeList;
};

#endif
