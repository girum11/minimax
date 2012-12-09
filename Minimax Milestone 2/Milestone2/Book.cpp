
#include "Book.h"
#include "MyLib.h"
#include <iostream>

/* Hint: You’ll find the TCmpPtr template from MyLib.h useful here. */

using namespace std;




Book::~Book() {

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