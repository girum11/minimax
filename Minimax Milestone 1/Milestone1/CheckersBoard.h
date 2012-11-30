#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

#include <assert.h>
#include "Board.h"
#include "MyLib.h"

class CheckersMove;

class CheckersBoard : public Board {
public:
   friend class CheckersMove;

   enum { kWhite = -1, kBlack = 1, kKing = 7 };
   enum { kNumCells = 32, kStartingPieces = 12, kStartingBackPieces = 4 };

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
   int GetWhoseMove() const {return mWhoseMove == kWhite;}
   const std::list<const Move *> &GetMoveHist() const 
    {return *(std::list<const Move *> *)&mMoveHist;}

   Board *Clone() const;
   Key *GetKey() const;

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
   
   // kDim is equal to vertical height of the board, measured in Cells.
   enum { kDim = 4 };

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

   // Quick helper functions for GetCell()
   static inline bool IsEven(char num) { return num % 2; }
   static inline bool IsOdd(char num) { return !IsEven(num); }

   static inline Cell *GetCell(int row, int col) {
      // Out of bounds assertion
      assert(row >= 'A' && row <= 'H' && col >= 1 && col <= 8);

      // Reduce the inputted characters to their numerical form
      // (I drew up a diagram of this in my binder)
      row -= 'A';
      col -= 1;
      
      // Invalid square assertion [the operator=(string) should've caught
      // that before flow of control gets here].  Row and Col must either 
      // both be odd or both be even to be valid.
      assert((IsEven(row) && IsEven(col)) || (IsOdd(row) && IsOdd(col)));

      int offset = (row*kDim) + (col/2);
      std::cout << "GetCell() returns offset: " << offset << std::endl << std::endl;

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

   int mWhoseMove; // Whose move it is, using kWPiece and kBPiece.

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
