#ifndef BOOK_H
#define BOOK_H

#include <map>
#include <iostream>
#include "Class.h"
#include "BestMove.h"
#include "MyLib.h"

class Book : public std::map<TCmpPtr<const Board::Key>, BestMove> {
public:
   Book() : mLevel(0) {}
   virtual ~Book();
   
   int GetLevel()          {return mLevel;}
   void SetLevel(int val)  {mLevel = val;}
   
   std::istream &Read(std::istream &is, const Class *brdCls);
   std::ostream &Write(std::ostream &os);
   
protected:
   short mLevel;
};

#endif