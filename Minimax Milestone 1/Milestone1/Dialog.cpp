/*
 * Dialog.cpp
 *
 *  Created on: Oct 26, 2012
 *      Author: girum
 */

#include "Dialog.h"
#include <assert.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "MyLib.h"

using namespace std;

// This method is used by Othello and Checkers, but not Pylos (since
// Pylos has specific setters that apply logic for its options).
// 
// This method is similar to PylosDlg::ReadMethodInt(), except that there isn't
// setter logic for it.
void Dialog::ReadLimitInt(std::istream &in, std::ostream &out,
 int *val, int lo, int hi, std::string prompt) {
   string inputString;
   int inputValue = 0, res = 0;
   static const int kTrailingCharLength = 11;
   char trailingChar[kTrailingCharLength] = {'\0'};
   bool inputSuccessfullyRead = false;

   while (!inputSuccessfullyRead) {
//      try {
         out << prompt << " [" << lo << ", " << hi << "]: ";
         
         // Here, sscanf() the whole line to ensure that no trailing garbage 
         // was inputted
         if (getline(in, inputString).eof()) {
            throw BaseException("Unexpected EOF");
         }
         res = sscanf(inputString.c_str(), " %d %1s", &inputValue, trailingChar);
         
         if (trailingChar[0] != '\0') {
            out << "Unexpected garbage after value.\n";
            // Clear out trailingChar
            for (int i = 0; i < kTrailingCharLength; ++i) 
               trailingChar[i] = '\0';
            continue;
         } else if (res == 0) {
            out << "Badly formatted input\n";
            // Clear out trailingChar
            for (int i = 0; i < kTrailingCharLength; ++i) 
               trailingChar[i] = '\0';
            continue;
         }

         // Save the values.
         if (inputValue >= lo && inputValue <= hi) {
            *val = inputValue;
         } else {
            out << "Please enter a value between " << lo << " and " << hi << endl;
            continue;
         }

         inputSuccessfullyRead = true;

//       } catch (BaseException &exc) {
//          out << "Error: " << exc.what();
//       } catch (...) {
//          out << "SOME OTHER UNKNOWN ERROR";
//          assert(false);
//       }

      // out << endl;
   }
}