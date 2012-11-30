
#include <assert.h>
#include "CheckersView.h"
#include "CheckersBoard.h"


using namespace std;

Class CheckersView::mClass("CheckersView", &CreateCheckersView);

void CheckersView::Draw(ostream &out) {

   // TODO: Next up is a basic view of the CheckersBoard

   string rtn("");
   int row = 0, col = 0;
   char sqr;
   const CheckersBoard *cb = dynamic_cast<const CheckersBoard *>(mModel);
   assert(cb != NULL);  // TODO: Yank this.

   



   out << "sup nigga";
}