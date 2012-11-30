#ifndef CHECKERSBOARD_H
#define CHECKERSBOARD_H

#include "Board.h"
#include "MyLib.h"

class CheckersMove;

class CheckersBoard : public Board {
public:
   friend class CheckersMove;

   // Don't rename these -- mWhite and mBlack are member datum names
   enum {mWPiece = -1, mBPiece = 1};

   // TODO: Put any inner classes you want here


   CheckersBoard(void);
   ~CheckersBoard(void);

   long GetValue() const;
   void ApplyMove(Move *);
   void UndoLastMove();
   void GetAllMoves(std::list<Move *> *) const;
   Move *CreateMove() const;
   int GetWhoseMove() const {return mNextMove == mWhite;}
   const std::list<const Move *> &GetMoveHist() const 
    {return *(std::list<const Move *> *)&mMoveHist;}

   Board *Clone() const;
   Key *GetKey() const;

   // Option accessor/mutator.  GetOptions returns dynamically allocated
   // object representing options. SetOptions takes similar object.  Caller
   // owns object in both cases.
   static void *GetOptions();
   static void SetOptions(const void *opts);

   const Class *GetClass() const { return &mClass; };

protected:
   
   typedef ulong Set;

   std::istream &Read(std::istream &);
   std::ostream &Write(std::ostream &) const;


   static BoardClass mClass;

   // Bitmasks indicating which cells contain white pieces, black pieces,
   // and Kings.  No-marble cells are 0 in both masks.  Bits are assigned to
   // cells in row-major, column-minor order, from LSB to MSB.  All 32 bits
   // are thus used.
   mutable Set mBlack;
   mutable Set mWhite;
   mutable Set mKings;
   
   char mNextMove;
   std::list<Move *> mMoveHist; // History of moves thus far.
   
private:
   static Object *CreateCheckersBoard() { return new CheckersBoard; };

};


#endif 
