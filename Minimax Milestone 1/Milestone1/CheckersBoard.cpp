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
   list<CheckersMove *> *castedMoves = reinterpret_cast<list<CheckersMove *>*>(uncastMoves);
   char row;
   unsigned int col;
   bool canJumpAtLeastOnce = false;

   assert(uncastMoves->size() == 0 && castedMoves->size() == 0);

   // If the game is over, don't construct a list
   if (mBlackCount == 0 || mWhiteCount == 0)
      return;

   for (row = 'A'; row <= 'H'; row++) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2) {
         Cell *cell = GetCell(row, col);
         assert(cell != NULL);

         for (int dir = 0; dir < kSqr; dir++) {
            // If a piece can move in a particular direction, then do it.
            // It can't possibly jump a piece in that direction.
            if (!canJumpAtLeastOnce && CanMove(cell, dir)) {
               // Add a non-jump move in this direction
               CheckersMove::LocVector locs;
               locs.push_back(cell->loc);
               locs.push_back(cell->neighborCells[dir]->loc);
               CheckersMove *newMove = new CheckersMove(locs);
               castedMoves->push_back(newMove);
            }
            else if (CanJump(cell, dir)) {
               // At this point, you're guaranteed to have at least one
               // possible jump that you can make.  Run a DFS on the possible 
               // multiple jumps that you can take.

               // Before you do that, remove any other "non-jump" moves that
               // you added before you add "jump" moves.
               if (!canJumpAtLeastOnce) {
                  for (list<CheckersMove *>::iterator listIter = castedMoves->begin();
                   listIter != castedMoves->end(); listIter++) {
                      delete *listIter;
                  }
                  castedMoves->clear();
                  canJumpAtLeastOnce = true;
               }

               // Add the starting location to any possible jumps, since they
               // all would start from this location.
               CheckersMove::LocVector locs;
               locs.push_back(cell->loc);
               MultipleJumpDFS(castedMoves, locs, cell);
            }
         }
      }
   }
}

void CheckersBoard::MultipleJumpDFS(list<CheckersMove *> *moves, 
 CheckersMove::LocVector locs, Cell *cell) const {
   Cell *cellToJumpInto = NULL;
   bool foundDeeperJumpBranch = false;

   for (int dir = 0; dir < kSqr; dir++) {
      if (CanJump(cell, dir)) {
         // You've found a multiple jump route here.  Set up for your recursive
         // call to keep searching deeper.
         cellToJumpInto = cell->neighborCells[dir]->neighborCells[dir];
         foundDeeperJumpBranch = true;

         // Add the cell that you would jump over into to the LocVector
         // that you're constructing for this [multiple] jump.
         locs.push_back(cellToJumpInto->loc);

         // Recursive call, using the appended LocVector and the new location
         // that you would jump into.
         MultipleJumpDFS(moves, locs, cellToJumpInto);
      }
   }

   // If you get to a branch that is cut off from having any deeper branches
   // in any direction, then you've found a jump (or multiple jump) that doens't
   // stop halfway through completion.  Add it to the list of moves.
   if (!foundDeeperJumpBranch) {
      CheckersMove *newMove = new CheckersMove(locs);
      moves->push_back(newMove);
   }
}

Board::Move *CheckersBoard::CreateMove() const {
   return new CheckersMove(CheckersMove::LocVector());
}


Board *CheckersBoard::Clone() const {
   // [Staley] Think carefully about this one.  You should be able to do it in just
   // [Staley] 5-10 lines.  Don't do needless work
   CheckersBoard *boardCopy = new CheckersBoard(*this);

   list<Move *>::iterator moveHistIter;

   boardCopy->mMoveHist.clear();
   for (moveHistIter = boardCopy->mMoveHist.begin(); 
    moveHistIter != boardCopy->mMoveHist.end(); moveHistIter++) {
      *moveHistIter = (*moveHistIter)->Clone();
   }

   return boardCopy;
}


Board::Key *CheckersBoard::GetKey() const {
   // Leave this empty when you first turn in Milestone 1 -- Bender doesn't
   // check this at all.  That is, be sure to start getting rolling in handins 
   // BEFORE implementing this.
   return NULL;
}


void *CheckersBoard::GetOptions() {
   // The caller of this method owns the object that is returned here.
   return new Rules(mRules);
}

void CheckersBoard::SetOptions(const void *opts) {
   // The caller of this method owns the object that is returned here.
   mRules = *reinterpret_cast<const Rules *>(opts);
}

istream &CheckersBoard::Read(istream &is) {

   return is;
}

ostream &CheckersBoard::Write(ostream &os) const { 
   
   return os;
}
