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

// TODO: Fix PylosBoard mOffs definition hack.
int PylosBoard::mOffs[kDim] = { 0, 0, 0, 0 };

void PylosBoard::StaticInit()
{
   Cell *cell;
   int level, row, col, ndx, nextSet = 0, nextCell = 0;

   for (level = 0; level < kDim; level++)
      for (row = 0; row < kDim - level; row++)
         for (col = 0; col < kDim - level; col++, nextCell++) {

            cell = mCells + nextCell;
            cell->level = level;
            cell->mask = 1 << nextCell;

            // Set up below and above pointers.
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

            // Add cell mask to horizontal/vertical alignments if relevant
         }

   // Add cell masks to square alignments

   // Copy set data back into cell set collections.
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
   // More work needed here.
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
   // Other stuff needed here, related to board valuation
   // This is a great place for a few asserts, too.

   HalfPut(trg);
}

void PylosBoard::TakeMarble(Spot *trg) {
   // Other stuff needed here, related to board valuation
   // This is a great place for a few asserts, too.

   HalfTake(trg);
}

void PylosBoard::ApplyMove(Move *move)
{
   PylosMove *tm = dynamic_cast<PylosMove *>(move);
   int rChange = -1;  // Start by assuming we'll lose one from reserve
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
   // Fill in
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
         if (trg && (trg->subs & (mWhite|mBlack)) == trg->subs) { // found a target spot
            locs.clear();
            locs.push_back(pair<int, int>(tRow, tCol));
            moves->push_back(new PylosMove(locs, PylosMove::kReserve));

            for (sRow = 0; sRow < kDim; sRow++) // search for promote moves
               for (sCol = 0; sCol < kDim; sCol++) {
                  src = mSpots[sRow][sCol].top;
                  if (src && (src->sups & (mWhite|mBlack)) == 0
                   && (src->mask & sideMask) && src->level < trg->level
                   && (sRow < tRow || sRow > tRow + 1    // Not a support for trg
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

// For each move in *mvs that completes one or more sets, add all
// combination of spots to take back.
void PylosBoard::AddTakeBacks(list<PylosMove *> *mvs) const {
   // You'll find HalfPut and HalfTake useful here.  You need to be able
   // to temporarily put/take marbles in order to make this logic manageable,
   // and you want it fast, so you don't want all the overhead of board
   // value management, since you'll ultimately leave the board unchanged
   // once the function is done.
}

Board::Move *PylosBoard::CreateMove() const
{
   return new PylosMove(PylosMove::LocVector(1), PylosMove::kReserve);
}

Board *PylosBoard::Clone() const
{
   // Think carefully about this one.  You should be able to do it in just
   // 5-10 lines.  Don't do needless work
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
	return is;
}

// Don't change this.  Make Read conform to it.
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
   // As with Clone, think carefully and don't do needless work.
}

// TODO: Implement reflection
const Class *PylosBoard::GetClass() const {
	return 0;
}
