#ifndef PYLOSBOARD_H
#define PYLOSBOARD_H

#include <iostream>
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
      int levelWgt;  // Additional weight for each level higher per stone
      int marbleWgt; // Weight of each marble in reserve
      int freeWgt;   // Weight for each uncovered marble
      
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
   
   // Add a method to prevent PylosBoard from being an abstract class.
   // May add a public method for use by PylosView
   // Add a static method to support the Class system, plus a static
   // private member datum below

   // Option accessor/mutator.  GetOptions returns dynamically allocated
   // object representing options. SetOptions takes similar object.  Caller
   // owns object in both cases.
   static void *GetOptions();
   static void SetOptions(const void *opts);

   // Arrange for this to be called at static initialization time, to set up
   // the static mCells and mSets.  See below
   static void StaticInit();
   
   const Class *GetClass() const;

protected:
   enum {kBitsPerCell = 2, kCellMask = 0x3, kBlack = -1, kWhite = 1};
   enum {kNumCells = 30, kSetsPerCell = 6, kNumSets = 28, kStones = 15};
   enum {kNW = 0, kNE = 1, kSE = 2, kSW = 3, kSqr = 4};
            
   typedef ulong Set;
   
   struct Cell {
      int level;     // Level the cell is on, numbering from 0 (bottom)
      ulong mask;    // Mask with this cell's bit turned on
      ulong subs;    // Mask having 1-bits for the cells supporting this one
      ulong sups;    // Mask having 1-bits for the cells supported by this one
      int setCount;  // Number of alignments this cell might be part of
      Set sets[kSetsPerCell];  // Masks for the cells in each alignment
      Cell *below[kSqr];       // Direct pointers to the 4 supporting cells
      Cell *above;             // Direct pointer to the supported cell
      
      Cell() : level(0), mask(0), subs(0), sups(0), setCount(0),
       above(0) {
         memset((void *)sets, 0, sizeof(sets));
         memset((void *)below, 0, sizeof(below));
      }
      
      void addSet(Set set) {sets[setCount++] = set;}
   };
    
   // Describes the situation at one row/col "spot", which is a column of
   // cells having the same row/col value within their level (visualize a Spot
   // as a diagonal column extending upward and to the SE within the Pylos 
   // pile, and having between 1 (for spots like 3,3) to 4 (for the 0, 0 spot)
   // layers.  A Spot has a top filled cell (possibly null if no cells are 
   // filled) and possibly an empty cell above that if there's still room.
   struct Spot {
      Cell *empty;    // Next empty cell, or null
      Cell *top;      // Top filled cell, or null
      
      Spot(): empty(0), top(0) {}
   };
   
   std::istream &Read(std::istream &);
   std::ostream &Write(std::ostream &) const;

   // Put (or take back) a marble to (or from) the top of the Spot given
   void PutMarble(Spot *);
   void TakeMarble(Spot *);

   // Augment a list of PylosMoves by adding the various marble takeback
   // possibilities for moves that complet an alignment.
   void AddTakeBacks(std::list<PylosMove *> *) const;

   // Free all PylosBoard storage
   void Delete();
      
   // Is row, col in bounds assuming we are on level "lvl"?
   static inline bool InBounds(int row, int col, int lvl = 0) {
      return InRange<int>(0, row, kDim - lvl) && InRange<int>(0, col, kDim - lvl);
   }

   // Return the Cell, within mCells, corresponding to row, col, lvl.  Note 
   // that row sizes on "lvl" are (kDim-lvl)
   static inline Cell *GetCell(int row, int col, int lvl)
   {
      return mCells + mOffs[lvl] + (kDim - lvl)*row + col;
   }
   
   // Return a bitmask with a 1-bit for (row, col, lvl), or 0 if out of bounds.
   static inline ulong GetMask(int row, int col, int lvl) {
      return InBounds(row, col, lvl) ? GetCell(row, col, lvl)->mask : 0;
   }
   
   // Adjust "spt" to reflect putting a marble on its top, and adjust the mWhite
   // and mBlack masks, but do not update state relative to board valuation.  
   // Used to "test out" a marble placement at low cost.
   inline void HalfPut(Spot *spt) const {
      spt->top = spt->empty;
      spt->empty = spt->top->above;
      
      if (mWhoseMove == kWhite)
         mWhite |= spt->top->mask;
      else
         mBlack |= spt->top->mask;
   }
   
   // Like HalfPut, but in reverse
   inline void HalfTake(Spot *spt) const {
      // Fill in
   }
   
   // Add possible nested class and member datum to force StaticInit call.

   // Rules object for PylosBoard
   static Rules mRules;
   
   // Array of Sets holding bitmaps for each alignment.  The first 14 
   // alignments are the squares, in level-major, row-submajor order
   // Next 8 are the horizontal followed by the vertical level-0 rows,
   // and the last 6 are the horizontal followed by the vertical level 
   // 1 rows.
   static Set  mSets[kNumSets];
   
   // One Cell object for each cell, in 
   static Cell mCells[kNumCells];

   // Offsets within mCells at which each level starts
   static int mOffs[kDim];
   
   // Array of Spots, one for each row/col combination
   mutable Spot mSpots[kDim][kDim];

   // Bitmasks indicating which cells contain a white marble, and which
   // a black marble.  No-marble cells are 0 in both masks.  Bits are assigned
   // to cells in level-major, row submajor order, from LSB to MSB.  Top
   // two bits are thus unused
   mutable ulong mWhite;
   mutable ulong mBlack;
   
   int mWhoseMove;    // Whose move currently
   int mWhiteReserve; // How many marbles has white in his reserve
   int mBlackReserve; // How many marbles has black in his reserve
   int mLevelLead;    // Amount by which white leads in terms of marble level
   int mFreeLead;     // Amount of promoteable marbles white has over black.

   // History of moves leading to this point.
   std::list<Move *> mMoveHist;  
};

#endif

