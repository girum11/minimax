/*
 * PylosView.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: girum
 */

#include "PylosView.h"
#include "PylosBoard.h"

using namespace std;

// TODO: Use Clint's FString instead of string in PylosView::Draw()
/*
   You will definitely want to use FString from MyLib.h to assemble strings 
   in at least some cases.  Also, note that std::string doesn't automatically 
   resize when you index it; it does so only if you use operations like append or +=.  
   The FString constructor is a variadic method, so you must pass arguments that 
   match the format specifiers precisely.  No conversions will be made for you; 
   mismatched types will simply cause mysterious failures.
*/
void PylosView::Draw(ostream &out) {
   string rtn;
   int level = 0, row = 0, col = 0;
   char sqr;
   const PylosBoard *pb = dynamic_cast<const PylosBoard *>(mModel);
   PylosBoard::Set mask;

   for (level = PylosBoard::kDim - 1; level >= 0; level--) {
      for (row = 0; row < PylosBoard::kDim - level; row++) {
         for (col = 0; col < PylosBoard::kDim - level; col++) {
            
            mask = pb->GetCell(row, col, level)->mask;

            if (mask & pb->mWhite)
               rtn += " W";
            else if (mask & pb->mBlack)
               rtn += " B";
            else
               rtn += " .";
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
