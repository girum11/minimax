#include <assert.h>
#include "BestMove.h"


using namespace std;


/* A class that has dynamic memory inside it that *it* owns is required to 
   specify its copy constructor, operator=(), and destructor.  */

BestMove::BestMove(const BestMove &src) {
   move = src.move ? move->Clone() : NULL;

   value = src.value;
   depth = src.depth;
   numBoards = src.numBoards;
}

const BestMove &BestMove::operator=(const BestMove &src) {
   delete move;
   move = src.move ? move->Clone() : NULL;

   value = src.value;
   depth = src.depth;
   numBoards = src.numBoards;

   return *this;
}
   
BestMove::~BestMove() {
   delete move;
}