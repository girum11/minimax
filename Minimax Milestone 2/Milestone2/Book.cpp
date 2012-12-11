#include "Book.h"
#include "Board.h"
#include <assert.h>

using namespace std;

Book::~Book() {
   Book::iterator bookIter;
   for (bookIter = begin(); bookIter != end(); ++bookIter) {
      delete bookIter->first;
   }
}

// [Staley] Read/write book to and from a file in binary format.  
// [Staley] In Read, assume the Keys and Moves in the file are those for boardClass.
// [Staley]
// [Staley] When you read the book, assume that the depth of the BestMove is
// [Staley] the "level" of the book file.
// [Staley] Also, assume that the number of boards explored is 0.
istream &Book::Read(istream &is, const Class *boardClass) {
   Board *board = dynamic_cast<Board*>(boardClass->NewInstance());
   Board::Move *move = board->CreateMove(), *replyMove = board->CreateMove();
   Board::Key *key;
   char tempChar;
   long tempValue = 0;
   
   is.read(&tempChar, sizeof(tempChar));
   mLevel = tempChar;
   
   while (!is.eof()) {
      // Hack here, but eff it at this point  ~_~.
      key = const_cast<Board::Key *> (board->GetKey());
      is >> *key;
      is >> *move;
      is.read(&tempChar, sizeof(char));
      if (tempChar == 1) {
         is >> *replyMove;
      }
      is.read((char*)&tempValue, sizeof(tempValue));
      
      insert(value_type(key, BestMove(move->Clone(), tempChar == 1 ?
       replyMove->Clone() : NULL, EndianXfer(tempValue), mLevel, 0)));
   }
   
   delete board;
   delete move;
   
   return is;
}

// Same thing as Read(), but with less assumptions.
ostream &Book::Write(ostream &os) {
   Book::iterator bookIter;
   char tempChar = mLevel;
   long tempValue;
   
   os.write(&tempChar, sizeof(tempChar));
   
   for (bookIter = begin(); bookIter != end(); ++bookIter) {
      os << *(bookIter->first);
      os << *(bookIter->second.move);
      tempChar = bookIter->second.replyMove ? 1 : 0;
      os.write(&tempChar, sizeof(tempChar));
      if (tempChar == 1) {
         os << *(bookIter->second.replyMove);
      }
      
      tempValue = EndianXfer(bookIter->second.value);
      os.write((char*)&tempValue, sizeof(tempValue));
   }
   
   return os;
}