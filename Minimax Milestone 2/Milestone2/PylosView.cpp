#include "PylosView.h"
#include "PylosBoard.h"


Class PylosView::mClass("PylosView", &CreatePylosView);

void PylosView::Draw(std::ostream &out) {
   std::string rtn;
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