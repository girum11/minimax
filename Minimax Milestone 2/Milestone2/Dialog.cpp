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
   string inputString("");
   int inputValue = 0, res = 0;
   char trailingChar = '\0';
   bool inputSuccessfullyRead = false;

   // TODO: This method is hacked to hell and back to make it diff.
   while (!inputSuccessfullyRead) {
         out << prompt << " [" << lo << ", " << hi << "]: ";

         // Here, sscanf() the whole line to ensure that no trailing garbage 
         // was inputted
         do {
            if (getline(in, inputString).eof()) {
               if (!inputString.empty()) {
                  out << "Badly formatted input\n";
                  out << prompt << " [" << lo << ", " << hi << "]: ";
               }
               throw BaseException("Unexpected EOF");
            }
         } while (inputString.empty());

         res = sscanf(inputString.c_str(), " %d %1s", &inputValue, &trailingChar);
         
         if (res == 0) {
            out << "Badly formatted input\n";
            // Clear out trailingChar
            trailingChar = '\0';
            continue;
         }
         else if (trailingChar != '\0') {
            out << "Unexpected garbage after value.\n";
            // Clear out trailingChar
            trailingChar = '\0';
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
   }
}