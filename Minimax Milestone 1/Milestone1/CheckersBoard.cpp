#include <assert.h>
#include <vector>
#include <memory.h>
#include <cmath>
#include "CheckersDlg.h"
#include "CheckersView.h"
#include "CheckersMove.h"
#include "CheckersBoard.h"
#include "MyLib.h"
#include "BasicKey.h"

using namespace std;


/************************************************************************/
/* Declare/initialize static member datum here                          */
/************************************************************************/
CheckersBoard::Rules CheckersBoard::mRules;
CheckersBoard::Cell CheckersBoard::mCells[kNumCells];
ulong CheckersBoard::mBlackBackRow;
ulong CheckersBoard::mWhiteBackRow;

BoardClass CheckersBoard::mClass("CheckersBoard",
                                 &CreateCheckersBoard,
                                 "Checkers",
                                 &CheckersView::mClass,
                                 &CheckersDlg::mClass,
                                 &CheckersBoard::SetOptions,
                                 &CheckersBoard::GetOptions);
// The C++ definition here isn't required in C++11, which I'm using.
// Put it there anyways to force the "static block" to run.
CheckersBoard::CheckersBoardInitializer CheckersBoard::mInitializer;


void CheckersBoard::StaticInit() {
   Cell *cell;
   char row = 'A';
   unsigned col = 1;
   int nextCell = 0;

   // Initialize mCells, mBlackBackRow and mWhiteBackRow
   for (row = 'A'; row <= 'H'; row ++) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2, nextCell++) {
      
         // Initialize cell->mask
         cell = mCells + nextCell;
         cell->mask = 1 << nextCell;

         // cout << "Setup cell: " << row << col << "\n";
         cell->name = FString("%c%u", row, col);
         cell->loc = CheckersMove::Location(row, col);

         // Initialize mBlackBackRow
         if (row == 'A') {
            mBlackBackRow |= cell->mask;
         }
         // Initialize mWhiteBackRow
         else if (row == 'H') {
            mWhiteBackRow |= cell->mask;
         } 
      }
   }

   // Go back and initialize the directional pointers.  (Can't initialize
   // in the first pass, since not all cells have been created then).
   nextCell = 0;
   for (row = 'A'; row <= 'H'; row ++) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2, nextCell++) {

         cell = mCells + nextCell;

         // Set up the directional pointers for mCells.  GetCell() 
         // automatically returns NULL for any out of bounds values.
         cell->neighborCells[kSW] = GetCell(row-1, col-1);
         cell->neighborCells[kSE] = GetCell(row-1, col+1);
         cell->neighborCells[kNW] = GetCell(row+1, col-1);
         cell->neighborCells[kNE] = GetCell(row+1,col+1);
      }
   }
}

CheckersBoard::CheckersBoard() : mWhoseMove(kBlack), 
 mBlackCount(kStartingPieces), mBlackBackCount(kStartingBackPieces),
 mWhiteCount(kStartingPieces), mWhiteBackCount(kStartingBackPieces) {
   // Just to make sure that I'm covering all my bases with ALL member datum
   assert(mMoveHist.size() == 0);

   char row = 'A';
   unsigned col = 1;

   // Nil out the bitmasks at first
   mBlackSet = mWhiteSet = mKingSet = 0x0;

   // Fill up mBlackSet and mWhiteSet
   for (row = 'A'; row <= 'H'; row++) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2) {

         // Fill up initial mBlackSet
         if (row == 'A' || row == 'B' || row == 'C') {
            mBlackSet |= GetCell(row, col)->mask;
         }
         // Fill up initial mWhiteSet
         else if (row == 'F' || row == 'G' || row == 'H') {
            mWhiteSet |= GetCell(row, col)->mask;
         }
      }
   }
}

CheckersBoard::~CheckersBoard() {
   // Clear out mMoveHistory
   list<Move *>::iterator itr;
   for (itr = mMoveHist.begin(); itr != mMoveHist.end(); itr++)
      delete *itr;
   mMoveHist.clear();
}

long CheckersBoard::GetValue() const {
   return 0;
}

void CheckersBoard::ApplyMove(Move *move) {
   CheckersMove *castedMove = dynamic_cast<CheckersMove *>(move);
   CheckersMove::LocVector *locs = &castedMove->mLocs;
   assert(castedMove != NULL && castedMove->mLocs.size() >= 2);
   Set allPieces(mBlackSet | mWhiteSet);
   Cell *originCell = GetCell((*locs)[0].first, (*locs)[0].second);
   int jumpedPieces = 0; // The number of pieces that move "jumps"
   

   // Sanity checks on each destination piece.
   for (unsigned int index1 = 1; index1 < (*locs).size(); ++index1) {
      Cell *cell = GetCell((*locs)[index1].first, (*locs)[index1].second);
      
      // Assert that this cell (where this move wants to go to) isn't already
      // taken.
      assert((allPieces & cell->mask) == 0);
      
      // Assert that non-king pieces aren't moving backwards.
      if ((originCell->mask & mKingSet) == 0) {
         // Black non-kings should be moving upwards
         if (mWhoseMove == kBlack) {
            assert((*locs)[index1].first > (*locs)[index1-1].first);
         }
         // White non-kings should be moving downwards
         else if (mWhoseMove == kWhite) {
            assert((*locs)[index1].first < (*locs)[index1-1].first);
         }
      }
   }

   // Remove the piece from its original location
   HalfTake(GetCell((*locs)[0].first, (*locs)[0].second), mWhoseMove);

   // First, check if this move is a "non-jump move".  If it is, then don't
   // do anything (let the rest of applyMove() run its course).
   if (abs((*locs)[0].first - (*locs)[1].first) == 1 &&
    abs((int)((*locs)[0].second - (*locs)[1].second)) == 1) {
      cout << "Got non-jump move" << endl;
      // HalfPut(GetCell((*locs)[1].first, (*locs)[1].second), mWhoseMove);
   }
   // Otherwise, this move IS a jump move.
   else {
      for (unsigned int index2 = 1; index2 < (*locs).size(); ++index2) {
         Cell *jumpedCell = NULL;
         Cell *destCell = GetCell((*locs)[index2].first, (*locs)[index2].second);

         // Assert that this location actually jumps over a piece
         assert(abs((*locs)[index2-1].first - (*locs)[index2].first) == 2);
         // cout << "abs() = " << abs(((int)(*locs)[index2-1].second - (int)(*locs)[index2].second));
         assert(abs(((int)(*locs)[index2-1].second - (int)(*locs)[index2].second)) == 2);

         // If you at any point a non-king jumps into the back row, then assert 
         // that it STOPPED MOVING.  Also, add the piece to the king set.
         if ((destCell->mask & mKingSet) == 0) {
            if (mWhoseMove == kBlack && (destCell->mask & mWhiteBackRow)) {
               assert(index2 == ((*locs).size() - 1));
               mKingSet |= destCell->mask;
            }
            else if (mWhoseMove == kWhite && (destCell->mask & mBlackBackRow)) {
               assert(index2 == ((*locs).size() - 1));
               mKingSet |= destCell->mask;
            }
         }

         // Figure out which cell you jumped
         // North-west jump
         if ((*locs)[index2].first > (*locs)[index2-1].first
          && (*locs)[index2].second < (*locs)[index2-1].second) {
            jumpedCell = GetCell((*locs)[index2].first - 1, (*locs)[index2].second + 1);
         }
         // North-east jump
         else if ((*locs)[index2].first > (*locs)[index2-1].first
          && (*locs)[index2].second > (*locs)[index2-1].second) {
            jumpedCell = GetCell((*locs)[index2].first - 1, (*locs)[index2].second - 1);
         }
         // South-west jump
         else if ((*locs)[index2].first < (*locs)[index2-1].first
          && (*locs)[index2].second < (*locs)[index2-1].second) {
            jumpedCell = GetCell((*locs)[index2].first + 1, (*locs)[index2].second + 1);
         } 
         // South-east jump
         else if ((*locs)[index2].first < (*locs)[index2-1].first
          && (*locs)[index2].second > (*locs)[index2-1].second) {
            jumpedCell = GetCell((*locs)[index2].first + 1, (*locs)[index2].second - 1);
         } else assert(false); 

         // Assert that the piece that you jumped over is occupied by the other
         // player and not yourself.  Remove that piece.
         if (mWhoseMove == kBlack) {
            assert((jumpedCell->mask & mWhiteSet) != 0);
            HalfTake(jumpedCell, kWhite);

         } else if (mWhoseMove == kWhite) {
            assert((jumpedCell->mask & mBlackSet) != 0);
            HalfTake(jumpedCell, kBlack);

         } else assert(false);
      }
   }

   // Add the piece to its final destination
   Cell *destinationCell =
    GetCell((*locs)[locs->size()-1].first, (*locs)[locs->size()-1].second);
   HalfPut(destinationCell, mWhoseMove);

   // Change whose move it is
   if (mWhoseMove == kBlack)
      mWhoseMove = kWhite;
   else if (mWhoseMove == kWhite)
      mWhoseMove = kBlack;
   else assert(false);

   // Assert that the two bitmasks don't have any pieces in common.
   assert((mBlackSet & mWhiteSet) == 0);
}

void CheckersBoard::UndoLastMove() {
   // This should basically be ApplyMove() backwards.

}

void CheckersBoard::GetAllMoves(list<Move *> *uncastMoves) const {
   char row;
   unsigned int col;
   list<CheckersMove *>::iterator itr;
   list<CheckersMove *> *castedMoves = reinterpret_cast<list<CheckersMove *>*>(uncastMoves);

   assert(uncastMoves->size() == 0 && castedMoves->size() == 0);

   if (mBlackCount == 0 || mWhiteCount == 0)
      return;

   for (row = 'A'; row <= 'H'; row++) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2) {
         Cell *cell = GetCell(row, col);

         

         // For each occupied Cell on the board, inspect the moves it can take.
         // Black king case.
         if (mWhoseMove == kBlack && CellOccupied(row, col, kBlack) 
          && CellContainsKing(row, col)) {
             AddAllMovesInAllDirections(castedMoves, cell, true);
         } 
         // White King case
         else if (mWhoseMove == kWhite && CellOccupied(row, col, kWhite) 
          && CellContainsKing(row, col)) {
             AddAllMovesInAllDirections(castedMoves, cell, true);
         } 
         // Black piece case
         else if (mWhoseMove == kBlack && CellOccupied(row, col, kBlack)) {
            AddAllMovesInAllDirections(castedMoves, cell, false);
         } 
         // White piece case
         else if (mWhoseMove == kWhite && CellOccupied(row, col, kWhite)) {
            AddAllMovesInAllDirections(castedMoves, cell, false);
         }
      }
   }
}

// Adds possible moves for a particular piece, in all four directions.
void CheckersBoard::AddAllMovesInAllDirections(list<CheckersMove *> *moves, Cell *cell, 
 bool isKing) const {
   assert(cell != NULL);

   // In the order above, first see if the Locations are immediately
   // empty.  If they are, don't recurse.  Instead, just add that
   // location as a "non-jump" move.  This is the only possible way
   // that a move can be a "non-jump" move.
   // 
   // Otherwise, do a recursive DFS on the possible "jump moves" that
   // you can do.  Base case is when the Location that you would jump to
   // is out of bounds, or if the Location that you would jump to is
   // already occupied.
   
   // Kings should try all four directions
   if (isKing) {
      AddMovesInDirection(moves, cell, kSW, isKing);
      AddMovesInDirection(moves, cell, kSE, isKing);
      AddMovesInDirection(moves, cell, kNW, isKing);
      AddMovesInDirection(moves, cell, kNE, isKing);
   }
   // Black pieces should try topLeft and topRight
   else if (mWhoseMove == kBlack) {
      AddMovesInDirection(moves, cell, kNW, isKing);
      AddMovesInDirection(moves, cell, kNE, isKing);
   }
   // White pieces should try bottomLeft and bottomRight
   else if (mWhoseMove == kWhite) {
      AddMovesInDirection(moves, cell, kSW, isKing);
      AddMovesInDirection(moves, cell, kSE, isKing);
   } else assert(false);
}


// Attempts to short-circuit the DFS by finding out of bounds and non-jump
// directions.
void CheckersBoard::AddMovesInDirection(list<CheckersMove *> *moves, 
 Cell *from, int direction, bool isKing) const {    
   assert(from != NULL);  

   // If the piece in this direction is NULL (that is, out of bounds), then break.
   Cell *to = from->neighborCells[direction];
   if (to == NULL) {
      return;
   }
   
   Set allPieces = (mWhiteSet|mBlackSet);

   // If the Cell in this direction is empty, then we found a non=jump move.
   // Add a non-jump move in this direction to the list of moves, and then return.
   if ((to->mask & allPieces) == 0) {
      // Add a non-jump move in this direction, and return
      CheckersMove::LocVector locs;
      locs.push_back(from->loc);
      locs.push_back(to->loc);
      CheckersMove *newMove = new CheckersMove(locs);
      moves->push_back(newMove);
   }
   // Otherwise, this direction will yield either a jump move or won't yield
   // a move at all.  We have to run the recursive DFS in this direction.
   else {
      // Call the recursive DFS to find all possible jump moves in this
      // direction.
      CheckersMove::LocVector locs;
      locs.push_back(from->loc);
      AddJumpMoves(moves, locs, from, direction);
   }
}

// This is the recursive DFS.
void CheckersBoard::AddJumpMoves(list<CheckersMove *> *moves, 
 CheckersMove::LocVector locs, Cell *from, int direction, bool isKing) const {

//    // "The target spot" is the spot one space in the direction that you
//    // want to check.
//    // 
//    // Check if the target spot is occupied by the other player's piece.
//    // If it is, then check if the spot behind the target spot is empty.
//    // If it is, then add that move to the list of moves, 
//    // and then recursively recall this function 
//    // from that spot, in the direction that you jumped.
// 
//    // Find the cell diagonal to this one.
//    Cell *diagonalCell = from->neighborCells[direction];
// 
//    // Assert that the diagonal cell is not out of bounds (the method that
//    // calls this method should have already handled that case).
//    assert(diagonalCell != NULL);
// 
//    Cell *diagonalDiagonalCell = diagonalCell->neighborCells[direction];
// 
//    // If the diagonalDiagonalCell is out of bounds, then return without adding
//    // any new moves.
//    if (diagonalDiagonalCell == NULL) {
//       return;
//    }
//    
//    // If diagonalCell belongs to the other player and diagonalDiagonalCell
//    // is empty, then "make the jump" by adding this move to the list of moves,
//    // and additionally recursively call this function from diagonalDiagonalCell
//    // for each direction.
//    if ((mWhoseMove == kBlack && ((diagonalCell->mask & mWhiteSet) != 0)
//         && ((diagonalDiagonalCell->mask & (mWhiteSet|mBlackSet)) == 0))
//     || (mWhoseMove == kWhite && ((diagonalCell->mask & mBlackSet) != 0)
//     && ((diagonalDiagonalCell->mask & (mWhiteSet|mBlackSet)) == 0))) {
//       // Append the destination location of this jump to the LocVector that's
//       // being recursively passed around, and then recursively call this
//       // from the new location for all four directions (for the permissions 
//       // the piece has).
//       locs.push_back(diagonalDiagonalCell->loc);
//       
//       // Kings should try all four directions
//       if (isKing) {
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kSW, isKing);
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kSE, isKing);
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kNW, isKing);
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kNE, isKing);
//       }
//       // Black pieces should try topLeft and topRight
//       else if (mWhoseMove == kBlack) {
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kNW, isKing);
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kNE, isKing);
//       }
//       // White pieces should try bottomLeft and bottomRight
//       else if (mWhoseMove == kWhite) {
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kSW, isKing);
//          AddJumpMoves(moves, locs, diagonalDiagonalCell, kSE, isKing);
//       } else assert(false);
// 
//    }
// 

}



Board::Move *CheckersBoard::CreateMove() const {
   return new CheckersMove(CheckersMove::LocVector());
}


Board *CheckersBoard::Clone() const {
   // [Staley] Think carefully about this one.  You should be able to do it in just
   // [Staley] 5-10 lines.  Don't do needless work

   return NULL;
}


Board::Key *CheckersBoard::GetKey() const {
   // Leave this empty when you first turn in Milestone 1 -- Bender doesn't
   // check this at all.  That is, be sure to start getting rolling in handins 
   // BEFORE implementing this.
   return NULL;
}


void *CheckersBoard::GetOptions() {
   // The caller of this method owns the object that is returned here.

   return NULL;
}

void CheckersBoard::SetOptions(const void *opts) {
   // The caller of this method owns the object that is returned here.


}

istream &CheckersBoard::Read(istream &is) {

   return is;
}

ostream &CheckersBoard::Write(ostream &os) const { 
   
   return os;
}
