#include "CheckersView.h"
#include "CheckersBoard.h"

Class CheckersView::mClass("CheckersView", &CreateCheckersView);

void CheckersView::Draw(std::ostream &out) {

   std::string rtn("");
   char row = 'A';
   unsigned int col = 1;
   const CheckersBoard *cb = dynamic_cast<const CheckersBoard *>(mModel);

   // Print the header
   out << "           White\n\n";

   // Print out the board itself.
   for (row = 'H'; row >= 'A'; row--) {
      out << row << "  ";
      for (col = 1; col <= CheckersBoard::kWidth; col++) {
         
         if (IsValidCell(row, col)
          && cb->CellOccupied(row, col, CheckersBoard::kBlack)) {
            if (cb->CellContainsKing(row,col)) {
               out << "B  ";
            } else {
               out << "b  ";
            }
         }
         else if (IsValidCell(row, col)
          && cb->CellOccupied(row, col, CheckersBoard::kWhite)) {
            if (cb->CellContainsKing(row,col)) {
               out << "W  ";
            } else {
               out << "w  ";
            }
         } else {
            out << ".  ";
         }
      }
      out << std::endl;
   }

   // Print out the column labels at the bottom
   out << "   ";
   for (col = 1; col <= CheckersBoard::kWidth; col++) {
      out << col << "  ";
   }

   // Print out the footer
   out << "\n\n           Black\n\n";

   // Print out whose turn it is.
   if (mModel->GetWhoseMove() == CheckersBoard::kWhite)
      out << "White's move" << std::endl;
   else if (mModel->GetWhoseMove() == CheckersBoard::kBlack)
      out << "Black's move" << std::endl;
}