#ifndef DIALOG_H
#define DIALOG_H

#include <iostream>
#include <string>
#include "Class.h"

class Dialog : public Object {
public:
   virtual bool Run(std::istream &in, std::ostream &out, void *data) = 0;
   
   static void ReadLimitInt(std::istream &is, std::ostream &os,
    int *val, int lo , int hi, std::string prompt);

};

#endif

