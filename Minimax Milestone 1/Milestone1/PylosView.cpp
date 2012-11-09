/*
 * PylosView.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: girum
 */

#include "PylosView.h"
#include "PylosBoard.h"

using namespace std;

void PylosView::Draw(ostream &out) {
   string rtn;
   int level = 0, row = 0, col = 0;
   char sqr;
   const PylosBoard *pb = dynamic_cast<const PylosBoard *>(mModel);

   for (level = PylosBoard::kDim - 1; level >= 0; level--) {
      for (row = 0; row < PylosBoard::kDim - level; row++) {
         for (col = 0; col < PylosBoard::kDim - level; col++) {
            rtn += ". ";
         }
         rtn += "\n";
      }
      rtn += "\n";
   }
   rtn += pb->GetWhoseMove() ? "Black's move\n" : "White's move\n";

   out << rtn;
}

// TODO: Implement reflection
const Class *PylosView::GetClass() const {
	return 0;
}
