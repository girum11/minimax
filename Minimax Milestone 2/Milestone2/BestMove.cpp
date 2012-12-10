#include <assert.h>
#include "BestMove.h"

using namespace std;

const BestMove &BestMove::operator=(const BestMove &src) {
   delete move;
   move = src.move ? src.move->Clone() : NULL;

   delete replyMove;
   replyMove = src.replyMove ? src.replyMove->Clone() : NULL;
   
   value = src.value;
   depth = src.depth;
   numBoards = src.numBoards;
   
   return *this;
}

BestMove::BestMove(const BestMove &src){
   value = src.value;
   depth = src.depth;
   numBoards = src.numBoards;
   
   move = src.move ? src.move->Clone() : NULL;
   replyMove = src.replyMove ? src.replyMove->Clone() : NULL;
}

BestMove::~BestMove()
{
   delete move;
   delete replyMove;
}