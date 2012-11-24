#ifndef PYLOSDLG_H
#define PYLOSDLG_H

#include "Class.h"
#include "Dialog.h"
#include "PylosBoard.h"

class PylosDlg : public Dialog {
public:
   bool Run(std::istream &in, std::ostream &out, void *data);

   // [*Staley] Two methods and a member datum to support the Class system
   const Class *GetClass() const;

private:
   // [*Staley] Prompt for an int (using the string parameter).  Call the method
   // [*Staley] of PylosRules indicated by the method pointer to set the rules
   // [*Staley] object's property.  Catch and report any exceptions, looping
   // [*Staley] until reaching a value that has no exceptions.  Use "in"
   // [*Staley] and "out" for the dialog, not cin and cout.
   void ReadMethodInt(std::istream &in, std::ostream &out, std::string prompt,
    PylosBoard::Rules *rules, void (PylosBoard::Rules::*x)(int));
};

#endif