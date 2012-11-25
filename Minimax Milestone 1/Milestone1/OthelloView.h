#ifndef OTHELLOVIEW_H
#define OTHELLOVIEW_H

#include "View.h"

class OthelloView : public View {
public:
   void Draw(std::ostream &out);

   const Class *GetClass() const { return &mClass; };
   static Class mClass;

private:
   static Object *CreateOthelloView() { return new OthelloView; };
};

#endif
