#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

#include <climits>
#include <assert.h>
#include "Board.h"
#include "MyLib.h"

#define kNull -1

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
   };

   CheckersBoard();
   ~CheckersBoard();

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

   // [Staley] May add a public method for use by PylosView.
   // Public helper function that returns true if a cell is occupied
   // by a certain color.
   //
   // "byWhom" expects either kWhite or kBlack.
   bool CellOccupied(int row, int col, int byWhom) const {
      Set mask = this->GetCell(row, col)->mask;

      if (byWhom == kWhite) {
         return mask & this->mWhiteSet;
      } else if (byWhom == kBlack) {
         return mask & this->mBlackSet;
      } else {
         assert(false);
         return false;
      }
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
   
   
   enum { kStartingPieces = 12, kStartingBackPieces = 4 };

   typedef ulong Set;

   struct Cell {
      Set mask; // Mask with this cell's bit turned on

      // Pointer to the cell to the top left of this one, or NULL
      Cell *topLeft; 

      // Pointer to the cell to the top right of this one, or NULL
      Cell *topRight; 
      
      // Pointer to the cell to the bottom left of this one, or NULL
      Cell *bottomLeft;
      
      // Pointer to the cell to the bottom right of this one, or NULL
      Cell *bottomRight;
   };

   std::istream &Read(std::istream &);
   std::ostream &Write(std::ostream &) const;
   
   // Helper function to add a piece
   inline void HalfPut(Cell *cell, int color) {
      if (color == kBlack) {
         mBlackSet |= cell->mask;
      } else if (color == kWhite) {
         mWhiteSet |= cell->mask;
      } else assert(false);

      assert((mBlackSet & mWhiteSet) == 0);
   }

   // Helper function to remove a piece.
   inline void HalfTake(Cell *cell, int color) {
      if (color == kBlack) {
         mBlackSet &= ~(cell->mask);
      } else if (color == kWhite) {
         mWhiteSet &= ~(cell->mask);
      } else assert(false);

      assert((mBlackSet & mWhiteSet) == 0);
   }

   // Quick helper functions for GetCell()
   static inline bool IsEven(char num) { return num % 2; }
   static inline bool IsOdd(char num) { return !IsEven(num); }

// // Quick test of GetCell()
//    //
//    // Test out of bounds assertion.
//    //    GetCell(H9) -- number too big
//    //    GetCell(G0) -- number too small
//    // Need 100% coverage on invalid square assertion
//    //   GetCell(A6) -- even row, odd col
//    //   GetCell(B5) -- odd row, even col
//    // 
//    // GetCell(A5) should return 2
//    // GetCell(H8) should return 31
//    // GetCell(A1) should return 0
//    // GetCell(F6) should return 22
// 
//    // GetCell('H', 9);  this should assert()
//    // GetCell('G', 0);  this should assert()
//    // GetCell('A', 6);  this should assert()
//    // GetCell('B', 5);  this should assert()
//    GetCell('A', 5);
//    GetCell('H', 8);
//    GetCell('A', 1);
//    GetCell('F', 6);
   // 
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

//       int offset = (row*kDim) + (col/2);
//       std::cout << "GetCell() returns offset: " << offset << std::endl << std::endl;

      return mCells + (row*kDim) + (col/2);
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
   static Set mBlackBackRow; // Static bitmask of the cells of Black's back row
   static Set mWhiteBackRow; // Static bitmask of the cells of White's back row

   // Bitmasks indicating which cells contain white pieces, black pieces,
   // and Kings.  No-marble cells are 0 in both masks.  Bits are assigned to
   // cells in alphanumeric order, from LSB to MSB.  All 32 bits are used.
   mutable Set mBlackSet;
   mutable Set mWhiteSet;
   mutable Set mKingSet;

   int mWhoseMove; // Whose move it is.  Can be kBlack or kWhite.

   int mBlackCount; // The count of pieces Black has on the board
   int mBlackBackCount; // The count of pieces Black has in his back row

   int mWhiteCount; // The count of pieces White has on the board
   int mWhiteBackCount;// The count of pieces White has in his back row


   std::list<Move *> mMoveHist; // History of moves thus far.
   
private:
   static Object *CreateCheckersBoard() { return new CheckersBoard; };
   static BoardClass mClass;

};


#endif 
