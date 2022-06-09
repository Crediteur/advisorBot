#pragma once

#include <vector>
#include <string>

#include "CSVReader.h"
#include "OrderBook.h"
#include "CommandLine.h"

class MerkelMain{
    public:
        // default initialization
        MerkelMain();
        void init();

    private:
        // user commands
        void printMenu();
        void getUserOption();
        bool checkUserOption(std::string& userLine);
        void processUserOption();

        // stores the user input 
        std::vector<std::string> userInput{};

        // commandLine logic and msgs
        CommandLine commandLine;

        // point to CSV file through orderBook
        OrderBook orderBook{"20200601.csv"};

        // tracks currentTime and step
        std::string currentTime;

};