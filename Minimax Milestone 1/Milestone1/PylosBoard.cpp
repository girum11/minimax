#include <assert.h>
#include <memory.h>
#include <limits.h>
#include "PylosDlg.h"
#include "PylosBoard.h"
#include "PylosMove.h"
#include "MyLib.h"
#include "BasicKey.h"

////////////////////////////////////////////////////////////////////////////////
using namespace std;

PylosBoard::Set PylosBoard::mSets[kNumSets];
PylosBoard::Cell PylosBoard::mCells[kNumCells];
int PylosBoard::mOffs[kDim];

// The C++ definition here isn't required in C++11, which I'm using.
// Put it there anyways to force the "static block" to run.
PylosBoard::PylosBoardInitializer PylosBoard::mInitializer;

// Stolen helper function from StackOverflow, used to assert()
// alignment bits being set correctly
int NumberOfSetBits(int i) {
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void PylosBoard::StaticInit() {
   cout << "StaticInit() called!!!!" << endl;

   Cell *cell;
   int level = 0, row = 0, col = 0, ndx = 0, nextSet = 0, nextCell = 0;

   // Hold a set for the different alignments you can have
   Set horizontalAlignment = 0, verticalAlignment = 0, squareAlignment = 0;

   // Initialize mOffs
   mOffs[0] = 0;
   mOffs[1] = 16;
   mOffs[2] = 25;
   mOffs[3] = 29;


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
         }
      }
   }

   // Clean out mSets to assert that it was properly filled later
   for (int i = 0; i < kNumSets; i++)
      mSets[i] = 0x0;
   
   // Initialize mSets
   // [Staley] Add cell mask to horizontal/vertical alignments if relevant
   int setNum = 0;
   for (level = 0; level < 2; level++) {
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

   // [Staley] Add cell masks to square alignments
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

   // [Staley] Copy set data back into cell set collections.
   for (int i = 0; i < kNumSets; i++) {
      // Assert that each alignment was properly set
      assert(mSets[i] != 0x0);

      // Verify level 0 horizontal/vertical alignments
      if (i >= 0 && i < 8)
         assert(NumberOfSetBits(mSets[i]) == 4);
      // Verify level 1 horizontal/vertical alignments
      else if (i >= 8 && i < 14)
         assert(NumberOfSetBits(mSets[i]) == 3);
      // Verify square alignments
      else if (i >= 14 && i < 28)
         assert(NumberOfSetBits(mSets[i]) == 4);
      
      // For every block, add the alignments the block is a part of
      int setCounter = 0;
      for (level = 0; level < kDim; level++)
         for (row = 0; row < kDim - level; row++)
            for (col = 0; col < kDim - level; col++)
               // If this block is part of this alignment, then set it
               if (mSets[i] & GetMask(row, col, level))
                  GetCell(row, col, level)->sets[setCounter++] = mSets[i];
   }
}



void PylosBoard::Rules::SetMarble(int val) {

   if (val < 1 || val > 1000)
      throw BaseException("Marble weight must be between 1 and 1000 inclusive");

   marbleWgt = val;
}

void PylosBoard::Rules::SetLevel(int val) {
   if (val >= marbleWgt || val < 0)
      throw BaseException("Level weight must be nonnegative and less than"
      " marble weight");

   levelWgt = val;
}

void PylosBoard::Rules::SetFree(int val) {
   if (val % 2 != 0 || val < 0 || val >= marbleWgt)
      throw BaseException("Free weight must be even, nonnegative, and less"
      " than marble weight");

   freeWgt = val;
}

void PylosBoard::Rules::EndSwap() {
   levelWgt = EndianXfer(levelWgt);
   freeWgt = EndianXfer(freeWgt);
   marbleWgt = EndianXfer(marbleWgt);
}

PylosBoard::PylosBoard() : mWhite(0), mBlack(0), mWhoseMove(kWhite),
   mWhiteReserve(kStones), mBlackReserve(kStones), mLevelLead(0), mFreeLead(0)
{
   // [*Staley] More work needed here.
   // [Ian] This is where you construct member data.  Try going through with a 
   // highlighter, marking red for static member data that has been initialized
   // in StaticInit(), and yellow for non-static member data that has been
   // initialized in this constructor.

   // Initialize mSpots
   for (int row = 0; row < kDim; row++) {
      for (int col = 0; col < kDim; col++) {
         mSpots[row][col].empty = GetCell(row, col, 0);
         mSpots[row][col].top = NULL;
      }
   }

}

PylosBoard::Rules PylosBoard::mRules;

long PylosBoard::GetValue() const
{
   if (mWhiteReserve == 0)
      return -kWinVal;
   else if (mBlackReserve == 0)
      return kWinVal;
   else
      return mRules.marbleWgt*(mWhiteReserve - mBlackReserve)
      + mRules.levelWgt * mLevelLead + mRules.freeWgt * mFreeLead;
}

void PylosBoard::PutMarble(Spot *trg) {
   // [*Staley] Other stuff needed here, related to board valuation
   // [*Staley] This is a great place for a few asserts, too.
      
   
   HalfPut(trg);
  
   // Make sure that there aren't any spots that have both a white
   // piece and a black piece in the same spot.
   assert(mWhite & mBlack == 0x0);
}

void PylosBoard::TakeMarble(Spot *trg) {
   // [*Staley] Other stuff needed here, related to board valuation
   // [*Staley] This is a great place for a few asserts, too.

   HalfTake(trg);

   // Make sure that there aren't any spots that have both a white
   // piece and a black piece in the same spot. Play it safe here.
   assert(mWhite & mBlack == 0x0);
}

void PylosBoard::ApplyMove(Move *move)
{
   PylosMove *tm = dynamic_cast<PylosMove *>(move);
   int rChange = -1;  // [Staley] Start by assuming we'll lose one from reserve
   PylosMove::LocVector::iterator itr = tm->mLocs.begin();

   PutMarble(&mSpots[(*itr).first][(*itr).second]);

   itr++;
   for (; itr != tm->mLocs.end(); itr++) {
      TakeMarble(&mSpots[(*itr).first][(*itr).second]);
      rChange++;
   }

   if (mWhoseMove == kWhite)
      mWhiteReserve += rChange;
   else
      mBlackReserve += rChange;

   mMoveHist.push_back(move);
   mWhoseMove = -mWhoseMove;
}

void PylosBoard::UndoLastMove() {
   // [*Staley] Fill in
   // [Ian] Basically, do ApplyMove() backwards (obviously)
}

void PylosBoard::GetAllMoves(list<Move *> *moves) const
{
   int tRow, tCol, sRow, sCol;
   Cell *trg, *src;
   PylosMove::LocVector locs;
   list<PylosMove *>::iterator itr;
   list<PylosMove *> *mvs = reinterpret_cast<list<PylosMove *>*>(moves);
   ulong sideMask = mWhoseMove == kWhite ? mWhite : mBlack;

   moves->clear();
   if (mWhiteReserve == 0 || mBlackReserve == 0)
      return;

   for (tRow = 0; tRow < kDim; tRow++)
      for (tCol = 0; tCol < kDim; tCol++) {
         trg = mSpots[tRow][tCol].empty;
         if (trg && (trg->subs & (mWhite|mBlack)) == trg->subs) { // [Staley] found a target spot
            locs.clear();
            locs.push_back(pair<int, int>(tRow, tCol));
            moves->push_back(new PylosMove(locs, PylosMove::kReserve));

            for (sRow = 0; sRow < kDim; sRow++) // [Staley] search for promote moves
               for (sCol = 0; sCol < kDim; sCol++) {
                  src = mSpots[sRow][sCol].top;
                  if (src && (src->sups & (mWhite|mBlack)) == 0
                     && (src->mask & sideMask) && src->level < trg->level
                     && (sRow < tRow || sRow > tRow + 1    // [Staley] Not a support for trg
                     || sCol < tCol || sCol > tCol + 1)) {
                        locs.push_back(pair<int, int>(sRow, sCol));
                        moves->push_back(new PylosMove(locs, PylosMove::kPromote));
                        locs.pop_back();
                  }
               }
         }
      }

      AddTakeBacks(mvs);
}

// [*Staley] For each move in *mvs that completes one or more sets, add all
// [*Staley] combination of spots to take back.
void PylosBoard::AddTakeBacks(list<PylosMove *> *mvs) const {
   // [Staley] You'll find HalfPut and HalfTake useful here.  You need to be able
   // [Staley] to temporarily put/take marbles in order to make this logic manageable,
   // [Staley] and you want it fast, so you don't want all the overhead of board
   // [Staley] value management, since you'll ultimately leave the board unchanged
   // [Staley] once the function is done.
}

Board::Move *PylosBoard::CreateMove() const
{
   return new PylosMove(PylosMove::LocVector(1), PylosMove::kReserve);;
}

Board *PylosBoard::Clone() const
{
   // [*Staley] Think carefully about this one.  You should be able to do it in just
   // [*Staley] 5-10 lines.  Don't do needless work
   return 0;
}

Board::Key *PylosBoard::GetKey() const
{
   BasicKey<2> *rtn = new BasicKey<2>();

   rtn->vals[0] = (mWhoseMove == kWhite) << kNumCells | mWhite;
   rtn->vals[1] = mBlack;

   return rtn;
}


istream &PylosBoard::Read(istream &is)
{
   // [Staley] Fill in (conform to Write() method below)
   return is;
}

// [Staley] Don't change this.  Make Read conform to it.
ostream &PylosBoard::Write(ostream &os) const
{
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

void *PylosBoard::GetOptions()
{
   return new Rules(mRules);
}

void PylosBoard::SetOptions(const void *opts)
{
   mRules = *reinterpret_cast<const Rules *>(opts);
}

void PylosBoard::Delete()
{
   // [*Staley] As with Clone, think carefully and don't do needless work.
}

// TODO: Implement reflection
const Class *PylosBoard::GetClass() const {
   return 0;
}
