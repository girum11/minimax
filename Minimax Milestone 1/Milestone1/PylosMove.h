#ifndef PYLOSMOVE_H
#define PYLOSMOVE_H

#include <iostream>
#include <list>
#include <vector>
#include "PylosBoard.h"

// PylosMove represents one of two move types -- placement from
// the reserve to a row/col location, and promotion from a lower
// spot to a row/col location.  Both move types provide a list
// of 1 to 4 row/col pairs.  The first pair is always the
// target location.  In a promotion move, the second pair is
// the source location from which the promoted marble is taken.
// Note that row and col are all that is needed to specify a
// source or target location.  Level can always be inferred
// from the board configuration and the game rules.)
// For either move type, an additional one or two locations may
// be added if a recovery pattern is completed as a result of 
// the move.

class PylosMove : public Board::Move {
public:
   enum {kReserve, kPromote};  // Enum to mark what type of move we are.
   typedef std::vector<std::pair<short, short> > LocVector;
   friend class PylosBoard;
   
   PylosMove(const LocVector &locs, int type) : mLocs(locs), mType(type) {}
   virtual ~PylosMove() {}

   bool operator==(const Board::Move &rhs) const; 
   bool operator<(const Board::Move &rhs) const;
   operator std::string() const;
   void operator=(const std::string &src);
   Board::Move *Clone() const;

protected:
   std::istream &Read(std::istream &is);
   std::ostream &Write(std::ostream &) const;

   char mType;
   LocVector mLocs;

    void operator delete(void *p);
    void *operator new(size_t sz);

   // Static member datum to record freelist.  Use STL!
};

#endif

