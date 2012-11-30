#ifndef CHECKERSVIEW_H
#define CHECKERSVIEW_H

#include "View.h"

class CheckersView : public View {
public:
   void Draw(std::ostream &out);

   const Class *GetClass() const { return &mClass; }
   static Class mClass;

private:
   static Object *CreateCheckersView() { return new CheckersView; }
};

#endif