#include "PylosDlg.h"

using namespace std;

Class PylosDlg::mClass("PylosDlg", &CreatePylosDlg);

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
   char userResponse;

   out << "Marble weight: " << rules->marbleWgt << endl
      << "Level weight: " << rules->levelWgt << endl
      << "Free weight: " << rules->freeWgt << endl;

   // Ask user if he wants to modify the Options, and save response to 'resp'
   out << "Modify? [y/n] ";
   if ((in >> userResponse).eof())
      throw BaseException("Unexpected EOF");
      
   // Flush out 'in' until the end of the line
   while (in.get() != '\n' && !in.eof())
      ;
   
   out << endl;

   if (userResponse == 'y') {
      ReadMethodInt(in, out, "Enter marble weight: ", rules, &PylosBoard::Rules::SetMarble);
      ReadMethodInt(in, out, "Enter level weight: ", rules, &PylosBoard::Rules::SetLevel);
      ReadMethodInt(in, out, "Enter free weight: ", rules, &PylosBoard::Rules::SetFree);
   }
   
   return userResponse == 'y';
}

// [*Staley] Prompt for an int (using the string parameter).  Call the method
// [*Staley] of PylosRules indicated by the method pointer to set the rules
// [*Staley] object's property.  Catch and report any exceptions, looping
// [*Staley] until reaching a value that has no exceptions.  Use "in"
// [*Staley] and "out" for the dialog, not cin and cout.
void PylosDlg::ReadMethodInt(istream &in, ostream &out, string prompt,
 PylosBoard::Rules *rules, void (PylosBoard::Rules::*x)(int)) {
   string inputString;
   int inputValue = 0;
   static const int kTrailingCharLength = 11;
   char trailingChar[kTrailingCharLength] = {'\0'};
   bool inputSuccessfullyRead = false;

   while (!inputSuccessfullyRead) {
      try {
         out << prompt;
         
         // Here, sscanf() the whole line to ensure that no trailing garbage 
         // was inputted
         getline(in, inputString);
         sscanf(inputString.c_str(), " %d %1s", &inputValue, trailingChar);
         if (trailingChar[0] != '\0') {
            out << "Badly formatted input\n";
            // Clear out trailingChar
            for (int i = 0; i < kTrailingCharLength; ++i) trailingChar[i] = '\0';
            continue;
         }

         // Execute the setter member function
         (rules->*x)(inputValue);
         inputSuccessfullyRead = true;
      } catch (BaseException &exc) {
         out << "Error: " << exc.what();
      } catch (...) {
         out << "SOME OTHER UNKNOWN ERROR";
         assert(false);
      }

      out << endl;
   }
}