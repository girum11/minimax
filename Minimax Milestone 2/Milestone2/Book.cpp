
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
std::istream &Book::Read(std::istream &is, const Class *boardClass) {
   
   

   return is;
}

// [Staley] Read/write book to and from a file in binary format.
std::ostream &Book::Write(std::ostream &os) {

   
   return os;
}