/*
 * PylosView.cpp
 *
 *  Created on: Oct 29, 2012
 *      Author: girum
 */

#include "PylosView.h"
#include "PylosBoard.h"

using namespace std;

Class PylosView::mClass("PylosView", &CreatePylosView);

  
// [Staley] Uses an appropriate View-derived object to Draw a textual picture 
// [Staley] of the default board, and also prints a list of string-conversions 
// [Staley] of all moves possible from the default board.  ShowBoard calculates 
// [Staley] the largest move size in the list of possible moves, and then 
// [Staley] figures how many columns of moves of that largest size can be fit 
// [Staley] onto an 80-char screen, with one space after each column.  It then 
// [Staley] prints all the moves using that many columns, with moves 
// [Staley] left-justified within their column, so that they line up neatly.
void PylosView::Draw(ostream &out) {
   string rtn = "\n";
   int level = 0, row = 0, col = 0;
   const PylosBoard *pb = dynamic_cast<const PylosBoard *>(mModel);

   // Reserve some space for the output string so that you're not
   // reallocating over and over again.
   rtn.reserve(200);

   for (level = PylosBoard::kDim - 1; level >= 0; level--) {
      for (row = 0; row < PylosBoard::kDim - level; row++) {
         
         // "Center" this row's output
         for (int i = 0; i < level; ++i)
            rtn += " ";

         // Draw this row
         for (col = 0; col < PylosBoard::kDim - level; col++) {
            if (pb->CellOccupied(row, col, level, 1))
               rtn += "W ";
            else if (pb->CellOccupied(row, col, level, -1))
               rtn += "B ";
            else
               rtn += ". ";
         }
         // Step down to the next row
         rtn += "\n\n";
      }
   }
   rtn += pb->GetWhoseMove() ? "Black's move\n" : "White's move\n";

   out << rtn;
}