#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include "CSVReader.h"
#include "OrderBookEntry.h"

CSVReader::CSVReader(){}

// read csv path name
std::vector<OrderBookEntry> CSVReader::readCSV(std::string csvFilename){

    //open csv
    std::ifstream csvFile{csvFilename};

    std::vector<OrderBookEntry> entries;
    std::string line;
    int count {0};

    if (csvFile.is_open()){
        
        std::cout << csvFilename << " is open" << std::endl;

        // loop through lines in csv
        while (std::getline(csvFile, line)){
            
            ++count;

            //skip blank lines
            if (line == ""){ 
                continue;
            }

            try {
                // tokenise, convert vector to an entry, and push entry to entries
                entries.push_back(stringToOBE(tokenise(line, ',')));
            }
            catch (const std::exception& e){
                std::cout << "CSVReader::readCSV: bad data on line " << count << std::endl;
                continue;
            }
        }//end while
    }//end if

    std::cout << "Successfully read " << entries.size() << " lines from " << csvFilename << std::endl;
    csvFile.close();

    return entries;
}

// parse a string sentence on char into a vector of strings
std::vector<std::string> CSVReader::tokenise(std::string csvLine, char separator){

    std::vector<std::string> tokens{};
    std::string token{};
    unsigned int start, end;
    
    start = csvLine.find_first_not_of(separator,0);

    do{
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end) 
            break;
        if (end >= 0)
            token = csvLine.substr(start, end-start);
        else
            token = csvLine.substr(start, csvLine.length()-start);

        tokens.push_back(token);

        if (end > csvLine.length()) break;
        start = end +1;

    }while(end > 0);

    return tokens;
}

// converts tokenised (vector of strings) into an OrderBookEntry 
OrderBookEntry CSVReader::stringToOBE(std::vector<std::string> tokens){

    double price, amount;

    // check file size, error is caught back in readCSV
    if (tokens.size() != 5){
        std::cout << "CSVReader::stringToOBE: Incorrect OBE format, not size 5" << std::endl;
        throw std::exception{};
    }

    try{
        price = std::stod(tokens[3]);  
        amount = std::stod(tokens[4]);
    }
    catch (const std::exception& e){
        std::cout << "CSVReader::stringToOBE: Bad float parse, \"" << tokens[3] << "\" or \"" << tokens[4] << std::endl;
        throw std::exception{};
    }

    //create OrderBookEntry object {double, double, str, str, OBT}
    OrderBookEntry obe{price, amount, tokens[0], tokens[1], OrderBookEntry::stringToOrderBookType(tokens[2])};
    return obe;
}