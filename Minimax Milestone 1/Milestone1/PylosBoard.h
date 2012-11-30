#ifndef PYLOSBOARD_H
#define PYLOSBOARD_H

#include <iostream>
#include <assert.h>
#include <set>
#include <vector>
#include <string.h>
#include "MyLib.h"
#include "Board.h"
// Cannot #include "PylosBoard.h" as per the spec.

class PylosMove;

class PylosBoard : public Board {
public:
   const static int kDim = 4;

   friend class PylosMove;

   struct Rules {
      int levelWgt;  // [Staley] Additional weight for each level higher per stone
      int marbleWgt; // [Staley] Weight of each marble in reserve
      int freeWgt;   // [Staley] Weight for each uncovered marble
      
      Rules() : levelWgt(20), marbleWgt(100), freeWgt(6) {}
      
      int GetLevel()  {return levelWgt;}
      int GetMarble() {return marbleWgt;}
      int GetFree()   {return freeWgt;}
      
      void SetLevel(int val);
      void SetMarble(int val);
      void SetFree(int val);

      void EndSwap();
   };

   PylosBoard();
   ~PylosBoard() {Delete();}

   long GetValue() const;
   void ApplyMove(Move *);
   void UndoLastMove();
   void GetAllMoves(std::list<Move *> *) const;
   Move *CreateMove() const;
   int GetWhoseMove() const   {return mWhoseMove != kWhite;}
   
   const std::list<const Move *> &GetMoveHist() const 
    {return *(std::list<const Move *> *)&mMoveHist;}

   Board *Clone() const;
   Key *GetKey() const;
   
   // [Staley] May add a public method for use by PylosView.
   // Public helper function that returns true if a cell is occupied
   // by a certain color.
   //
   // "byWhom" expects either kWhite or kBlack.
   bool CellOccupied(int row, int col, int level, int byWhom) const {
      Set mask = this->GetCell(row, col, level)->mask;

      if (byWhom == kWhite) {
         return mask & this->mWhite;
      } else if (byWhom == kBlack) {
         return mask & this->mBlack;
      } else {
         assert(false);
         return false;
      }
   }

   // [Staley] Add a static method to support the Class system, plus a static
   // [Staley] private member datum below

   // [Staley] Option accessor/mutator.  GetOptions returns dynamically allocated
   // [Staley] object representing options. SetOptions takes similar object.  Caller
   // [Staley] owns object in both cases.
   static void *GetOptions();
   static void SetOptions(const void *opts);

   // [Staley] Arrange for this to be called at static initialization time, to set up
   // [Staley] the static mCells and mSets.  See below
   static void StaticInit();
   
   const Class *GetClass() const { return &mClass; };

protected:
   enum {kBitsPerCell = 2, kCellMask = 0x3, kBlack = -1, kWhite = 1};
   enum {kNumCells = 30, kSetsPerCell = 6, kNumSets = 28, kStones = 15};
   enum {kNW = 0, kNE = 1, kSE = 2, kSW = 3, kSqr = 4};
            
   typedef ulong Set;
   
   struct Cell {
      int level;     // [Staley] Level the cell is on, numbering from 0 (bottom)
      Set mask;    // [Staley] Mask with this cell's bit turned on
      Set subs;    // [Staley] Mask having 1-bits for the cells supporting this one
      Set sups;    // [Staley] Mask having 1-bits for the cells supported by this one
      int setCount;  // [Staley] Number of alignments this cell might be part of
      Set sets[kSetsPerCell];  // [Staley] Masks for the cells in each alignment
      Cell *below[kSqr];       // [Staley] Direct pointers to the 4 supporting cells
      Cell *above;             // [Staley] Direct pointer to the supported cell
      
      Cell() : level(0), mask(0), subs(0), sups(0), setCount(0),
       above(0) {
         memset((void *)sets, 0, sizeof(sets));
         memset((void *)below, 0, sizeof(below));
      }
      
      void addSet(Set set) {sets[setCount++] = set;}
   };
    
   // [Staley] Describes the situation at one row/col "spot", which is a column of
   // [Staley] cells having the same row/col value within their level (visualize a Spot
   // [Staley] as a diagonal column extending upward and to the SE within the Pylos 
   // [Staley] pile, and having between 1 (for spots like 3,3) to 4 (for the 0, 0 spot)
   // [Staley] layers.  A Spot has a top filled cell (possibly null if no cells are 
   // [Staley] filled) and possibly an empty cell above that if there's still room.
   struct Spot {
      Cell *empty;    // [Staley] Next empty cell, or null
      Cell *top;      // [Staley] Top filled cell, or null
      
      Spot(): empty(0), top(0) {}
   };
   
   std::istream &Read(std::istream &);
   std::ostream &Write(std::ostream &) const;

   // [Staley] Put (or take back) a marble to (or from) the top of the Spot given
   void PutMarble(Spot *);
   void TakeMarble(Spot *);

   // [Staley] Augment a list of PylosMoves by adding the various marble takeback
   // [Staley] possibilities for moves that complet an alignment.
   void AddTakeBacks(std::list<PylosMove *> *) const;

   // [Staley] Free all PylosBoard storage
   void Delete();
      
   // [Staley] Is row, col in bounds assuming we are on level "lvl"?
   static inline bool InBounds(int row, int col, int lvl = 0) {
      return InRange<int>(0, row, kDim - lvl) && InRange<int>(0, col, kDim - lvl);
   }

   // [Staley] Return the Cell, within mCells, corresponding to row, col, lvl.  Note 
   // [Staley] that row sizes on "lvl" are (kDim-lvl)
   static inline Cell *GetCell(int row, int col, int lvl) {
      return mCells + mOffs[lvl] + (kDim - lvl)*row + col;
   }
   
   // [Staley] Return a bitmask with a 1-bit for (row, col, lvl), or 0 if out of bounds.
   static inline Set GetMask(int row, int col, int lvl) {
      return InBounds(row, col, lvl) ? GetCell(row, col, lvl)->mask : 0;
   }
   
   // [Staley] Adjust "spot" to reflect putting a marble on its top, and adjust the mWhite
   // [Staley] and mBlack masks, but do not update state relative to board valuation.  
   // [Staley] Used to "test out" a marble placement at low cost.
   inline void HalfPut(Spot *spot) const {
      // Ensure that you're not trying to HalfPut() a completely filled Spot
      assert(spot != NULL && spot->empty != NULL);

      spot->top = spot->empty;
      spot->empty = spot->top->above;
      
      if (mWhoseMove == kWhite)
         mWhite |= spot->top->mask;
      else if (mWhoseMove == kBlack)
         mBlack |= spot->top->mask;
      else assert(false);
      
      // WARNING: Here write a verifier that all the spots are correct.  That is,
      // do the "IAmSane()" function for Spot correction in the board's state.
   }
   
   // [Staley] Like HalfPut, but in reverse
   inline void HalfTake(Spot *spot) const {
      // Ensure that you're not trying to HalfTake() an empty Spot
      assert(spot != NULL && spot->top != NULL);

      // Clear out the spot->top bit.
      if (mWhoseMove == kWhite)
         mWhite &= ~(spot->top->mask);
      else if (mWhoseMove == kBlack)
         mBlack &= ~(spot->top->mask);
      else assert(false);
      
      // [Staley] Fill in
      // Reset the spot so that it reflects the removed piece.
      
      spot->empty = spot->top;
      spot->top = spot->top->below[kNW];
   }

   // [Staley] Add possible nested class and member datum to force StaticInit call.
   struct PylosBoardInitializer {
      PylosBoardInitializer() {
         PylosBoard::StaticInit();
      }
   };
   static PylosBoardInitializer mInitializer;


   // [Staley] Rules object for PylosBoard
   static Rules mRules;
   
   // [Staley] Array of Sets holding bitmaps for each alignment.  The first 14 
   // [Staley] alignments are the squares, in level-major, row-submajor order
   // [Staley] Next 8 are the horizontal followed by the vertical level-0 rows,
   // [Staley] and the last 6 are the horizontal followed by the vertical level 
   // [Staley] 1 rows.
   static Set  mSets[kNumSets];
   
   // [Staley] One Cell object for each cell, in 
   static Cell mCells[kNumCells];

   // [Staley] Offsets within mCells at which each level starts
   static int mOffs[kDim];
   
   // [Staley] Array of Spots, one for each row/col combination
   mutable Spot mSpots[kDim][kDim];

   // [Staley] Bitmasks indicating which cells contain a white marble, and which
   // [Staley] a black marble.  No-marble cells are 0 in both masks.  Bits are assigned
   // [Staley] to cells in level-major, row submajor order, from LSB to MSB.  Top
   // [Staley] two bits are thus unused
   mutable Set mWhite;
   mutable Set mBlack;
   
   int mWhoseMove;    // [Staley] Whose move currently
   int mWhiteReserve; // [Staley] How many marbles has white in his reserve
   int mBlackReserve; // [Staley] How many marbles has black in his 
   int mLevelLead;    // [Staley] Amount by which white leads in terms of marble level
   int mFreeLead;     // [Staley] Amount of promoteable marbles white has over black.

   // [Staley] History of moves leading to this point.
   std::list<Move *> mMoveHist;
   
private:
   // Stolen helper function from StackOverflow, used to assert() 
   // alignment bits being set correctly
   static int NumberOfSetBits(int i) {
      i = i - ((i >> 1) & 0x55555555);
      i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
      return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
   }

   // My own helper function for AddTakebacks().
   void CalculateAllTakebacks(std::list<PylosMove *> *moves, 
    std::list<PylosMove *>::iterator moveIter,
    Set *mSet, PylosMove *potentialMove, Cell *potentialMoveCell) const;

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
   void FindFreeMarbles(std::set<std::pair<short,short> > *freeMarbles, 
    Set *playerMarbles, unsigned short startRow = 0, unsigned short startCol = 0) const {
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
   void InsertIfFree(std::set<std::pair<short,short> > *freeMarbles,
    Set *playerMarbles, int row, int col) const {
       Cell *marble = mSpots[row][col].top;

       if (marble && (marble->mask & *playerMarbles)
             && (marble->sups & (mWhite|mBlack)) == 0) {
         freeMarbles->insert(std::pair<short,short>(row,col));
      }
   }


   void ClearMSpots();
   void UpdateBoardValuation();

   static BoardClass mClass;
   static Object *CreatePylosBoard() { return new PylosBoard; };
};


#endif

