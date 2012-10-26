#include "OthelloDlg.h"
#include "OthelloBoard.h"

using namespace std;

bool OthelloDlg::Run(istream &in, ostream &out, void *data) {
   OthelloBoard::Rules *rules = reinterpret_cast<OthelloBoard::Rules *>(data);
   char resp;

   out << "Side weight: " << rules->sideWgt << endl
      << "Near side weight: " << rules->nearSideWgt << endl
      << "Corner weight: " << rules->cornerWgt << endl
      << "Inner weight: " << rules->innerWgt << endl;

   out << endl << "Modify? [y/n] ";
   if ((in >> resp).eof())
      throw BaseException("Unexpected EOF");
      
   while (in.get() != '\n' && !in.eof())
      ;

   if (resp == 'y') {
      out << endl;
      ReadLimitInt(in, out, &rules->sideWgt, 0, 100, "Enter side weight");
      ReadLimitInt(in, out, &rules->nearSideWgt, 0, 100,
       "Enter near side weight");
      ReadLimitInt(in, out, &rules->cornerWgt, 0, 100, "Enter corner weight");
      ReadLimitInt(in, out, &rules->innerWgt, 0, 100, "Enter inner weight");
   }
   return resp == 'y';
}
