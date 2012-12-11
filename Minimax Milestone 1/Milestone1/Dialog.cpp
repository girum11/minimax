
#include "Dialog.h"
#include <iostream>
#include "MyLib.h"

void Dialog::ReadLimitInt(std::istream &in, std::ostream &out,
 int *val, int lo, int hi, std::string prompt) {
   std::string inputString("");
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

         res = sscanf(inputString.c_str(), " %d %c", &inputValue, &trailingChar);
         
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
            out << "Please enter a value between " << lo << " and " << hi << std::endl;
            continue;
         }

         inputSuccessfullyRead = true;
   }
}