
#include "Book.h"
#include "MyLib.h"
#include <iostream>

/* Hint: You’ll find the TCmpPtr template from MyLib.h useful here. */

using namespace std;

// The book file binary format is as follows, with all values BigEndian:
// 
// Minimax level at which book was calculated – 1 byte
//
// For each Key/BestMove pair:
// 
// Key (BigEndian binary write of all the BasicKey ulongs)
// 
// BestMove
// 
// ReplyMove exists flag 1 if there’s a Reply, 0 if not – 1 byte
//
// ReplyMove if one exists
// 
// Computed minimax value – 4 bytes
// 
// Do not write out depth of the BestMove, nor the number of boards explored.  
// When you read the book, assume  these two values are the “level” of the book 
// file, and 0, respectively.
// 
// I will require an exact match with my binary file only for Othello, not for 
// the other games.  Exact match of my text output is required for all games.


Book::~Book() {
   // TODO: Do I need to do anything here, or do I inherit from the 
   // std::map superclass?
}

// [Staley] Read/write book to and from a file in binary format.  
// [Staley] In Read, assume the Keys and Moves in the file are those for boardClass.
// [Staley]
// [Staley] When you read the book, assume that the depth of the BestMove is
// [Staley] the "level" of the book file.
// [Staley] Also, assume that the number of boards explored is 0.
std::istream &Book::Read(std::istream &is, const Class *boardClass) {
   char temp = '\0';
   Board *board = dynamic_cast<Board *>(boardClass->NewInstance());
   Board::Key *key = NULL;

   is.read(&temp, sizeof(mLevel));
   mLevel = temp;



   // Don't forget to clean up after you finish
   delete board;

   return is;
}

// [Staley] Read/write book to and from a file in binary format.
std::ostream &Book::Write(std::ostream &os) {

   os.write((char *)&mLevel, sizeof(mLevel));

   for (Book::const_iterator bookIter = this->begin();
    bookIter != this->end(); bookIter++) {
      
      // Write the key

      // Write the BestMove

      // TODO: If it exists, write the "replyMove exists" flag.  1 if it exists,
      // 0 if it does not.

      // TODO: If the aforementioned flag is set, write the replyMove

      // Write the computed minimax value for this BestMove



   }

   
   return os;
}