#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

#include <assert.h>
#include "Board.h"
#include "MyLib.h"

class CheckersMove;

class CheckersBoard : public Board {
public:
   friend class CheckersMove;

   // Game logic relies on kWhite being -1 and kBlack being 1
   // kHeight is equal to vertical height of the board, measured in Cells.
   // kDim is equal to kHeight / 2 -- the number of actual Cells you can fit
   // vertically in the board
   enum { kWhite = -1, kBlack = 1, kKing = 7, kNumCells = 32, kDim = 4 };

   // Running Clint's version of this says the following:
   // "(Piece weight is always 100)"
   static const int pieceWgt = 100, kWidth = 8;

   struct Rules {
      int kingWgt, backRowWgt, moveWgt;
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

   // Arrange for this to be called at static initialization time, to set up
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
   
   inline void Put(Piece *piece, Cell *cell) const;
   inline Piece *Take(Cell *cell, int color) const;

   void RefreshBoardValuation();

   void MultipleJumpDFS(std::list<CheckersMove *> *, 
    std::vector<std::pair<char, unsigned int> >, Cell *) const;

   inline bool CanMove(Cell *cell, int direction) const;
   inline bool CanJump(Cell *cell, int dir) const;
   inline bool IsValidDirection(Cell *cell, int direction) const;
   
   // Quick helper functions for GetCell()
   static inline bool IsEven(char num) { return num % 2; }
   static inline bool IsOdd(char num) { return !IsEven(num); }

   static inline Cell *GetCell(char row, unsigned col);
   static inline Cell *GetCell(const std::pair<char, unsigned int> &loc) {
      return GetCell(loc.first, loc.second);
   }

   // Static member datum goes here
   struct CheckersBoardInitializer {
      CheckersBoardInitializer() { CheckersBoard::StaticInit(); }
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