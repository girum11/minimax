#ifndef OTHELLODLG_H
#define OTHELLODLG_H

#include "Dialog.h"

class OthelloDlg : public Dialog {
public:
   bool Run(std::istream &in, std::ostream &out, void *data);

};

#endif
