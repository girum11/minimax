#ifndef SIMPLEAIPLAYER_H
#define SIMPLEAIPLAYER_H

#include "BestMove.h"
#include "limits.h"

class Book;

class SimpleAIPlayer {
public:
   // Deepest allowed minimax level
   enum {SAVE_LEVEL = 1};

   static void Minimax(Board *brd, int lvl, long min, long max, BestMove *res,
    Book *bk, int debugLvl = 0);
};

#endif