
#include "Dialog.h"
#include <iostream>
#include "MyLib.h"

void Dialog::ReadLimitInt(std::istream &in, std::ostream &out,
 int *val, int lo, int hi, std::string prompt) {
   std::string inputString("");
   int inputValue = 0, res = 0;
   char garbage = '\0';

   // TODO: This method is hacked to hell and back to make it diff.
   while (true) {
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

         res = sscanf(inputString.c_str(), " %d %c", &inputValue, &garbage);
         
         if (res == 0) {
            out << "Badly formatted input\n";
            // Clear out trailingChar
            garbage = '\0';
            continue;
         }
         else if (garbage != '\0') {
            out << "Unexpected garbage after value.\n";
            // Clear out trailingChar
            garbage = '\0';
            continue;
         }

         // Save the values and break if you got a good value.
         if (inputValue >= lo && inputValue <= hi) {
            *val = inputValue;
            break;
         } 
            
         out << "Please enter a value between " 
          << lo << " and " << hi << std::endl;
   }
}