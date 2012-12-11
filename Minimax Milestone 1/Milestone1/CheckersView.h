#ifndef CHECKERSVIEW_H
#define CHECKERSVIEW_H

#include "View.h"

class CheckersView : public View {
public:
   void Draw(std::ostream &out);
   const Class *GetClass() const { return &mClass; }
   static Class mClass;
   static Object *CreateCheckersView() { return new CheckersView; }
   
   static inline bool IsValidCell(const char &row, const unsigned int &col) {
      return ((!((row - 'A') % 2) && !((col - 1) % 2)) 
       || (((row - 'A') % 2) && ((col - 1) % 2)));
   }
};

#endif