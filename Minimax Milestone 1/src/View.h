#ifndef VIEW_H
#define VIEW_H

#include <iostream>
#include "Board.h"
#include "Class.h"

class View : public Object {
public:
   virtual void Draw(std::ostream &out) = 0;
   virtual void SetModel(const Board *brd) {mModel = brd;}

protected:
   const Board *mModel;
};

#endif
