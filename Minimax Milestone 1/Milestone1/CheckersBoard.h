#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

#include <assert.h>
#include "Board.h"
#include "MyLib.h"

class CheckersMove;

class CheckersBoard : public Board {
public:
   static const int kWidth = 8;

   friend class CheckersMove;

   // Game logic relies on kWhite being -1 and kBlack being 1
   // kHeight is equal to vertical height of the board, measured in Cells.
   // kDim is equal to kHeight / 2 -- the number of actual Cells you can fit
   // vertically in the board
   enum { kWhite = -1, kBlack = 1, kKing = 7, kNumCells = 32, kDim = 4 };

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

   
   bool CellOccupied(int row, int col, int byWhom) const;
   bool CellContainsKing(int row, int col) const;

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

   void RefreshBoardValuation();

   void MultipleJumpDFS(std::list<CheckersMove *> *, 
    std::vector<std::pair<char, unsigned int> >, Cell *) const;

   inline bool CanMove(Cell *cell, int direction) const;
   inline bool CanJump(Cell *cell, int dir) const;
   inline bool IsValidDirection(Cell *cell, int direction) const;
   
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

      row = kWidth - row - 1;
      
      // Invalid square.  Row and Col must either be odd/even or even/odd,
      // respectively.
      if (!((IsEven(row) && IsOdd(col)) || (IsOdd(row) && IsEven(col)))) {
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
   mutable Set mBlackSet, mWhiteSet, mKingSet;

   int mWhoseMove; // Whose move it is.  Can be kBlack or kWhite.

   // Piece counts
   int mBlackPieceCount, mBlackKingCount, mBlackBackCount, mWhitePieceCount,
    mWhiteKingCount, mWhiteBackCount; 

   std::list<Move *> mMoveHist; // History of moves thus far.
   std::list<Piece *> mCapturedPieces; // Stack of pieces captured.
   
   static Object *CreateCheckersBoard() { return new CheckersBoard; };
   static BoardClass mClass;

};


#endif 