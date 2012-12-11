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


// TODO: Ian had this great idea where you testRun for a large value
// that runs in the background, diffing against Clint's version as you go.
// Since you don't have to adhere to a 0.1 second limit, this can catch
// a lot of SegFault type errors and the sort.

/************************************************************************/
/* Declare/initialize static member datum here                          */
/************************************************************************/
CheckersBoard::Rules CheckersBoard::mRules;
CheckersBoard::Cell CheckersBoard::mCells[kNumCells];
ulong CheckersBoard::mBlackBackSet, CheckersBoard::mWhiteBackSet;

BoardClass CheckersBoard::mClass("CheckersBoard", &CreateCheckersBoard,
 "Checkers", &CheckersView::mClass, &CheckersDlg::mClass,
 &CheckersBoard::SetOptions, &CheckersBoard::GetOptions);

// The C++ definition here isn't required in C++11, which I'm using.
// Put it there anyways to force the "static block" to run.
CheckersBoard::CheckersBoardInitializer CheckersBoard::mInitializer;


void CheckersBoard::StaticInit() {
   Cell *cell;
   char row = 'A';
   unsigned int col = 1, nextCell = 0;

   // Initialize mCells, mBlackBackRow and mWhiteBackRow
   for (row = 'H'; row >= 'A'; row--) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2, nextCell++) {
      
         // Initialize cell->mask
         cell = mCells + nextCell;
         cell->mask = 1 << nextCell;

         cell->loc = CheckersMove::Location(row, col);

         // Initialize mBlackBackRow
         if (row == 'A') {
            mBlackBackSet |= cell->mask;
         }
         // Initialize mWhiteBackRow
         else if (row == 'H') {
            mWhiteBackSet |= cell->mask;
         } 
      }
   }

   // Go back and initialize the directional pointers.  (Can't initialize
   // in the first pass, since not all cells have been created then).
   nextCell = 0;
   for (row = 'A'; row <= 'H'; row ++) {
      for (col = ((row-'A')%2) + 1; col <= kWidth; col += 2, nextCell++) {

         cell = GetCell(row, col);

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
 mBlackPieceCount(kStartingPieces), mBlackKingCount(0), 
 mBlackBackCount(kStartingBackPieces), mWhitePieceCount(kStartingPieces), 
 mWhiteKingCount(0), mWhiteBackCount(kStartingBackPieces) {
   // Just to make sure that I'm covering all my bases with ALL member datum
   assert(mMoveHist.size() == 0);

   // Call the "Delete()" method, which really is just a housecleaning method
   // that fills up the CheckersBoard with the right initial pieces.
   Delete();
}

void CheckersBoard::Delete() {
   // Reset the member datum to their default values.
   mBlackSet = mWhiteSet = mKingSet = 0x0;
   mWhoseMove = kBlack;
   mBlackPieceCount = mWhitePieceCount = kStartingPieces;
   mBlackKingCount = mWhiteKingCount = 0;
   mBlackBackCount = mWhiteBackCount = 0;

   // Fill up mBlackSet and mWhiteSet
   for (char row = 'A'; row <= 'H'; row++) {
      for (unsigned int col = ((row-'A')%2) + 1; col <= kWidth; col += 2) {

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

   // Clear out mMoveHistory and mCapturedPieces.
   list<Move *>::iterator moveIter;
   for (moveIter = mMoveHist.begin(); moveIter != mMoveHist.end(); moveIter++)
      delete *moveIter;
   mMoveHist.clear();
   mCapturedPieces.clear();
}

// TODO: Refactor this to not use loops here.
// "Compute the values for Pylos and Checkers incrementally, updating when 
// moves are made.  You should have no loops in GetValue.  This will be much 
// faster than recomputing the value, and will affect the speed of your minimax 
// algorithm later."
long CheckersBoard::GetValue() const {
   int movesLeft = 0;

   // Prep to handle the case where the game is over due to noone being able
   // to move.
   // TODO: If my code runs too slow, here's where I can optimize by not
   // looking up all of the moves every time I showVal.
   list<Move *> allMoves;
   GetAllMoves(&allMoves);
   movesLeft = allMoves.size();

   // Clean up after your GetAllMoves() call.
   for (list<Move *>::iterator listIter = allMoves.begin();
    listIter != allMoves.end(); listIter++) {
       delete *listIter;
   }
   
   // First, check if anyone can even move.  The player who's stuck on his
   // turn is the one who lost, so the other player must have won.
   if (movesLeft == 0)
      return kWinVal * -mWhoseMove;
   else if ((mWhitePieceCount + mWhiteKingCount) == 0)
      return kWinVal;
   else if ((mBlackPieceCount + mBlackKingCount) == 0)
      return -kWinVal;
   else
      return pieceWgt * (mBlackPieceCount - mWhitePieceCount) + 
       mRules.kingWgt * (mBlackKingCount - mWhiteKingCount) +
       mRules.backRowWgt * (mBlackBackCount - mWhiteBackCount) +
       mRules.moveWgt * mWhoseMove;
}

void CheckersBoard::ApplyMove(Move *move) {
   CheckersMove *castedMove = dynamic_cast<CheckersMove *>(move);
   CheckersMove::LocVector *locs = &castedMove->mLocs;
   assert(castedMove != NULL && castedMove->mLocs.size() >= 2);
   Set allPieces(mBlackSet | mWhiteSet);
   Cell *originCell = GetCell((*locs)[0]);
   int jumpedPieces = 0; // The number of pieces that move "jumps"
   Piece *pieceToMove = NULL;

   // TODO: Don't use this code unless you have a bug.  This is just a series 
   // of assert statements.
//    Assertions on each destination piece.
//       for (unsigned int index = 1; index < (*locs).size(); ++index) {
//          Cell *cell = GetCell((*locs)[index].first, (*locs)[index].second);
//          
//          // Assert that this cell (where this move wants to go to) isn't already
//          // taken.
//          assert((allPieces & cell->mask) == 0);
//          
//          // Assert that non-king pieces aren't moving backwards.
//          if ((originCell->mask & mKingSet) == 0) {
//             // Black non-kings should be moving upwards
//             if (mWhoseMove == kBlack) {
//                assert((*locs)[index].first > (*locs)[index-1].first);
//             }
//             // White non-kings should be moving downwards
//             else if (mWhoseMove == kWhite) {
//                assert((*locs)[index].first < (*locs)[index-1].first);
//             }
//          }
//       }

   // Remove the piece from its original location.
   pieceToMove = HalfTake(originCell, mWhoseMove);

   // If this move is a "jump move", then you need to do perform some extra
   // logic to remove cells that you've jumped over.
   if (castedMove->mIsJumpMove) {
      for (unsigned int i = 1; i < (*locs).size(); ++i) {
         Cell *jumpedOverCell = NULL;
         Cell *toCell = GetCell((*locs)[i]);

         // Assert that this location actually jumps over a piece
         assert(CheckersMove::IsJump((*locs)[i-1], (*locs)[i]));

         // If you at any point a non-king jumps into the back row, then assert 
         // that it STOPPED MOVING.
         if (!pieceToMove->isKing) {
            if (mWhoseMove == kBlack && (toCell->mask & mWhiteBackSet))
               assert(i == ((*locs).size() - 1));
            else if (mWhoseMove == kWhite && (toCell->mask & mBlackBackSet))
               assert(i == ((*locs).size() - 1));
         }

         // Figure out which cell you jumped over.
         // TODO: Refactor this.
         // North-west jump
         if ((*locs)[i].first > (*locs)[i-1].first
          && (*locs)[i].second < (*locs)[i-1].second)
            jumpedOverCell = GetCell((*locs)[i].first - 1, (*locs)[i].second + 1);
         // North-east jump
         else if ((*locs)[i].first > (*locs)[i-1].first
          && (*locs)[i].second > (*locs)[i-1].second)
            jumpedOverCell = GetCell((*locs)[i].first - 1, (*locs)[i].second - 1);
         // South-west jump
         else if ((*locs)[i].first < (*locs)[i-1].first
          && (*locs)[i].second < (*locs)[i-1].second)
            jumpedOverCell = GetCell((*locs)[i].first + 1, (*locs)[i].second + 1);
         // South-east jump
         else if ((*locs)[i].first < (*locs)[i-1].first
          && (*locs)[i].second > (*locs)[i-1].second)
            jumpedOverCell = GetCell((*locs)[i].first + 1, (*locs)[i].second - 1);

         // TODO: Only use this code if you have a bug and you need to turn
         // on assert statements.
//          Assert that the piece that you jumped over is occupied by the other
//                   // player and not yourself.
//                   if (mWhoseMove == kBlack) {
//                      assert((jumpedOverCell->mask & mWhiteSet) != 0);
//                   } else if (mWhoseMove == kWhite) {
//                      assert((jumpedOverCell->mask & mBlackSet) != 0);
//                   } else assert(false);

         // Remove the piece that you jumped over, and save it.
         Piece *removedPiece = HalfTake(jumpedOverCell, -mWhoseMove);
         mCapturedPieces.push_back(removedPiece);
      }
   }

   // Add the piece to its final destination
   Cell *destinationCell =
    GetCell((*locs)[locs->size()-1].first, (*locs)[locs->size()-1].second);
   
   // If the piece that you're about to Put() is going to be put into the
   // opponent's back row, then this is a "king me" move.
   // Set the flags for it, and add this cell to the mKingSet bitmask.
   if ((mWhoseMove == kBlack && ((destinationCell->mask & mWhiteBackSet) != 0))
    || (mWhoseMove == kWhite && ((destinationCell->mask & mBlackBackSet) != 0))) {
      mKingSet |= destinationCell->mask;
      castedMove->mIsKingMeMove = true;
      pieceToMove->isKing = true;
   }

   // Add the piece to its final destination.
   HalfPut(pieceToMove, destinationCell);

   // Assert that the two bitmasks don't have any pieces in common.
   assert((mBlackSet & mWhiteSet) == 0);

   // Change whose move it is and keep a track of this move in mMoveHist
   mMoveHist.push_back(move);
   mWhoseMove = -mWhoseMove;

   // Refresh Board valuation before you finish
   RefreshBoardValuation();

   // Assert that the two bitmasks don't have any pieces in common.
   assert((mBlackSet & mWhiteSet) == 0);
}

void CheckersBoard::UndoLastMove() {
   // Load up the last move in mMoveHist.
   CheckersMove *moveToUndo = dynamic_cast<CheckersMove *>(mMoveHist.back());
   Cell *originCell = GetCell(moveToUndo->mLocs[0]),
    *destCell = GetCell(moveToUndo->mLocs[moveToUndo->mLocs.size()-1]);
   Piece *pieceToMove = NULL;

   // Switch whose move it is (early, so that you're taking away the turn
   // that happened BEFORE this current turn)
   mWhoseMove = -mWhoseMove;

   // First, regardless of if this move is a jump move or not, Take the 
   // ending location away and Put the starting location back in.
   pieceToMove = HalfTake(destCell, mWhoseMove);

   // Before you actually call Put() and Take(), check if you're undoing a 
   // "king me" move.  If that's the case,  then un-King the piece that you
   // Put() back in.
   if (moveToUndo->mIsKingMeMove) {
      mKingSet &= ~(originCell->mask);
      pieceToMove->isKing = false;
   }

   HalfPut(pieceToMove, originCell);
   
   // If you're undoing a jump move, then Put each of the moves that you 
   // captured back in.
   if (moveToUndo->mIsJumpMove) {
      int numberOfPiecesToPutBack = moveToUndo->mLocs.size() - 1;

      for (int i = 0; i < numberOfPiecesToPutBack; i++) {
         Piece *pieceToPutBack = mCapturedPieces.back();
         mCapturedPieces.pop_back();
         Cell *cellToPutItIn = GetCell(pieceToPutBack->loc);
         HalfPut(pieceToPutBack, cellToPutItIn);

         // Delete the Piece object once you put it back into the board.
         delete pieceToPutBack;
      }
   }

   // Destroy history of the move
   delete moveToUndo;
   mMoveHist.pop_back();

   // Refresh Board valuation before you finish
   RefreshBoardValuation();
}

void CheckersBoard::GetAllMoves(list<Move *> *uncastMoves) const {
   list<CheckersMove *> *castedMoves = reinterpret_cast<list<CheckersMove *>*>(uncastMoves);
   char row;
   unsigned int col;
   bool canJumpAtLeastOnce = false;

   assert(uncastMoves->size() == 0 && castedMoves->size() == 0);

   // If the game is over, don't construct a list
   if ((mBlackPieceCount+mBlackKingCount) == 0 || 
    (mWhitePieceCount+mWhiteKingCount) == 0)
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
               CheckersMove *newMove = new CheckersMove(locs, false);
               castedMoves->push_back(newMove);
            }
            else if (CanJump(cell, dir)) {
               // If you can jump any piece, then stop what you're doing,
               // clear out the non-move pieces that you've collected,
               // and step through the rest of the board running the DFS
               // on each Cell.
               
               // Clear the board.
               for (list<CheckersMove *>::iterator listIter = castedMoves->begin();
                listIter != castedMoves->end(); listIter++) {
                   delete *listIter;
               }
               castedMoves->clear();

               // Step through the rest of the board
               for (char newRow = row; newRow <= 'H'; newRow++) {
                  for (unsigned int newCol = ((newRow-'A')%2) + 1; 
                   newCol <= kWidth; newCol += 2) {
                     Cell *newCell = GetCell(newRow, newCol);

                     // Add the starting location to any possible jumps, since 
                     // they all would start from this location.
                     CheckersMove::LocVector locs;
                     locs.push_back(newCell->loc);
                     MultipleJumpDFS(castedMoves, locs, newCell);
                  }
               }

               // End the method early.
               return;
            }
         }
      }
   }
}

// This method should look for multiple jumps, starting from the cell that you
// initially jumped into.
void CheckersBoard::MultipleJumpDFS(list<CheckersMove *> *moves, 
 CheckersMove::LocVector locs, Cell *cell) const {
   Cell *destCell = NULL;
   bool foundDeeperJumpBranch = false;

   // If moving this to this cell is a "king me" move, then immediately 
   // cut off this branch from having any deeper branches (set
   // foundDeeperJumpBranch to false and break before getting to the
   // recursive call).
   if (locs.size() > 1 && ((cell->mask & mKingSet) == 0)) {
      if ((mWhoseMove == kBlack && (cell->mask & mWhiteBackSet))
         || (mWhoseMove == kWhite && (cell->mask & mBlackBackSet))) {
         CheckersMove *newMove = new CheckersMove(locs, true);
         moves->push_back(newMove);
         return;
      }
   }

   // Otherwise, check all possible routes as usual, with the exception
   // of routes that have already been checked by the non-recursive function
   // that calls this.
   for (int dir = 0; dir < kSqr; dir++) {
      if (CanJump(cell, dir)) {
         // You've found a multiple jump route here.  Set up for your recursive
         // call to keep searching deeper.
         destCell = cell->neighborCells[dir]->neighborCells[dir];
         foundDeeperJumpBranch = true;

         // Temporarily move this piece from the old location to the new 
         // location, and remove the piece that you jumped over.  Undo all of 
         // this after the recursive call.
         Piece *oldLocation = HalfTake(cell, mWhoseMove);
         Piece *capturedPiece = HalfTake(cell->neighborCells[dir], -mWhoseMove);
         HalfPut(oldLocation, destCell);

         // Add the cell that you would jump over into to the LocVector
         // that you're constructing for this [multiple] jump.
         locs.push_back(destCell->loc);

         // Recursive call, using the appended LocVector and the new location
         // that you would jump into.
         MultipleJumpDFS(moves, locs, destCell);

         // Remove this most recent Location from the LocVector, since you're
         // going to be reusing this LocVector as you recurse.
         locs.pop_back();

         // Put the temporary jump back to where it was, and clean up afterwards
         Piece *newLocation = HalfTake(destCell, mWhoseMove);
         delete newLocation;
         HalfPut(capturedPiece, cell->neighborCells[dir]);
         HalfPut(oldLocation, cell);
         delete capturedPiece;
         delete oldLocation;
      }
   }

   // If you get to a branch that is cut off from having any deeper branches
   // in any direction, then you've found a jump (or multiple jump) that doens't
   // stop halfway through completion.  Add it to the list of moves.
   if (!foundDeeperJumpBranch && locs.size() > 1) {
      CheckersMove *newMove = new CheckersMove(locs, true);
      moves->push_back(newMove);
   }
}

Board::Move *CheckersBoard::CreateMove() const {
   return new CheckersMove(CheckersMove::LocVector(), false);
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
   BasicKey<4> *rtn = new BasicKey<4>();

   rtn->vals[0] = mWhoseMove == kWhite;
   rtn->vals[1] = mBlackSet;
   rtn->vals[2] = mWhiteSet;
   rtn->vals[3] = mKingSet;

   return rtn;
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

   int moveCount = -1;

   // Clear out the Default board's existing data
   Delete();

   // Read in the Rules that the board should use.
   is.read((char *)&CheckersBoard::mRules, sizeof(CheckersBoard::mRules));
   CheckersBoard::mRules.EndSwap();

   is.read((char *)&moveCount, sizeof(moveCount));
   assert(moveCount != -1);  // sanity check to ensure that the read() happened
   for (int i = 0; i < moveCount; i++) {
      CheckersMove *newMove = new CheckersMove(CheckersMove::LocVector(), false);
      is >> *newMove;

      assert(newMove->mLocs.size() != 0);

      ApplyMove(newMove);
   }

   return is;
}

ostream &CheckersBoard::Write(ostream &os) const { 
   Rules rules = mRules;
   list<Move *>::const_iterator iter;
   int moveCount = EndianXfer((int)mMoveHist.size());

   rules.EndSwap();
   os.write((char *)&rules, sizeof(rules));

   os.write((char *)&moveCount, sizeof(moveCount));
   for (iter = mMoveHist.begin(); iter != mMoveHist.end(); iter++)
      os << **iter;

   return os;
}

void CheckersBoard::Rules::EndSwap() {
   this->kingWgt = EndianXfer(this->kingWgt);
   this->backRowWgt = EndianXfer(this->backRowWgt);
   this->moveWgt = EndianXfer(this->moveWgt);
}

void CheckersBoard::RefreshBoardValuation() {
   // Clear out existing values
   mBlackPieceCount = mWhitePieceCount = mBlackKingCount = mWhiteKingCount
    = mBlackBackCount = mWhiteBackCount = 0;

   // Count up the values for each type
   for (char row = 'A'; row <= 'H'; row ++) {
      for (unsigned int col = ((row-'A')%2) + 1; col <= kWidth; col += 2) {
         assert((mWhiteSet & mBlackSet) == 0);

         // If this cell is currently occupied by a Black piece...
         if (GetCell(row, col)->mask & mBlackSet) {
            if (GetCell(row, col)->mask & mKingSet)
               ++mBlackKingCount;
            else
               ++mBlackPieceCount;

            if (row == 'A')
               ++mBlackBackCount;
         } 
         // If this cell is currently occupied by a White piece...
         else if (GetCell(row, col)->mask & mWhiteSet) {
            if (GetCell(row, col)->mask & mKingSet) 
               ++mWhiteKingCount;
            else
               ++mWhitePieceCount;

            if (row == 'H')
               ++mWhiteBackCount;
         }
      }
   }
}

// TODO: Refactor this to use a Piece instead of a Cell.
inline bool CheckersBoard::CanMove(Cell *cell, int direction) const {
   // Validate that this piece can move in the direction that you
   // want to move in, and ensure that the piece that you want to move towards is inbounds
   // and isn't already occupied.
   return IsValidDirection(cell, direction) && cell->neighborCells[direction]
    && !(cell->neighborCells[direction]->mask & (mBlackSet|mWhiteSet));
}

// TODO: Refactor this to use a Piece instead of a Cell.
inline bool CheckersBoard::CanJump(Cell *cell, int dir) const {
   // Validate that this piece can move in the direction that you
   // want to move in.
   if (!IsValidDirection(cell, dir))
      return false;

   // Validate that you're actually jumping over a piece (that you're not
   // trying to jump over a piece that exists out of bounds).
   if (cell->neighborCells[dir] == NULL)
      return false;

   // Validate that the piece you're jumping over belongs to the other player
   if ((mWhoseMove == kBlack && 
         ((cell->neighborCells[dir]->mask & mWhiteSet) == 0))
      || (mWhoseMove == kWhite &&
         ((cell->neighborCells[dir]->mask & mBlackSet) == 0))) {
      return false;
   }
      
   // Verify that the spot you want to jump into is inbounds
   // and empty.
   if (!cell->neighborCells[dir]->neighborCells[dir] ||
      (cell->neighborCells[dir]->neighborCells[dir]->mask 
      & (mBlackSet|mWhiteSet)) != 0) {
         return false;
   }

   return true;
}

// Validates that a particular cell is allowed to move in a given direction
// TODO: Refactor this to use a Piece instead of a Cell.
inline bool CheckersBoard::IsValidDirection(Cell *cell, int direction) const {
   // Kings can move any direction
   if (mWhoseMove == kBlack && (cell->mask & mBlackSet) != 0 
      && (mKingSet&cell->mask) != 0) {
      return true;
   } else if (mWhoseMove == kWhite && (cell->mask & mWhiteSet) != 0 
      && (mKingSet&cell->mask) != 0) {
      return true;
   }
   // Black non-kings can only move upwards
   else if (mWhoseMove == kBlack && (cell->mask & mBlackSet) != 0 
      && (direction == kNW || direction == kNE)) {
      return true;
   }
   // White non-kings can only move downwards
   else if (mWhoseMove == kWhite && (cell->mask & mWhiteSet) != 0 
      && (direction == kSW || direction == kSE)) {
      return true;
   }
   return false;
}

// [Staley] May add a public method for use by CheckersView.
// Public helper function that returns true if a cell is occupied
// by a certain color.
//
// "byWhom" expects either kWhite or kBlack.
bool CheckersBoard::CellOccupied(int row, int col, int byWhom) const {
   assert(GetCell(row, col) != NULL);
      
   if (byWhom == kWhite)
      return (!(GetCell(row, col)->mask & this->mWhiteSet));
   else if (byWhom == kBlack)
      return (!(GetCell(row, col)->mask & this->mBlackSet));
}

bool CheckersBoard::CellContainsKing(int row, int col) const {
   return ((GetCell(row,col)->mask & this->mKingSet) != 0);
}

// Helper function to add a piece on the board.
// ApplyMove() and UndoLastMove() should use Put() instead of this method.
inline void CheckersBoard::HalfPut(Piece *piece, Cell *cell) const {
   if (piece->color == kBlack) {
      mBlackSet |= cell->mask;
   } else if (piece->color == kWhite) {
      mWhiteSet |= cell->mask;
   } else assert(false);

   assert((mBlackSet & mWhiteSet) == 0);

   // If the piece to put down is a king, add the piece to mKingSet
   if (piece->isKing)
      mKingSet |= cell->mask;
}

// Helper function to remove a piece of a specific color.  
// Returns the Piece that was removed.
// ApplyMove() and UndoLastMove() should use Take() instead of this method.
inline CheckersBoard::Piece *CheckersBoard::HalfTake(Cell *cell, int color) const {     
   // Assert that the bitmasks don't overlap, so that you can safely
   // clear the mask from BOTH bitmasks.
   assert((mBlackSet & mWhiteSet) == 0);

   mBlackSet &= ~(cell->mask);
   mWhiteSet &= ~(cell->mask);

   // Figure out if this was a King or not.
   bool wasKing = (cell->mask & mKingSet);

   // Remove the cell from mKingSet before you finish
   mKingSet &= ~(cell->mask);

   // TODO: Does this have to dynamically allocate?
   return new Piece(wasKing, color, cell->loc);
}

// Returns NULL if parameters are invalid
inline CheckersBoard::Cell *CheckersBoard::GetCell(char row, unsigned col) {
   // Out of bounds
   if (row < 'A' || row > 'H' || col < 1 || col > 8) {
      return NULL;
   }

   // Reduce the inputted characters to their numerical form
   // (I drew up a diagram of this in my binder)
   row -= 'A';
   col -= 1;

   row = kWidth - row - 1;
      
   // Invalid square.  Row and Col must either be odd/even or even/odd,
   // respectively.
   if (!((IsEven(row) && IsOdd(col)) || (IsOdd(row) && IsEven(col)))) {
      return NULL;
   }

   return mCells + (row*kDim) + (col/2);
}