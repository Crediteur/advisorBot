#pragma once

#include <string>
#include <vector>

#include "OrderBookEntry.h"

class CSVReader{
    public:
        CSVReader();
        // read csv path name
        static std::vector<OrderBookEntry> readCSV(std::string csvFilename);

        // parse a string sentence on char into a vector of strings
        static std::vector<std::string> tokenise(std::string csvLine, char separator);
    
    private:
        // converts tokenised (vector of strings) into an OrderBookEntry 
        static OrderBookEntry stringToOBE(std::vector<std::string> tokens);
};