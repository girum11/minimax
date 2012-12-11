#include <assert.h>
#include <memory.h>
#include <limits.h>
#include <algorithm>
#include "PylosDlg.h"
#include "PylosBoard.h"
#include "PylosMove.h"
#include "MyLib.h"
#include "BasicKey.h"
#include "PylosView.h"

using namespace std;

PylosBoard::Set PylosBoard::mSets[kNumSets];
PylosBoard::Cell PylosBoard::mCells[kNumCells];
int PylosBoard::mOffs[kDim];
PylosBoard::Rules PylosBoard::mRules;

BoardClass PylosBoard::mClass("PylosBoard",
                   &CreatePylosBoard,
                   "Pylos",
                   &PylosView::mClass,
                   &PylosDlg::mClass,
                   &PylosBoard::SetOptions,
                   &PylosBoard::GetOptions);

// The C++ definition here isn't required in C++11, which I'm using.
// Put it there anyways to force the "static block" to run.
PylosBoard::PylosBoardInitializer PylosBoard::mInitializer;

PylosBoard::PylosBoard() : mWhite(0), mBlack(0), mWhoseMove(kWhite),
   mWhiteReserve(kStones), mBlackReserve(kStones), mLevelLead(0), mFreeLead(0) {
      // Initialize mSpots
      ClearMSpots();
}

void PylosBoard::ClearMSpots() {
   for (int row = 0; row < kDim; row++) {
      for (int col = 0; col < kDim; col++) {
         mSpots[row][col].empty = GetCell(row, col, 0);
         mSpots[row][col].top = NULL;
      }
   }
}

// TODO: Don't use this code unless you need to debug, since it has magic
// numbers in it.
// Stolen helper function from StackOverflow, used to assert() 
// alignment bits being set correctly
// int PylosBoard::NumberOfSetBits(int i) {
//    i = i - ((i >> 1) & 0x55555555);
//    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
//    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
// }

void PylosBoard::StaticInit() {
   Cell *cell;
   int level = 0, row = 0, col = 0, ndx = 0, nextCell = 0;

   // Hold a set for the different alignments you can have
   Set horizontalAlignment = 0, verticalAlignment = 0, squareAlignment = 0;

   // Initialize mOffs
   mOffs[0] = 0;
   for (int i = 1; i < kDim; ++i) {
      mOffs[i] = mOffs[i-1] + (kDim - (i-1)) * (kDim - (i-1));
   }

   // Initialize mCells
   for (level = 0; level < kDim; level++) {
      for (row = 0; row < kDim - level; row++) {
         for (col = 0; col < kDim - level; col++, nextCell++) {

            cell = mCells + nextCell;
            cell->level = level;
            cell->mask = 1 << nextCell;

            // [Staley] Set up below and above pointers.
            if (level > 0) {
               cell->below[kNW] = GetCell(row, col, level-1);
               cell->below[kNE] = GetCell(row, col+1, level-1);
               cell->below[kSE] = GetCell(row+1, col+1, level-1);
               cell->below[kSW] = GetCell(row+1, col, level-1);

               cell->below[kNW]->above = cell;

               for (ndx = 0; ndx < kSqr; ndx++) {
                  cell->below[ndx]->sups |= cell->mask;
                  cell->subs |= cell->below[ndx]->mask;
               }
            }
            // Initialize below[] pointers to be NULL for cells at bottom level.
            // This is so that my HalfTake() logic can work.
            else if (level == 0) {
               for (int dir = 0; dir < kSqr; dir++) {
                  cell->below[dir] = NULL;
               }
            }
         }
      }
   }

   // Clean out mSets to assert that it was properly filled later
   for (int level = 0; level < kNumSets; level++)
      mSets[level] = 0x0;

   // Initialize mSets
   // [Staley] Add cell mask to horizontal/vertical alignments if relevant
   int setNum = 0;
   // There cannot be horizontal/vertical alignments on the top two levels
   for (level = 0; level < kDim - 2; level++) {
      // Step across this level's grid diagonally
      for (int rowcol = 0; rowcol < kDim - level; rowcol++) {
         // Clean off the alignments
         horizontalAlignment = verticalAlignment = 0x0;

         // Create the horizontal and vertical alignment for this node
         for (int i = 0; i < kDim - level; i++) {
            horizontalAlignment |= GetMask(rowcol, i, level);
            verticalAlignment |= GetMask(i, rowcol, level);
         }

         // Throw the newly generated alignments into mSets
         mSets[setNum++] = horizontalAlignment;
         mSets[setNum++] = verticalAlignment;
      }
   }

   // [Staley] Add cell masks to square alignments.  All levels can potentially
   // have square alignments, except for the very top level.
   for (level = 0; level < kDim - 1; level++) {
      for (row = 0; row < kDim - level; row++) {
         for (col = 0; col < kDim - level; col++) {
            if (InBounds(row+1,col+1,level)) {
               // Reset the square alignment
               squareAlignment = 0x0;

               squareAlignment |= GetMask(row  , col  , level);
               squareAlignment |= GetMask(row  , col+1, level);
               squareAlignment |= GetMask(row+1, col  , level);
               squareAlignment |= GetMask(row+1, col+1, level);

               // Add the square alignment to mSets
               mSets[setNum++] = squareAlignment;
            }
         }
      }
   }


   // TODO: Don't use this code unless you need to debug, since it relies
   // on a helper function from StackOverflow with magic numbers in it.
//    Sanity checks before we go back and copy set data
//       // back into cell set collection.
//       assert(setNum == kNumSets);
//       for (int level = 0; level < kNumSets; level++) {
//          // Assert that each alignment was properly set
//          assert(mSets[level] != 0x0);
//    
//          // Verify level 0 horizontal/vertical alignments
//          if (level >= 0 && level < 8)
//             assert(NumberOfSetBits(mSets[level]) == 4);
//          // Verify level 1 horizontal/vertical alignments
//          else if (level >= 8 && level < 14)
//             assert(NumberOfSetBits(mSets[level]) == 3);
//          // Verify square alignments
//          else if (level >= 14 && level < 28)
//             assert(NumberOfSetBits(mSets[level]) == 4);
//       }

   // [Staley] Copy set data back into cell set collections.
   // For each cell, check all of the alignments to see if
   // the cell belongs to that alignment.
   for (level = 0; level < kDim; level++) {
      for (row = 0; row < kDim - level; row++) {
         for (col = 0; col < kDim - level; col++) {
            int setCounter = 0;
            for (int i = 0; i < kNumSets; i++) {
               // If this cell is part of this alignment, then add
               // this alignment to that cell's set of alignments
               if (mSets[i] & GetMask(row, col, level)) {
                  // Make sure we're not indexing the array out of bounds
                  assert(setCounter < kSetsPerCell);
                  GetCell(row, col, level)->sets[setCounter++] = mSets[i];
               }
            }
         }
      }
   }
}

void PylosBoard::PutMarble(Spot *trg) {
   // [Staley] Other stuff needed here, related to board valuation
   // [Staley] This is a great place for a few asserts, too.
   // "Board valuation" updates is to update the values of 
   // mWhiteReserve, mBlackReserve, mLevelLead, and mFreeLead

   HalfPut(trg);

   // Make sure that there aren't any spots that have both a white
   // piece and a black piece in the same spot.
   assert((mWhite & mBlack) == 0x0);

   // Update mLevelLead and mFreeLead
   // UpdateBoardValuation();
}

void PylosBoard::TakeMarble(Spot *trg) {
   // [Staley] Other stuff needed here, related to board valuation
   // [Staley] This is a great place for a few asserts, too.
   // "Board valuation" updates is to update the values of 
   // mWhiteReserve, mBlackReserve, mLevelLead, and mFreeLead

   HalfTake(trg);

   // Make sure that there aren't any spots that have both a white
   // piece and a black piece in the same spot. Play it safe here.
   assert((mWhite & mBlack) == 0x0);

   // Update mLevelLead and mFreeLead
   // UpdateBoardValuation();
}

void PylosBoard::UpdateBoardValuation() {

   // A player can never actually put down the final marble, since you have
   // to fill up Level 2 to place the marble.  Filling up Level 2 would
   // drain the other player's marbles, ending the game.  Thus, it should never
   // be true that both players have 0 marbles.
   assert(!(mWhiteReserve == 0 && mBlackReserve == 0));

   // Go through all pieces and update mLevelLead and mFreeLead
   Set allPieces(mWhite|mBlack);
   mLevelLead = mFreeLead = 0;
   for (int level = 0; level < kDim; level++) {
      for (int row = 0; row < kDim - level; row++) {
         for (int col = 0; col < kDim - level; col++) {

            Cell *cell = GetCell(row, col, level);
            // Update mLevelLead
            if (cell->mask & mWhite) {
               mLevelLead += level;
            } else if (cell->mask & mBlack)  {
               mLevelLead -= level;
            }

            // Update mFreeLead
            if ((cell->sups & allPieces) == 0 && (cell->mask & mWhite)) {
               mFreeLead += 1;
            } else if ((cell->sups & allPieces) == 0 && (cell->mask & mBlack)) {
               mFreeLead -= 1;
            }
         }
      }
   }
}

void PylosBoard::ApplyMove(Move *move) {
   PylosMove *tm = dynamic_cast<PylosMove *>(move);

   int rChange = -1;  // [Staley] Start by assuming we'll lose one from reserve
   PylosMove::LocVector::iterator locIter = tm->mLocs.begin();

   PutMarble(&mSpots[(*locIter).first][(*locIter).second]);

   locIter++;
   for (; locIter != tm->mLocs.end(); locIter++) {
      TakeMarble(&mSpots[(*locIter).first][(*locIter).second]);
      rChange++;
   }

   // Update the reserve weights, once you've finished calculating rChange
   if (mWhoseMove == kWhite)
      mWhiteReserve += rChange;
   else if (mWhoseMove == kBlack)
      mBlackReserve += rChange;
   else assert(false);

   UpdateBoardValuation();

   // Change whose move it is and keep a track of this move in mMoveHist
   mMoveHist.push_back(move);
   mWhoseMove = -mWhoseMove;
}

void PylosBoard::UndoLastMove() {
   // [Staley] Fill in
   // [Ian] Basically, do ApplyMove() backwards (obviously)

   PylosMove *moveToUndo = dynamic_cast<PylosMove *>(mMoveHist.back());

   // Start by assuming that the reserve will gain a new piece.
   int rChange = 1;
   PylosMove::LocVector::iterator itr = moveToUndo->mLocs.begin();

   // Switch whose move it is (early, so that you're taking away the turn
   // that happened BEFORE this current turn)
   mWhoseMove = -mWhoseMove;

   // cout << "Trying to undo move: " << (string) *moveToUndo << endl;

   // If the piece was a Promote piece, or if it removed pieces, put those back
   // first, before you take away any pieces you put down.
   itr++;
   for (; itr != moveToUndo->mLocs.end(); itr++) {
      PutMarble(&mSpots[(*itr).first][(*itr).second]);
      rChange--;
   }

   // Now, take away that first piece that you put down
   itr = moveToUndo->mLocs.begin();
   TakeMarble(&mSpots[(*itr).first][(*itr).second]);

   // Make changes to reserve counts
   if (mWhoseMove == kWhite) {
      mWhiteReserve += rChange;
   } else if (mWhoseMove == kBlack) {
      mBlackReserve += rChange;
   } else assert(false);

   UpdateBoardValuation();

   // Destroy history of the move
   delete moveToUndo;
   mMoveHist.pop_back();
}

void PylosBoard::GetAllMoves(list<Move *> *uncastMoves) const {
   int tRow, tCol, sRow, sCol;
   Cell *trg, *src;
   PylosMove::LocVector locs;
   list<PylosMove *>::iterator itr;
   list<PylosMove *> *moves = reinterpret_cast<list<PylosMove *>*>(uncastMoves);
   ulong sideMask = mWhoseMove == kWhite ? mWhite : mBlack;

   assert(uncastMoves->size() == 0 && moves->size() == 0);

   // Short-circuit out if one of the players has no more marbles.  The player
   // whose move it is currently should be the winner.
   if (mWhiteReserve == 0 || mBlackReserve == 0)
      return;

   for (tRow = 0; tRow < kDim; tRow++)
      for (tCol = 0; tCol < kDim; tCol++) {
         trg = mSpots[tRow][tCol].empty;
         if (trg && (trg->subs & (mWhite|mBlack)) == trg->subs) { 
            locs.clear();
            locs.push_back(pair<int, int>(tRow, tCol));
            moves->push_back(new PylosMove(locs, PylosMove::kReserve));

            for (sRow = 0; sRow < kDim; sRow++)
               for (sCol = 0; sCol < kDim; sCol++) {
                  src = mSpots[sRow][sCol].top;
                  if (src && (src->sups & (mWhite|mBlack)) == 0
                   && (src->mask & sideMask) && src->level < trg->level
                   && (sRow < tRow || sRow > tRow + 1    
                   || sCol < tCol || sCol > tCol + 1)) {
                     locs.push_back(pair<int, int>(sRow, sCol));
                     moves->push_back(new PylosMove(locs, PylosMove::kPromote));
                     locs.pop_back();
                  }
               }
         }
      }

   AddTakeBacks(moves);

}

// [Staley] For each move in *moves that completes one or more sets, add all
// [Staley] combination of spots to take back.
void PylosBoard::AddTakeBacks(list<PylosMove *> *moves) const {
   // Hints:
   // The reason you want to HalfPut and HalfTake here is so that you
   // can verify that the possibility of adding in a new Cell creates possible
   // "takeback moves."
   // The basic logic of verifying that a move creates an alignment is to simply
   // AND each of the Sets from sets[] of each cell against mWhite and mBlack.


   // First, make a temporary copy of the original moves.
   // This is so that you don't add to the list as you loop through it.
   list<PylosMove *> movesCopy(*moves);

   for (list<PylosMove *>::const_iterator movesCopyIter = movesCopy.begin(); 
    movesCopyIter != movesCopy.end(); movesCopyIter++) {
      // Grab the cell that we're thinking about putting down
      PylosMove *move = *movesCopyIter;
      Spot *moveSpot = &mSpots[move->mLocs[0].first][move->mLocs[0].second];
      Spot *promoteFromSpot = NULL;
      Cell *moveCell = moveSpot->empty;

      // Sanity check:  A possible move shouldn't be able to be applied to a 
      // filled Spot.
      assert(moveCell != NULL);

      // Straightaway, put down the marble to inspect the new state of the 
      // board (to check for possible alignments).  Also, don't forget to 
      // HalfTake() in case we're promoting a marble.
      HalfPut(moveSpot);
      if (move->mType == PylosMove::kPromote) {
         promoteFromSpot = &mSpots[move->mLocs[1].first][move->mLocs[1].second];
         HalfTake(promoteFromSpot);
      }

      // Find the iterator that points to where you want to add moves to.
      // WARNING:  THIS IS SLOW.  This one line of code causes 
      // AddTakeBacks to be O(n^2).
      list<PylosMove *>::iterator movesIter = 
       std::find(moves->begin(), moves->end(), *movesCopyIter);

      if (mWhoseMove == kWhite)
         CalculateAllTakebacks(moves, movesIter, &mWhite, move, moveCell);
      else if (mWhoseMove == kBlack)
         CalculateAllTakebacks(moves, movesIter, &mBlack, move, moveCell);
      else assert(false);

      // Don't forget to take away that "trial" attempt of putting down a
      // marble, along with putting back the one you may have taken away if
      // it was a Promote move.
      HalfTake(moveSpot);
      if (move->mType == PylosMove::kPromote) {
         HalfPut(promoteFromSpot);
      }
   }
}

void PylosBoard::CalculateAllTakebacks(list<PylosMove *> *allMoves,
 list<PylosMove *>::iterator moveIter, Set *mSet,
 PylosMove *move, Cell *moveCell) const {

   // TODO: These sets can probably be lists instead, and insert at O(1)
   // instead of O(logn).
   set<pair<short,short> > fMar1;
   set<pair<short,short> > fMar2;
   bool alignmentFound = false;

   // For each of this cell's possible alignments,
   for (int i = 0; i < kSetsPerCell && !alignmentFound; i++) {
      // If putting down this cell creates a new alignment 
      // (checking against all possible alignments),
      if (moveCell->sets[i] &&
       (moveCell->sets[i] & *mSet) == moveCell->sets[i]) {

         // Then you've ID'd a move that "completes one or more sets."
         // From this point you should just go through and compile a list
         // of "freeMarbles" -- marbles that do not support other marbles.
         FindFreeMarbles(&fMar1, mSet);

         // Once you've compiled a list of "free marbles," then add
         // all combinations of marbles you can take on this turn to
         // the list of Moves you can make
         for (set<pair<short,short> >::const_iterator fmIter1 = 
          fMar1.begin(); fmIter1 != fMar1.end(); fmIter1++) {

            // Grab the Spot that corresponds to the free marble that we're 
            // about to yank 
            Spot *freeMarble1 = &mSpots[(*fmIter1).first][(*fmIter1).second];
            assert(freeMarble1->top != NULL);

            // Construct the "takeback move" using the data from the 
            // potentialMove and the location of freeMarble1
            PylosMove *takebackMove = new PylosMove(move->mLocs, move->mType);
            takebackMove->mLocs.push_back(*fmIter1);
            takebackMove->AssertMe();

            // Throw the new takebackMove into the list of all moves
            moveIter++;
            allMoves->insert(moveIter, takebackMove);
            moveIter--;

            // Make sure to HalfTake() freeMarble1 to update state of the 
            // board, in preparation for finding all possible freeMarble2's
            HalfTake(freeMarble1);

            // At this point, taking away freeMarble1 might have freed up 
            // additional marbles.  Check those as well.
            FindFreeMarbles(&fMar2, mSet, (*fmIter1).first, (*fmIter1).second);

            // For each of freeMarbles2, 
            for (set<pair<short,short> >::const_iterator fmIter2 = 
             fMar2.begin(); fmIter2 != fMar2.end(); fmIter2++) {
               // IT IS NOT TRUE THAT: 
               // assert(*freeMarbleIter1 != *freeMarbleIter2);
               // That is, it IS possible to take the same spot twice, since 
               // taking a spot can potentially reveal another freeMarble with 
               // the same "spot number.

               // Construct the "takeback move" using freeMarble1 AND 
               // freeMarble2. Ensure that they're ordered.
               takebackMove = new PylosMove(move->mLocs, move->mType);
               takebackMove->mLocs.push_back(*fmIter1);
               takebackMove->mLocs.push_back(*fmIter2);

               takebackMove->AssertMe();

               // Throw this one into the list of all moves.
               allMoves->insert(++moveIter, takebackMove);
               --moveIter;
            }

            // Don't forget to HalfPut() freeMarble1 back into the board for 
            // the next iteration.
            HalfPut(freeMarble1);
            fMar2.clear();
         }

         // If you *did* find an alignment, then you should NOT check the rest
         // of the possible alignments, since you'd only get duplicate moves.
         alignmentFound = true;
      }

      fMar1.clear();
   }
}

// WARNING: Preventing duplicates is hard.  Right now, I'm preventing
// them by simply only calling FindFreeMarbles on rows/cols that are PAST
// the current free marble.  If there's a Bender error, here's where
// you can start.
//
// BUG: Instead of truly finding free marbles that are PAST the current
// free marble, I'm only finding marbles with row AND col values that are
// greater than or equal to startRow.  Instead, iterate through the rest
// of this row, and then step down to the next row and iterate through
// to the end as normal.
void PylosBoard::FindFreeMarbles(std::set<std::pair<short,short> > *freeMarbles, 
   Set *playerMarbles, unsigned short startRow, unsigned short startCol) const {
   // Quick sanity check
   assert(freeMarbles && freeMarbles->size() == 0);

   // First, iterate through the rest of this starting row.
   for (int row = startRow; row < kDim; row++) {
      for (int col = startCol; col < kDim; col++) {
         InsertIfFree(freeMarbles, playerMarbles, row, col);
      }
   }

   // Then, iterate through the rest of the board as usual
   for (int row = startRow + 1; row < kDim; row++) {
      for (int col = 0; col < kDim; col++) {
         InsertIfFree(freeMarbles, playerMarbles, row, col);
      }
   }
}

// A marble is "free" if it does not support any other marbles.
// Bitwise, this means that all of the possible marbles it can 
// sup[port] are NOT present in the current board -- black OR white.
// Also, the freeMarble to take back must belong to that player.
void PylosBoard::InsertIfFree(std::set<std::pair<short,short> > *freeMarbles,
   Set *playerMarbles, int row, int col) const {
      Cell *marble = mSpots[row][col].top;

      if (marble && (marble->mask & *playerMarbles)
            && (marble->sups & (mWhite|mBlack)) == 0) {
      freeMarbles->insert(std::pair<short,short>(row,col));
   }
}

Board::Move *PylosBoard::CreateMove() const {
   return new PylosMove(PylosMove::LocVector(1), PylosMove::kReserve);
}

// mMoveHist bug isn't in Clone() for sure.
Board *PylosBoard::Clone() const {
   // [Staley] Think carefully about this one.  You should be able to do it in 
   // just 5-10 lines.  Don't do needless work.

   PylosBoard *boardCopy = new PylosBoard(*this);
   list<Move *>::iterator moveHistIter;
   // [Ian] Memberwise copies don't work for pointers... 

   // Yeah... my MoveHistory. You need to go through and deep copy the 
   // moveHistory, or else you'll have a shallow copy of the pointers and 
   // not the pointers' data.
   boardCopy->mMoveHist.clear();
   for (moveHistIter = boardCopy->mMoveHist.begin(); 
    moveHistIter != boardCopy->mMoveHist.end(); moveHistIter++) {
      // my old code before I looked at Clint's OthelloBoard::Clone()
      /*PylosMove *castedMove = dynamic_cast<PylosMove *>(*moveHistIter);
      Move *moveCopy = new PylosMove(castedMove->mLocs, castedMove->mType);
      boardCopy->mMoveHist.push_back(moveCopy);*/
      *moveHistIter = (*moveHistIter)->Clone();
   }

   return boardCopy;
}


void PylosBoard::Delete() {
   // [Staley] As with Clone, think carefully and don't do needless work.

   // Delete all NONSTATIC member datum.  Do NOT delete static member datum.

   ClearMSpots();
   mWhite = mBlack = 0x0;
   mWhoseMove = kWhite;
   mWhiteReserve = mBlackReserve = kStones;
   mLevelLead = mFreeLead = 0;

   for (list<Move *>::iterator iter = mMoveHist.begin();
    iter != mMoveHist.end(); iter++) {
       delete *iter;
   }
   mMoveHist.clear();

   // DON'T reset the static rules to their defaults, since this is a member
   // function, and Delete() gets called on the cmpBoard in the BoardTest.
//    PylosBoard::mRules.marbleWgt = 100;
//    PylosBoard::mRules.levelWgt = 20;
//    PylosBoard::mRules.freeWgt = 6;
}

Board::Key *PylosBoard::GetKey() const {
   BasicKey<2> *rtn = new BasicKey<2>();

   rtn->vals[0] = (mWhoseMove == kWhite) << kNumCells | mWhite;
   rtn->vals[1] = mBlack;

   return rtn;
}


istream &PylosBoard::Read(istream &is) {
   // [Staley] Fill in (conform to Write() method below)

   // Assigned reading for this: http://cplusplus.com/doc/tutorial/files/
   // Read() is mostly Write() backwards, with a few exceptions.
   // Order's important.
   int moveCount = -1;

   // Clear out the Default board's existing data
   Delete();

   // Read in the Rules that the board should use.
   is.read((char *)&PylosBoard::mRules, sizeof(Rules));
   mRules.EndSwap();

   is.read((char *)&moveCount, sizeof(moveCount));
   assert(moveCount != -1);  // sanity check to ensure the read() happened
   for (int i = 0; i < moveCount; i++) {
      // Will this mLocs on the RTS still be there, since it's being
      // inserted as a member variable of something on the RTH?
      // Well, mLocs *should* be copy-constructed in.
      PylosMove *newMove = new PylosMove(PylosMove::LocVector(),0);
      is >> *newMove;

      ApplyMove(newMove);
   }

   return is;
}

// Don't change this.  Make Read conform to it.
ostream &PylosBoard::Write(ostream &os) const {
   Rules rls = mRules;
   list<Move *>::const_iterator itr;
   int mvCount = EndianXfer((int)mMoveHist.size());

   rls.EndSwap();
   os.write((char *)&rls, sizeof(rls));

   os.write((char *)&mvCount, sizeof(mvCount));
   for (itr = mMoveHist.begin(); itr != mMoveHist.end(); itr++)
      os << **itr;

   return os;
}


// [Staley] Write the two methods GetOptions and SetOptions of PylosBoard, 
// [Staley] without changing their signature in the PylosBoard.h file.  
// [Staley] (You'll need to cast to and from void *.)  Do not worry about 
// [Staley] setting up a Class object yet; that will come in step 7.  You just 
// [Staley] want the methods ready for initializing the Class object. 
// [Staley] 
// [Staley] The void * returned from GetOptions, both in Pylos and other Board 
// [Staley] types, is owned by the caller.  And, the caller retains ownership 
// [Staley] of the void * passed to SetOptions.
// [Staley] 
// [Staley] Create PylosDlg.h/cpp, and make a new PylosDlg object in your 
// [Staley] BoardTest main directly via new.  Test the GetOptions and 
// [Staley] SetOptions methods by implementing the setOptions command, using 
// [Staley] the PylosDlg object.
void *PylosBoard::GetOptions() {
   return new Rules(mRules);
}

// [Staley] Write the two methods GetOptions and SetOptions of PylosBoard, 
// [Staley] without changing their signature in the PylosBoard.h file.  
// [Staley] (You'll need to cast to and from void *.)  Do not worry about 
// [Staley] setting up a Class object yet; that will come in step 7.  You just 
// [Staley] want the methods ready for initializing the Class object. 
// [Staley] 
// [Staley] The void * returned from GetOptions, both in Pylos and other Board 
// [Staley] types, is owned by the caller.  And, the caller retains ownership 
// [Staley] of the void * passed to SetOptions.
// [Staley] 
// [Staley] Create PylosDlg.h/cpp, and make a new PylosDlg object in your 
// [Staley] BoardTest main directly via new.  Test the GetOptions and 
// [Staley] SetOptions methods by implementing the setOptions command, using 
// [Staley] the PylosDlg object.
void PylosBoard::SetOptions(const void *opts) {
   mRules = *reinterpret_cast<const Rules *>(opts);
}

void PylosBoard::Rules::SetMarble(int val) {
   if (val < 1 || val > 1000)
      throw BaseException(
       "Marble weight must be between 1 and 1000 inclusive\n");
   this->marbleWgt = val;
}

void PylosBoard::Rules::SetLevel(int val) {
   if (val >= this->marbleWgt || val < 0)
      throw BaseException("Level weight must be nonnegative and less than"
      " marble weight\n");

   this->levelWgt = val;
}

void PylosBoard::Rules::SetFree(int val) {
   if (val % 2 != 0 || val < 0 || val >= this->marbleWgt)
      throw BaseException("Free weight must be even, nonnegative, and less"
      " than marble weight\n");

   this->freeWgt = val;
}

void PylosBoard::Rules::EndSwap() {
   this->levelWgt = EndianXfer(this->levelWgt);
   this->freeWgt = EndianXfer(this->freeWgt);
   this->marbleWgt = EndianXfer(this->marbleWgt);
}

long PylosBoard::GetValue() const {
   if (mWhiteReserve == 0)
      return -kWinVal;
   else if (mBlackReserve == 0)
      return kWinVal;
   else
      return mRules.marbleWgt*(mWhiteReserve - mBlackReserve)
      + mRules.levelWgt * mLevelLead + mRules.freeWgt * mFreeLead;
}

