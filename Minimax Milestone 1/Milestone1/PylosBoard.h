#ifndef PYLOSBOARD_H
#define PYLOSBOARD_H

#include <iostream>
#include <assert.h>
#include <set>
#include <vector>
#include <string.h>
#include "MyLib.h"
#include "Board.h"

class PylosMove;

class PylosBoard : public Board {
public:
   const static int kDim = 4;

   friend class PylosMove;

   struct Rules {
      int levelWgt;  // [Staley] Additional weight for each level higher per stone
      int marbleWgt; // [Staley] Weight of each marble in reserve
      int freeWgt;   // [Staley] Weight for each uncovered marble
      
      Rules() : levelWgt(20), freeWgt(6), marbleWgt(100) {}
      
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
   
   // [*Staley] Add a method to prevent PylosBoard from being an abstract class.
   // [*Staley] May add a public method for use by PylosView
   // [*Staley] Add a static method to support the Class system, plus a static
   // [*Staley] private member datum below

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
      ulong mask;    // [Staley] Mask with this cell's bit turned on
      ulong subs;    // [Staley] Mask having 1-bits for the cells supporting this one
      ulong sups;    // [Staley] Mask having 1-bits for the cells supported by this one
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
   static inline ulong GetMask(int row, int col, int lvl) {
      return InBounds(row, col, lvl) ? GetCell(row, col, lvl)->mask : 0;
   }
   
   // [Staley] Adjust "spot" to reflect putting a marble on its top, and adjust the mWhite
   // [Staley] and mBlack masks, but do not update state relative to board valuation.  
   // [Staley] Used to "test out" a marble placement at low cost.
   inline void HalfPut(Spot *spot) const {
      assert(spot != NULL);

      spot->top = spot->empty;
      spot->empty = spot->top->above;
      
      if (mWhoseMove == kWhite)
         mWhite |= spot->top->mask;
      else if (mWhoseMove == kBlack)
         mBlack |= spot->top->mask;
      else
         assert(false);
      
      // [Staley] Here write a verifier that all the spots are correct
      // Do the "IAmSane()" function that's all asserts here
   }
   
   // [Staley] Like HalfPut, but in reverse
   inline void HalfTake(Spot *spot) const {
      assert(spot != NULL && spot->top != NULL);

      // Clear out the corresponding bits
      if (mWhoseMove == kWhite)
         mWhite &= ~(spot->top->mask);
      else if (mWhoseMove == kBlack)
         mBlack &= ~(spot->top->mask);
      else
         assert(false);
      
      // [Staley] Fill in
      // Change over the Spots
      spot->empty = spot->top;
      spot->top = spot->empty->below[kNW];
   }

   // [Staley] Add possible nested class and member datum to force StaticInit call.
   struct PylosBoardInitializer {
      PylosBoardInitializer() {
         StaticInit();
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
   mutable ulong mWhite;
   mutable ulong mBlack;
   
   int mWhoseMove;    // [Staley] Whose move currently
   int mWhiteReserve; // [Staley] How many marbles has white in his reserve
   int mBlackReserve; // [Staley] How many marbles has black in his 
   int mLevelLead;    // [Staley] Amount by which white leads in terms of marble level
   int mFreeLead;     // [Staley] Amount of promoteable marbles white has over black.

   // [Staley] History of moves leading to this point.
   std::list<Move *> mMoveHist;

   // TODO: PylosBoard cannot friend PylosView
   friend class PylosView;
   
   // Mutual friendship between PylosBoard and PylosMove is allowed.
   friend class PylosBoard;
   
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
    std::list<PylosMove *>::const_iterator moveIter,
    Set *mSet, PylosMove *potentialMove, Cell *potentialMoveCell) const;

   // WARNING: Preventing duplicates is hard.  Right now, I'm preventing
   // them by simply only calling FindFreeMarbles on rows/cols that are PAST
   // the current free marble.  If there's a Bender error, here's where
   // you can start.
   void FindFreeMarbles(std::set<std::pair<short,short> > *freeMarbles, 
    ulong *playerMarbles, unsigned short startRow = 0, unsigned short startCol = 0) const {
      // Quick sanity check
      assert(freeMarbles && freeMarbles->size() == 0);

      for (int row = startRow; row < kDim; row++) {
         for (int col = startCol; col < kDim; col++) {
            // A marble is "free" if it does not support any other marbles.
            // Bitwise, this means that all of the possible marbles it can 
            // sup[port] are NOT present in the current board -- black OR white.
            // Also, the freeMarble to take back must belong to that player.
            if (mSpots[row][col].top
             && mSpots[row][col].top->mask & *playerMarbles
             && (mSpots[row][col].top->sups & (mWhite|mBlack)) == 0) {
               freeMarbles->insert(std::pair<int, int>(row,col)).second;
            }
         }
      }
   }

   void ClearMSpots();
   void UpdateBoardValuation();

   static BoardClass mClass;
   static Object *CreatePylosBoard() { return new PylosBoard; };
};


#endif

