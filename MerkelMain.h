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

        // move timeframe specified amount, returns true if successful
        bool moveStep(int steps);
        bool moveStep(std::string& steps);

        // gets filtered orders by parameter, calculates the sum over each time step and then returns the average
        double avgAlgo(std::string& product, OrderBookType& type, int stepCount);

        // calculate the average of max/min bid/ask over a fixed or optional time frame
        double predictAlgo(std::string& maxMin, std::string& product, OrderBookType& type);
        double predictAlgo(std::string& maxMin, std::string& product, OrderBookType& type, int stepCount);
        
        // calculates standard deviation of current and last 10 frames
        std::pair<double, double> standardDevi(std::string& product, OrderBookType& type);

        // commandLine logic and msgs
        CommandLine commandLine;

        // tracks currentTime and step
        std::string currentTime;

        // point to CSV file through orderBook
        OrderBook orderBook{"20200601.csv"};

        // flag to exit program
        int loopStatus = 1;
};