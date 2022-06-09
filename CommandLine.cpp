#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "CommandLine.h"
#include "OrderBook.h"
#include "OrderBookEntry.h"

CommandLine::CommandLine(){}

// returns true if string matches a command from the commands list
bool CommandLine::commandExists(std::string str){
    return std::find(commands.begin(), commands.end(),str) !=commands.end();
}

// user has partially entered help as first word in their input
void CommandLine::help(){
    
        std::cout << "The avaliable <command>s are:\n";
        std::cout << " help <command>\n";
        std::cout << " prod\n";
        std::cout << " min <product> <bid/ask>\n";
        std::cout << " max <product> <bid/ask>\n";
        std::cout << " avg <product> <bid/ask>\n";
        std::cout << " predict <max/min> <product> <bid/ask>\n";
        std::cout << " time\n";
        std::cout << " step" << std::endl;
        //std::cout << "customCommand" << std::endl;
    //std::cout << "CommandLine::help" << std::endl;
}

// user has entered exactly help <cmd>
void CommandLine::helpCMD(std::string command){
    if (command == "prod"){
        std::cout << "prod - list all avaliable products" << std::endl;
    }
    else if (command == "min"){
        std::cout << "min - find the minimum bid/ask for the product in the current time step\n";
        std::cout << "      ie. min ETH/BTC ask" << std::endl;
    }
    else if (command == "max"){
        std::cout << "max - find the maximum bid/ask for the product in the current time step\n";
        std::cout << "      ie. max BTC/DOGE bid" << std::endl;
    }
    else if (command == "avg"){
        std::cout << "avg - compute the average bid/ask for the product over the next given time steps\n";
        std::cout << "      ie. avg BTC/ETH ask 20" << std::endl;
    }
    else if (command == "predict"){
        std::cout << "predict - compute max/min bid/ask for the product in the next time step\n";
        std::cout << "      ie. predict max ETH/DOGE bid" << std::endl;
    }
    else if (command == "time"){
        std::cout << "time - print current timeframe" << std::endl;

    }
    else if (command == "step"){
        std::cout << "step - move to the next time frame(s)" << std::endl;
        std::cout << "      ie. step 10" << std::endl;
    }
    else{
        std::cout << "CommandLine::helpCMD: Error, no matching command" << std::endl;
    }
}

// list all product types
void CommandLine::prod(std::vector<std::string>& products){
    for (std::string& s: products){
        std::cout << s << std::endl;
    }
    //std::cout << "CommandLine::prod" << std::endl;
}

// returns minimum price order filtered by "min <product> <bid/ask>" of current time frame
void CommandLine::min(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders){

    std::cout << "Minimum " << product << " " << type << ": " << OrderBook::getLowPrice(filteredOrders) << std::endl;
    //std::cout << "CommandLine::min" << std::endl;
}

// returns maximum price order filtered by "max <product> <bid/ask>" of current time frame
void CommandLine::max(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders){

    std::cout << "Maximum " << product << " " << type << ": " << OrderBook::getHighPrice(filteredOrders) << std::endl;
    // std::cout << "CommandLine::max" << std::endl;
}

void CommandLine::avg(){
    // avg is simple moving avg over timesteps ([E1+E2..+En] / n)
    // Common moving average lengths are 10, 20, 50, 100, and 200
    std::cout << "CommandLine::avg" << std::endl;
}
void CommandLine::predict(){
    // predicts the max/min bid/ask for the next time step using the moving avg (10?)
    // max/min will loook at the orders higher/lower than the average and predict the next max/min step
    std::cout << "CommandLine::predict" << std::endl;
}

// prints current time frame
void CommandLine::time(std::string& currentTime, int& timestep){
    
    std::cout << "Current time: " << currentTime << ", Step: " << timestep << std::endl;
    // std::cout << "CommandLine::time" << std::endl;
}

// move to next time step, total: 2147 steps in 20200601.csv
void CommandLine::step(std::string& currentTime, int& timestep){
    
    std::cout << "Now at " << currentTime << ", Step: " << timestep << std::endl;
    // std::cout << "CommandLine::step" << std::endl;
}

// void CommandLine::customCommand(){
//     std::cout << "CommandLine::step" << std::endl;
// }

void avgAlgo();
void predictAlgo();