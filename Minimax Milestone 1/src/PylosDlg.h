#ifndef PYLOSDLG_H
#define PYLOSDLG_H

#include "Class.h"
#include "Dialog.h"
#include "PylosBoard.h"

class PylosDlg : public Dialog {
public:
   bool Run(std::istream &in, std::ostream &out, void *data);
   // Two methods and a member datum to support the Class system

private:
   // Prompt for an int (using the string parameter).  Call the method
   // of PylosRules indicated by the method pointer to set the rules
   // object's property.  Catch and report any exceptions, looping
   // until reaching a value that has no exceptions.  Use "in"
   // and "out" for the dialog, not cin and cout.
   void ReadMethodInt(std::istream &in, std::ostream &out, std::string,
    PylosBoard::Rules *, void (PylosBoard::Rules::*x)(int));
};

#endif