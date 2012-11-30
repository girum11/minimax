#ifndef CHECKERSDLG_H
#define CHECKERSDLG_H

#include "Dialog.h"


class CheckersDlg : public Dialog {
public:
   
   bool Run(std::istream &in, std::ostream &out, void *data);
   
   const Class *GetClass() const { return &mClass; }
   static Class mClass;

private:
   static Object *CreateCheckersDlg() { return new CheckersDlg; }

};



#endif