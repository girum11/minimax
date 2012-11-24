#include "PylosDlg.h"

#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

using namespace std;

// [Staley] Run the dialog object, passing it the options for the default board
// [Staley] as a void *.  You’ll have to use the BoardClass for the default 
// [Staley] board type to get this void *.  The dialog object modifies the 
// [Staley] board options by prompting the user after displaying the current 
// [Staley] options, and if the dialog run is successful, BoardTest sets the 
// [Staley] default board’s options to the new value.
// [Staley] 
// [Staley] The options to set for Pylos and Checkers are the various weights 
// [Staley] described under GetValue above.  See my model code for format 
// [Staley] details.
// [Staley] 
// [Staley] Whenever the static weights are changed all existing boards of the 
// [Staley] current board class must automatically reflect the change in the 
// [Staley] return of GetValue.
bool PylosDlg::Run(std::istream &in, std::ostream &out, void *data) {
   PylosBoard::Rules *rules = reinterpret_cast<PylosBoard::Rules *>(data);
   char resp;

   out << "Marble weight: " << rules->marbleWgt << endl
      << "Level weight: " << rules->levelWgt << endl
      << "Free weight: " << rules->freeWgt << endl;

   out << endl << "Modify? [y/n] ";
   if ((in >> resp).eof())
      throw BaseException("Unexpected EOF");
      
   while (in.get() != '\n' && !in.eof())
      ;

   if (resp == 'y') {
      out << endl;
      ReadMethodInt(in, out, "Enter marble weight", rules, &PylosBoard::Rules::SetMarble);
      ReadMethodInt(in, out, "Enter level weight", rules, &PylosBoard::Rules::SetLevel);
      ReadMethodInt(in, out, "Enter free weight", rules, &PylosBoard::Rules::SetFree);
   }
   return resp == 'y';
}

// [*Staley] Prompt for an int (using the string parameter).  Call the method
// [*Staley] of PylosRules indicated by the method pointer to set the rules
// [*Staley] object's property.  Catch and report any exceptions, looping
// [*Staley] until reaching a value that has no exceptions.  Use "in"
// [*Staley] and "out" for the dialog, not cin and cout.
void PylosDlg::ReadMethodInt(istream &in, ostream &out, string prompt,
 PylosBoard::Rules *rules, void (PylosBoard::Rules::*x)(int)) {
   int input;
   bool inputSuccessfullyRead = false;

   while (!inputSuccessfullyRead) {
      try {
         out << prompt;
         in >> input;
         (rules->*x)(input);
         inputSuccessfullyRead = true;
      } catch (BaseException &exc) {
         out << "Error: " << exc.what() << endl;
      } catch (...) {
         out << "SOME OTHER UNKNOWN ERROR" << endl;
      }
   }
}


// TODO: Implement reflection
const Class *PylosDlg::GetClass() const {
   return 0;
}