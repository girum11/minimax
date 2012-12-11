#include "CheckersView.h"
#include "CheckersBoard.h"

Class CheckersView::mClass("CheckersView", &CreateCheckersView);

void CheckersView::Draw(std::ostream &out) {
   const CheckersBoard *cb = dynamic_cast<const CheckersBoard *>(mModel);

   // Print the header
   out << "           White\n\n";

   // Print out the board itself.
   for (char row = 'H'; row >= 'A'; row--) {
      out << row << "  ";
      for (unsigned int col = 1; col <= CheckersBoard::kWidth; col++) {

         if (cb->CellOccupied(row, col, CheckersBoard::kBlack))
            out << (cb->CellContainsKing(row, col) ? "B " : "b ");
         else if (cb->CellOccupied(row, col, CheckersBoard::kWhite))
            out << (cb->CellContainsKing(row, col) ? "W " : "w ");
         else
            out << ".  ";
      }
      out << std::endl;
   }

   // Print out the column labels at the bottom
   out << "   ";
   for (unsigned int col = 1; col <= CheckersBoard::kWidth; col++)
      out << col << "  ";

   // Print out the footer
   out << "\n\n           Black\n\n";

   // Print out whose turn it is.
   if (cb->mWhoseMove == CheckersBoard::kWhite)
      out << "White's move" << std::endl;
   else if (cb->mWhoseMove == CheckersBoard::kBlack)
      out << "Black's move" << std::endl;
}