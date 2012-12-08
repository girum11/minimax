
#include "Book.h"
#include "MyLib.h"
#include <iostream>

using namespace std;


/* 
Hint:
   The Book class works just like an STL map (think derivation), mapping 
   const Key pointers (like those you get back from GetKey) to BestMoves.  
   You’ll find the TCmpPtr template from MyLib.h useful here.  
*/



// [Staley] Read/write book to and from a file in binary format.  
// [Staley] In Read, assume the Keys and Moves in the file are those for boardClass.
std::istream &Book::Read(std::istream &is, const Class *boardClass) {


   return is;
}


// [Staley] Read/write book to and from a file in binary format.
std::ostream &Book::Write(std::ostream &os) {

   
   return os;
}