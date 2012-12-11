#ifndef PYLOSVIEW_H_
#define PYLOSVIEW_H_

#include "View.h"

class PylosView : public View {
public:
	void Draw(std::ostream &out);
   const Class *GetClass() const { return &mClass; }
   static Class mClass;
   static Object *CreatePylosView() { return new PylosView; }
};

#endif
