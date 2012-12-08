#ifndef BOOK_H
#define BOOK_H

#pragma warning (disable: 4786)
#include <map>
#include <iostream>
#include "Class.h"
#include "BestMove.h"
#include "MyLib.h"
#include "Board.h"

class Book : std::map<const Board::Key *, BestMove> {
public:
	Book() : mLevel(0) {}
   virtual ~Book();

   int GetLevel()          {return mLevel;}
   void SetLevel(int val)  {mLevel = val;}
   
   // Read/write book to and from a file in binary format.  In Read,
   // assume the Keys and Moves in the file are those for brdCls.
	std::istream &Read(std::istream &is, const Class *brdCls);
	std::ostream &Write(std::ostream &os);

protected:
	short mLevel;
};

#endif
