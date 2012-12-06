#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

#include <climits>
#include <assert.h>
#include "Board.h"
#include "MyLib.h"

class CheckersMove;

class CheckersBoard : public Board {
public:
   static const int kWidth = 8;

   friend class CheckersMove;

   enum { kWhite = -1, kBlack = 1, kKing = 7 };
   // kHeight is equal to vertical height of the board, measured in Cells.
   // kDim is equal to kHeight / 2 -- the number of actual Cells you can fit
   // vertically in the board
   enum { kNumCells = 32, kDim = 4 };

   // Running Clint's version of this says the following:
   // "(Piece weight is always 100)"
   static const int pieceWgt = 100;
   struct Rules {
      int kingWgt; // Weight of each King
      int backRowWgt; // Weight of any back row pieces
      int moveWgt; // Weight of it being your turn

      Rules() : kingWgt(300), backRowWgt(100), moveWgt(20) {}

      void EndSwap();
   };

   CheckersBoard();
   ~CheckersBoard() { Delete(); }

   long GetValue() const;
   void ApplyMove(Move *);
   void UndoLastMove();
   void GetAllMoves(std::list<Move *> *) const;
   Move *CreateMove() const;
   int GetWhoseMove() const {return mWhoseMove;}
   const std::list<const Move *> &GetMoveHist() const 
    {return *(std::list<const Move *> *)&mMoveHist;}

   Board *Clone() const;
   Key *GetKey() const;

   // [Staley] May add a public method for use by CheckersView.
   // Public helper function that returns true if a cell is occupied
   // by a certain color.
   //
   // "byWhom" expects either kWhite or kBlack.
   bool CellOccupied(int row, int col, int byWhom) const {
      assert(GetCell(row, col) != NULL);
      
      Set mask = GetCell(row, col)->mask;

      if (byWhom == kWhite) {
         return ((mask & this->mWhiteSet) != 0);
      } else if (byWhom == kBlack) {
         return ((mask & this->mBlackSet) != 0);
      } else {
         assert(false);
         return false;
      }
   }

   bool CellContainsKing(int row, int col) const {
      Set mask = GetCell(row,col)->mask;

      return ((mask & this->mKingSet) != 0);
   }

   // Option accessor/mutator.  GetOptions returns dynamically allocated
   // object representing options. SetOptions takes similar object.  Caller
   // owns object in both cases.
   static void *GetOptions();
   static void SetOptions(const void *opts);

   // TODO: Arrange for this to be called at static initialization time, to set up
   // the static mWhiteBackRow and mBlackBackRow
   static void StaticInit();

   const Class *GetClass() const { return &mClass; };

protected:
   
   
   enum { kStartingPieces = 12, kStartingBackPieces = 4, kSqr = 4 };
   enum { kSW = 0, kSE = 1, kNW = 2, kNE = 3 };

   typedef ulong Set;

   // Struct that defines static cells of the CheckersBoard.  Don't change this.
   struct Cell {
      Set mask; // Mask with this cell's bit turned on

      Cell *neighborCells[kSqr];

      // std::string name;  // Formatted name of this Cell
      std::pair<char, unsigned int> loc;
   };

   // Struct that defines non-static Checkers pieces being played on top
   // of cells.
   struct Piece {
      bool isKing;
      int color;
      std::pair<char, unsigned int> loc;

      Piece(bool isKing, int color, std::pair<char, unsigned int> loc) : 
         isKing(isKing), color(color), loc(loc) {}
   };

   std::istream &Read(std::istream &);
   std::ostream &Write(std::ostream &) const;

   // Frees all CheckersBoard storage.
   void Delete();
   
   // Helper function to add a piece on the board.
   // ApplyMove() and UndoLastMove() should use Put() instead of this method.
   inline void HalfPut(Piece *piece, Cell *cell) const {
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
   inline Piece *HalfTake(Cell *cell, int color) const {     
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

   // Put a piece from one cell to another
   // TODO: Should I put an assert here that ensures that the location
   // of the piece is the same as the targetCell?
   inline void Put(Piece *piece, Cell *targetCell) {
      HalfPut(piece, targetCell);
   }

   // Take away a piece of a certain color from a cell.
   inline Piece *Take(Cell *cell, int color) {
      Piece *removedPiece = HalfTake(cell, color);
      return removedPiece;
   }

   void RefreshBoardValuation() {
      // Clear out existing values
      mBlackPieceCount = mWhitePieceCount = 0;
      mBlackKingCount = mWhiteKingCount = 0;
      mBlackBackCount = mWhiteBackCount = 0;

      // Count up the values for each type
      for (char row = 'A'; row <= 'H'; row ++) {
         for (unsigned int col = ((row-'A')%2) + 1; col <= kWidth; col += 2) {
            Cell *cell = GetCell(row, col);
            assert((mWhiteSet & mBlackSet) == 0);

            // If this cell is currently occupied by a Black piece...
            if (cell->mask & mBlackSet) {
               if (cell->mask & mKingSet)
                  ++mBlackKingCount;
               else
                  ++mBlackPieceCount;

               if (row == 'A')
                  ++mBlackBackCount;
            } 
            // If this cell is currently occupied by a White piece...
            else if (cell->mask & mWhiteSet) {
               if (cell->mask & mKingSet) 
                  ++mWhiteKingCount;
               else
                  ++mWhitePieceCount;

               if (row == 'H')
                  ++mWhiteBackCount;
            }
         }
      }
   }

   void MultipleJumpDFS(std::list<CheckersMove *> *, 
    std::vector<std::pair<char, unsigned int> >, Cell *) const;

   // TODO: Refactor this to use a Piece instead of a Cell.
   inline bool CanMove(Cell *cell, int direction) const {
      // Validate that this piece can move in the direction that you
      // want to move in.
      if (!IsValidDirection(cell, direction))
         return false;

      // Ensure that the piece that you want to move towards is inbounds
      // and isn't already occupied.
      if (!cell->neighborCells[direction]
       || (cell->neighborCells[direction]->mask & (mBlackSet|mWhiteSet)) != 0) {
         return false;
      }

      return true;
   }

   // TODO: Refactor this to use a Piece instead of a Cell.
   inline bool CanJump(Cell *cell, int dir) const {
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
   inline bool IsValidDirection(Cell *cell, int direction) const {
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

   // Quick helper functions for GetCell()
   static inline bool IsEven(char num) { return num % 2; }
   static inline bool IsOdd(char num) { return !IsEven(num); }

   // Returns NULL if parameters are invalid
   static inline Cell *GetCell(char row, unsigned col) {
      // Out of bounds
      if (row < 'A' || row > 'H' || col < 1 || col > 8) {
         return NULL;
      }

      // Reduce the inputted characters to their numerical form
      // (I drew up a diagram of this in my binder)
      row -= 'A';
      col -= 1;
      
      // Invalid square.  Row and Col must either both be odd or both be even 
      // to be valid.
      if (!((IsEven(row) && IsEven(col)) || (IsOdd(row) && IsOdd(col)))) {

         return NULL;
      }

      return mCells + (row*kDim) + (col/2);
   }

   static inline Cell *GetCell(const std::pair<char, unsigned int> &loc) {
      return GetCell(loc.first, loc.second);
   }

   // Static member datum goes here
   struct CheckersBoardInitializer {
      CheckersBoardInitializer() {
         CheckersBoard::StaticInit();
      }
   };
   static CheckersBoardInitializer mInitializer;

   static Rules mRules; // The static rules object for CheckersBoard

   static Cell mCells[kNumCells]; // One Cell for each cell
   static Set mBlackBackSet; // Static bitmask of the cells of Black's back row
   static Set mWhiteBackSet; // Static bitmask of the cells of White's back row

   // Bitmasks indicating which cells contain white pieces, black pieces,
   // and Kings.  No-marble cells are 0 in both masks.  Bits are assigned to
   // cells in alphanumeric order, from LSB to MSB.  All 32 bits are used.
   mutable Set mBlackSet;
   mutable Set mWhiteSet;
   mutable Set mKingSet;

   int mWhoseMove; // Whose move it is.  Can be kBlack or kWhite.

   int mBlackPieceCount; // The count of pieces Black has on the board
   int mBlackKingCount; // The count of kings Black has on the board
   int mBlackBackCount; // The count of pieces Black has in his back row

   int mWhitePieceCount; // The count of pieces White has on the board
   int mWhiteKingCount; // The count of kings White has on the board
   int mWhiteBackCount;// The count of pieces White has in his back row

   std::list<Move *> mMoveHist; // History of moves thus far.
   std::list<Piece *> mCapturedPieces; // Stack of pieces captured.
   
private:
   static Object *CreateCheckersBoard() { return new CheckersBoard; };
   static BoardClass mClass;

};


#endif 
