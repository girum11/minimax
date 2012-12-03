#include "CheckersDlg.h"
#include "CheckersBoard.h"

using namespace std;

Class CheckersDlg::mClass("CheckersDlg", &CreateCheckersDlg);

bool CheckersDlg::Run(istream &in, ostream &out, void *data) {
   CheckersBoard::Rules *rules = reinterpret_cast<CheckersBoard::Rules *>(data);

   return false;
}