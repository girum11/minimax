#include <iomanip>
#include <assert.h>
#include "CheckersView.h"
#include "CheckersBoard.h"


using namespace std;

Class CheckersView::mClass("CheckersView", &CreateCheckersView);

bool IsValidCell(char row, unsigned int col) {
   row -= 'A';
   col -= 1;

   return ((row % 2 == 0) && (col % 2 == 0))
    || ((row % 2 != 0) && (col % 2 != 0));
}

void CheckersView::Draw(ostream &out) {

   string rtn("");
   char row = 'A';
   unsigned int col = 1;
   const CheckersBoard *cb = dynamic_cast<const CheckersBoard *>(mModel);
   assert(cb != NULL); 
   cb->AssertMe();

   // Print the header
   out << setw(16) << "White" << endl << endl;

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
      out << endl;
   }

   // Print out the column labels at the bottom
   out << "   ";
   for (col = 1; col <= CheckersBoard::kWidth; col++) {
      out << col << "  ";
   }

   // Print out the footer
   out << endl << endl << setw(16) << "Black" << endl << endl;

   // Print out whose turn it is.
   if (mModel->GetWhoseMove() == CheckersBoard::kWhite) {
      out << "White's move" << endl;
   } else if (mModel->GetWhoseMove() == CheckersBoard::kBlack) {
      out << "Black's move" << endl;
   }
}