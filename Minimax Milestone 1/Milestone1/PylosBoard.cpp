#include <assert.h>
#include <memory.h>
#include <limits.h>
#include "PylosDlg.h"
#include "PylosBoard.h"
// TODO: PylosMove cannot be imported in PylosBoard
#include "PylosMove.h"
#include "MyLib.h"
#include "BasicKey.h"


////////////////////////////////////////////////////////////////////////////////
using namespace std;

PylosBoard::Set PylosBoard::mSets[kNumSets];
PylosBoard::Cell PylosBoard::mCells[kNumCells];
int PylosBoard::mOffs[kDim];
PylosBoard::Rules PylosBoard::mRules;

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


PylosBoard::PylosBoard() : mWhite(0), mBlack(0), mWhoseMove(kWhite),
   mWhiteReserve(kStones), mBlackReserve(kStones), mLevelLead(0), mFreeLead(0) {
      // [Staley] More work needed here.
      // [Ian] This is where you construct member data.  Try going through with a 
      // highlighter, marking red for static member data that has been initialized
      // in StaticInit(), and yellow for non-static member data that has been
      // initialized in this constructor.

      // Initialize mSpots
      InitializeMSpots();
}

void PylosBoard::InitializeMSpots() {
   for (int row = 0; row < kDim; row++) {
      for (int col = 0; col < kDim; col++) {
         mSpots[row][col].empty = GetCell(row, col, 0);
         mSpots[row][col].top = NULL;
      }
   }
}

void PylosBoard::StaticInit() {
   Cell *cell;
   int level = 0, row = 0, col = 0, ndx = 0, nextSet = 0, nextCell = 0;

   // Hold a set for the different alignments you can have
   Set horizontalAlignment = 0, verticalAlignment = 0, squareAlignment = 0;

   // Initialize mOffs  
   // TODO No magic numbers
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
   // There cannot be alignments on the top two levels
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


   // Sanity checks before we go back and copy set data
   // back into cell set collection.
   assert(setNum == kNumSets);
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
   }

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

long PylosBoard::GetValue() const {
   if (mWhiteReserve == 0)
      return -kWinVal;
   else if (mBlackReserve == 0)
      return kWinVal;
   else
      return mRules.marbleWgt*(mWhiteReserve - mBlackReserve)
      + mRules.levelWgt * mLevelLead + mRules.freeWgt * mFreeLead;
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

   // ****TODO: TEST THISSSSSSS
   // Update mLevelLead and mFreeLead
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
            if (cell->sups & mWhite) {
               mFreeLead += 1;
            } else if (cell->sups & mBlack) {
               mFreeLead -= 1;
            }
         }
      }
   }
}

void PylosBoard::TakeMarble(Spot *trg) {
   // [*Staley] Other stuff needed here, related to board valuation
   // [*Staley] This is a great place for a few asserts, too.
   // "Board valuation" updates is to update the values of 
   // mWhiteReserve, mBlackReserve, mLevelLead, and mFreeLead

   HalfTake(trg);

   // Make sure that there aren't any spots that have both a white
   // piece and a black piece in the same spot. Play it safe here.
   assert((mWhite & mBlack) == 0x0);

   // *Update mLevelLead and mFreeLead

}

void PylosBoard::ApplyMove(Move *move)
{
   PylosMove *tm = dynamic_cast<PylosMove *>(move);

   int rChange = -1;  // [Staley] Start by assuming we'll lose one from reserve
   PylosMove::LocVector::iterator itr = tm->mLocs.begin();

   /** BEGIN Girum's code to verify that a move is actually a valid move */
   list<Move *> moves;
   PylosBoard::GetAllMoves(&moves);
   bool listContainsMove = false;
   for (list<Move *>::const_iterator iter = moves.begin(); iter != moves.end(); iter++) {
      if ((*iter)->operator==(*move)) {
         listContainsMove = true;
      }
   }
   if (!listContainsMove) {
      cout << "LIST DOES NOT CONTAIN MOVE AS VALID, DID NOT DO MOVE" << endl;
      return;
   }
   /** END Girum's code */

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
   // [Staley] Fill in
   // [Ian] Basically, do ApplyMove() backwards (obviously)

   PylosMove *moveToUndo = dynamic_cast<PylosMove *>(mMoveHist.back());

   // Start by assuming that we'll the reserve will gain a new piece.
   int rChange = 1;
   PylosMove::LocVector::iterator itr = moveToUndo->mLocs.begin();

   // Switch whose move it is (early so that HalfTake() works properly)
   mWhoseMove = -mWhoseMove;

   cout << "Trying to take marble [" << (*itr).first << "," << (*itr).second << "]\n";
   TakeMarble(&mSpots[(*itr).first][(*itr).second]);

   itr++;
   for (; itr != moveToUndo->mLocs.end(); itr++) {
      PutMarble(&mSpots[(*itr).first][(*itr).second]);
      rChange--;
   }

   // Make changes to reserve counts
   if (mWhoseMove == kWhite)
      mWhiteReserve -= rChange;
   else
      mBlackReserve -= rChange;

   // Destroy history of the move
   mMoveHist.pop_back();
}

void PylosBoard::GetAllMoves(list<Move *> *uncastMoves) const
{
   int tRow, tCol, sRow, sCol;
   Cell *trg, *src;
   PylosMove::LocVector locs;
   list<PylosMove *>::iterator itr;
   list<PylosMove *> *castedMoves = reinterpret_cast<list<PylosMove *>*>(uncastMoves);
   ulong sideMask = mWhoseMove == kWhite ? mWhite : mBlack;

   castedMoves->clear();
   if (mWhiteReserve == 0 || mBlackReserve == 0)
      return;

   for (tRow = 0; tRow < kDim; tRow++)
      for (tCol = 0; tCol < kDim; tCol++) {
         trg = mSpots[tRow][tCol].empty;
         if (trg && (trg->subs & (mWhite|mBlack)) == trg->subs) { // [Staley] found a target spot
            locs.clear();
            locs.push_back(pair<int, int>(tRow, tCol));
            castedMoves->push_back(new PylosMove(locs, PylosMove::kReserve));

            for (sRow = 0; sRow < kDim; sRow++) // [Staley] search for promote moves
               for (sCol = 0; sCol < kDim; sCol++) {
                  src = mSpots[sRow][sCol].top;
                  if (src && (src->sups & (mWhite|mBlack)) == 0
                   && (src->mask & sideMask) && src->level < trg->level
                   && (sRow < tRow || sRow > tRow + 1    // [Staley] Not a support for trg
                   || sCol < tCol || sCol > tCol + 1)) {
                     locs.push_back(pair<int, int>(sRow, sCol));
                     castedMoves->push_back(new PylosMove(locs, PylosMove::kPromote));
                     locs.pop_back();
                  }
               }
         }
      }

      AddTakeBacks(castedMoves);
}

// [Staley] For each move in *moves that completes one or more sets, add all
// [Staley] combination of spots to take back.
void PylosBoard::AddTakeBacks(list<PylosMove *> *moves) const {
   // [Staley] You'll find HalfPut and HalfTake useful here.  You need to be able
   // [Staley] to temporarily put/take marbles in order to make this logic manageable,
   // [Staley] and you want it fast, so you don't want all the overhead of board
   // [Staley] value management, since you'll ultimately leave the board unchanged
   // [Staley] once the function is done.

   // Hints:
   // The reason you want to HalfPut and HalfTake here is so that you
   // can verify that the possibility of adding in a new Cell creates possible
   // "takeback moves."
   // The basic logic of verifying that a move creates an alignment is to simply
   // AND each of the Sets from sets[] of each cell against mWhite and mBlack.

   // Hints from myself:
   // Straightaway, just HalfPut() each possible move, since a possible move could
   // only be added into a square that's empty.  If any one of the set's alignments 
   // is then fulfilled by the new HalfPut()'d state of the board, then you've ID'd 
   // a move that "completes one or more sets."
   // From there, simply iterate through all the marbles (in the HalfPut()'d board)
   // that don't actually support any other marbles.
   // Don't forget to HalfTake() when you're done.

   // First, make a temporary copy of the original moves.
   // This is so that you don't add to the list as you loop through it.
   list<PylosMove *> movesCopy(*moves);

   for (list<PylosMove *>::const_iterator movesCopyIter = movesCopy.begin(); 
    movesCopyIter != movesCopy.end(); movesCopyIter++) {
      // Grab the cell that we're thinking about putting down
      PylosMove *potentialMove = *movesCopyIter;
      Spot *potentialMoveSpot = &mSpots[potentialMove->mLocs[0].first][potentialMove->mLocs[0].second];
      Cell *potentialMoveCell = potentialMoveSpot->empty;

      // Sanity check:  A possible move shouldn't be able to be applied to a filled Spot
      assert(potentialMoveCell != NULL);

      // Straightaway, put down the marble to inspect the new state of the board (to check
      // for possible alignments)
      HalfPut(potentialMoveSpot);

      // Find the iterator that points to where you want to add moves to.
      // WARNING:  THIS IS SLOW.  This one line of code causes AddTakeBacks to be O(N^2)
      list<PylosMove *>::const_iterator movesIter = std::find(moves->begin(), moves->end(), *movesCopyIter);

      if (mWhoseMove == kWhite)
         CalculateAllTakebacks(moves, movesIter, &mWhite, potentialMove, potentialMoveCell);
      else if (mWhoseMove == kBlack)
         CalculateAllTakebacks(moves, movesIter, &mBlack, potentialMove, potentialMoveCell);
      else assert(false);

      HalfTake(potentialMoveSpot);
   }
}

void PylosBoard::CalculateAllTakebacks(list<PylosMove *> *allMoves,
                                       list<PylosMove *>::const_iterator moveIter,
                                       Set *mSet, 
                                       PylosMove *potentialMove,
                                       Cell *potentialMoveCell) const {
                                          set<pair<short,short> > freeMarbles1;
                                          set<pair<short,short> > freeMarbles2;

   // For each of this cell's possible alignments,
   for (int i = 0; i < kSetsPerCell; i++) {
      // If putting down this cell creates a new alignment 
      // (checking against all possible alignments),
      if (potentialMoveCell->sets[i] &&
       (potentialMoveCell->sets[i] & *mSet) == potentialMoveCell->sets[i]) {

         // Then you've ID'd a move that "completes one or more sets."
         // From this point you should just go through and compile a list
         // of "freeMarbles" -- marbles that do not support other marbles.
         FindFreeMarbles(&freeMarbles1, mSet);

         // Once you've compiled a list of "free marbles," then add
         // all combinations of marbles you can take on this turn to
         // the list of Moves you can make
         for (set<pair<short,short> >::const_iterator freeMarbleIter1 = freeMarbles1.begin();
            freeMarbleIter1 != freeMarbles1.end(); freeMarbleIter1++) {

            // Grab the Spot that corresponds to the free marble that we're about to yank 
            Spot *freeMarble1 = &mSpots[(*freeMarbleIter1).first][(*freeMarbleIter1).second];
            assert(freeMarble1->top != NULL);

            // Construct the "takeback move" using the data from the potentialMove and
            // the location of freeMarble1
            PylosMove *takebackMove = new PylosMove(potentialMove->mLocs, potentialMove->mType);
            takebackMove->mLocs.push_back(*freeMarbleIter1);
            takebackMove->AssertMe();

            // Throw the new takebackMove into the list of all moves
            allMoves->insert(++moveIter, takebackMove);
            --moveIter;

            // Make sure to HalfTake() freeMarble1 to update state of the board, in preparation
            // for finding all possible freeMarble2's
            HalfTake(freeMarble1);

            // At this point, taking away freeMarble1 might have freed up additional
            // marbles.  Check those as well.
            FindFreeMarbles(&freeMarbles2, mSet, (*freeMarbleIter1).first, (*freeMarbleIter1).second);

            // For each of freeMarbles2, 
            for (set<pair<short,short> >::const_iterator freeMarbleIter2 = freeMarbles2.begin();
               freeMarbleIter2 != freeMarbles2.end(); freeMarbleIter2++) {
               // IT IS NOT TRUE THAT: assert(*freeMarbleIter1 != *freeMarbleIter2);
               // That is, it IS possible to take the same spot twice, since taking a spot
               // can potentially reveal another freeMarble with the same "spot number.

               // Construct the "takeback move" using freeMarble1 AND freeMarble2.
               // Ensure that they're ordered.
               takebackMove = new PylosMove(potentialMove->mLocs, potentialMove->mType);
               takebackMove->mLocs.push_back(*freeMarbleIter1);
               takebackMove->mLocs.push_back(*freeMarbleIter2);

               takebackMove->AssertMe();

               // Throw this one into the list of all moves.
               allMoves->insert(++moveIter, takebackMove);
               --moveIter;
            }

            // Don't forget to HalfPut() freeMarble1 back into the board for the next iteration
            HalfPut(freeMarble1);
            freeMarbles2.clear();
         }
      }

      freeMarbles1.clear();
   }
}

Board::Move *PylosBoard::CreateMove() const {
   return new PylosMove(PylosMove::LocVector(1), PylosMove::kReserve);;
}

Board *PylosBoard::Clone() const {
   // [Staley] Think carefully about this one.  You should be able to do it in just
   // [Staley] 5-10 lines.  Don't do needless work

   PylosBoard *boardCopy = new PylosBoard();
   *boardCopy = *this;

   // Ian:  Don't I need more work than this?  What I currently
   // have isn't the full 5-10 lines of code... how can I test what I'm missing in
   // the memberwise copy?  The debugger's values *appear* to have everything copied
   // over...

   // [Ian] Memberwise copies don't work for pointers... namely my MoveHistory.
   // [Ian] You need to go through and deep copy the moveHistory, or else you'll
   // have a shallow copy of the pointers and not the pointers' data.
   boardCopy->mMoveHist.clear();
   for (list<Move *>::const_iterator moveHistIter = mMoveHist.begin();
    moveHistIter != mMoveHist.end(); moveHistIter++) {
      PylosMove *castedMove = dynamic_cast<PylosMove *>(*moveHistIter);
      Move *moveCopy = new PylosMove(castedMove->mLocs, castedMove->mType);
      boardCopy->mMoveHist.push_back(moveCopy);
   }

   return boardCopy;
}

void PylosBoard::Delete() {
   // [Staley] As with Clone, think carefully and don't do needless work.

   // Ian:  Uh... inspiration?  How would I even start to go about doing this?
   // Do I "delete this"?

   // So Delete() is more of a "housekeeper" than an actual destructor.
   // Performs everything the destructor needs to do that the default destructor
   // doesn't.  Namely, freeing things that pointers point to, and clearing out
   // arrays/vectors that won't get destroyed by the default destructor do in fact
   // get cleared out (can't push to the back of a non-empty array). 
   // In this case, we're not guaranteed that the destructor will even be called,
   // so we need to clear things out ourselves.
   // Basically, since we're not sure that the destructor will be called, just null
   // everything out that's not static.

   InitializeMSpots();
   mWhite = mBlack = 0x0;
   mWhoseMove = kWhite;
   mWhiteReserve = mBlackReserve = 0;
   mLevelLead = mFreeLead = 0;
   mMoveHist.clear();
}

Board::Key *PylosBoard::GetKey() const {
   BasicKey<2> *rtn = new BasicKey<2>();

   rtn->vals[0] = (mWhoseMove == kWhite) << kNumCells | mWhite;
   rtn->vals[1] = mBlack;

   return rtn;
}


istream &PylosBoard::Read(istream &is) {
   // [*Staley] Fill in (conform to Write() method below)
   // Ian:  Same here... inspiration on how to understand
   // the Write() method to make Read() conform to it

   // Assigned reading for this: http://cplusplus.com/doc/tutorial/files/
   // Read() is mostly Write() backwards, with a few exceptions.
   // Order's important.

   return is;
}

// [Staley] Don't change this.  Make Read conform to it.
ostream &PylosBoard::Write(ostream &os) const {
   Rules rules = mRules;
   list<Move *>::const_iterator itr;
   int mvCount = EndianXfer((int)mMoveHist.size());

   rules.EndSwap();
   os.write((char *)&rules, sizeof(rules));

   os.write((char *)&mvCount, sizeof(mvCount));
   for (itr = mMoveHist.begin(); itr != mMoveHist.end(); itr++)
      os << **itr;

   return os;
}

void *PylosBoard::GetOptions() {
   return new Rules(mRules);
}

void PylosBoard::SetOptions(const void *opts) {
   mRules = *reinterpret_cast<const Rules *>(opts);
}

//TODO Implement reflection
const Class *PylosBoard::GetClass() const {
   return 0;
}
