
#include <assert.h>
#include "CheckersView.h"
#include "CheckersBoard.h"


using namespace std;

Class CheckersView::mClass("CheckersView", &CreateCheckersView);

void CheckersView::Draw(ostream &out) {
   string rtn("");
   int row = 0, col = 0;
   char sqr;
   const CheckersBoard *cb = dynamic_cast<const CheckersBoard *>(mModel);
   assert(cb != NULL);  // TODO: Yank this.

   



   out << rtn;
}