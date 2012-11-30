#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

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
   
   // kDim is equal to the board's Cell length divided by 2 -- which is the
   // number of pieces you can fit on a single row/column of the board.
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

   static inline Cell *GetCell(int row, int col) {
      
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
