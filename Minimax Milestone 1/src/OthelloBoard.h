#ifndef OTHELLOBOARD_H
#define OTHELLOBOARD_H

#include <iostream>
#include <set>
#include "MyLib.h"
#include "Board.h"

class OthelloMove;

class OthelloBoard : public Board {
public:
   friend class OthelloMove;

   enum {mWPiece = -1, mBPiece = 1, dim = 8};

   // Direction represents the row/col changes needed to move in a direction.
   // Public in order to satisfy defective Visual C++ compiler.
   struct Direction {
      short rDelta;  // Change in row for the direction (-1, 0, or 1)
      short cDelta;  // Change in col for the direction
   };

   struct Rules {
      int cornerWgt;
      int sideWgt;
      int nearSideWgt;
      int innerWgt;
   };

   OthelloBoard();
   ~OthelloBoard();

   long GetValue() const;
   void ApplyMove(Move *);
   void UndoLastMove();
   void GetAllMoves(std::list<Move *> *) const;
   Move *CreateMove() const;
   int GetWhoseMove() const {return mNextMove == mWPiece;}
   const std::list<const Move *> &GetMoveHist() const 
    {return *(std::list<const Move *> *)&mMoveHist;}
   char GetSquare(int row, int col) const {return mBoard[row][col];}

   Board *Clone() const;
   Key *GetKey() const;

   // Option accessor/mutator.  GetOptions returns dynamically allocated
   // object representing options. SetOptions takes similar object.  Caller
   // owns object in both cases.
   static void *GetOptions();
   static void SetOptions(const void *opts);

   const Class *GetClass() const;

protected:
   enum {mNumDirs = 8, squareCount = 64, sqrShift = 2, sqrMask = 0x3};

   std::istream &Read(std::istream &);
   std::ostream &Write(std::ostream &) const;
   void RecalcWeight();  // Recalculate current weight of this OthelloBoard.
   void ClearHistory();  // Clear out move history of this board.

   static BoardClass mClass;
   static std::set<OthelloBoard *> mRoster;
   static Direction mDirs[mNumDirs];
   static short mWeights[dim][dim];
   
   static bool InBounds(int row, int col)
    {return InRange<short>(0, row, dim) && InRange<short>(0, col, dim);}

   char mBoard[dim][dim];       // Current state of board
   char mNextMove;              // Whose move is next (mWPiece or mBPiece)
   char mPassCount;             // How many pass moves have just been made
   short mWeight;               // Current board weight.
   std::list<Move *> mMoveHist; // History of moves thus far.
};

#endif
