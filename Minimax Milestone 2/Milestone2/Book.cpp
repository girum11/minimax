
#include "Book.h"
#include "MyLib.h"
#include <iostream>
#include <assert.h>

/* Hint: You’ll find the TCmpPtr template from MyLib.h useful here. */

using namespace std;

Book::~Book() {
   for (Book::iterator bookIter = begin(); bookIter != end(); bookIter++) {
      delete bookIter->first;
   }
}

// [Staley] Read/write book to and from a file in binary format.  
// [Staley] In Read, assume the Keys and Moves in the file are those for boardClass.
// [Staley]
// [Staley] When you read the book, assume that the depth of the BestMove is
// [Staley] the "level" of the book file.
// [Staley] Also, assume that the number of boards explored is 0.
std::istream &Book::Read(std::istream &is, const Class *boardClass) {
//    Board *board = dynamic_cast<Board *>(boardClass->NewInstance());
//    Board::Key *key = NULL;
//    Board::Move *move = board->CreateMove();
//    //  *replyMove = board->CreateMove();
//    char tempFlag = 0;
//    long tempValue = 0;
// 
//    is.read((char *)&mLevel, sizeof(mLevel));
//    
//    while (!is.eof()) {
//       const Board::Key *tempKey = board->GetKey();
//       key = dynamic_cast<Board::Key *>(tempKey->GetClass()->NewInstance());
//       assert(key != NULL);
//       delete tempKey;
// 
//       is >> *key;
//       is >> *move;
//       
//       // Read in the "has reply move" flag, and accordingly make an attempt to
//       // read the replyMove or not.
// //       is.read((char *)&tempFlag, sizeof(tempFlag));
// //       if (tempFlag) {
// //          is >> *replyMove;
// //       }
// //       else
// //          replyMove = NULL;
// 
//       // Read 
//       is.read((char *)&tempValue, sizeof(tempValue));
//       
//       tempValue = EndianXfer(tempValue);
// 
//       insert(pair<const Board::Key *, BestMove>(
//        key, BestMove(move, 
//        // replyMove, 
//        NULL,
//        tempValue, mLevel, 0)));
//    }
// 
//    // Don't forget to clean up after you finish
//    delete board;

   return is;
}

// [Staley] Read/write book to and from a file in binary format.
std::ostream &Book::Write(std::ostream &os) {
//    long tempValue = 0;
//    char tempFlag = 0;
// 
//    // Write mLevel.
//    os.write((char *)&mLevel, sizeof(mLevel));
// 
//    for (Book::const_iterator bookIter = this->begin();
//     bookIter != this->end(); bookIter++) {
//       
//       // Write each key
//       os << *(bookIter->first);
// 
//       // Write each BestMove
//       // Can this also use the ostream operator like the Key above does?
//       os << *(bookIter->second.move);
//       // os.write((char *)&bookIter->second, sizeof(bookIter->second));
// 
// //       // If it exists, write each  "replyMove exists" flag.  Write the
// //       // number 1 if it exists, or 0 if it does not.
// //       tempFlag = bookIter->second.replyMove ? 1 : 0;
// //       os.write((char *)&tempFlag, sizeof(tempFlag));
// 
// //       // If the aforementioned flag is set, write the replyMove
// //       if (tempFlag)
// //          os << *(bookIter->second.replyMove);
// 
//       // Write the computed minimax value for this BestMove
//       tempValue = EndianXfer(bookIter->second.value);
//       os.write((char *)&tempValue, sizeof(bookIter->second.value));
//    }

   
   return os;
}