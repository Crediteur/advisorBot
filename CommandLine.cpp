#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds/milliseconds [DO NOT USE PERIODS]
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "CommandLine.h"
#include "OrderBook.h"
#include "OrderBookEntry.h"

CommandLine::CommandLine(){}

// returns true if string matches a command from the commands list
bool CommandLine::commandExists (const std::string str){
    return std::find(commands.begin(), commands.end(),str) !=commands.end();
}

// user has partially entered "help" as first word in their input
void CommandLine::help(){
    
        std::cout << "The avaliable <command>s are:\n";
        std::cout << " help <command>\n";
        std::cout << " prod\n";
        std::cout << " min <product> <bid/ask>\n";
        std::cout << " max <product> <bid/ask>\n";
        std::cout << " avg <product> <bid/ask> <steps>\n";
        std::cout << " predict <max/min> <product> <bid/ask> *<steps>\n";
        std::cout << " time\n";
        std::cout << " step <steps>" << std::endl;
        std::cout << " devi <product> <bid/ask>\n";
        std::cout << " exit\n";
        //std::cout << "customCommand" << std::endl;
    //std::cout << "CommandLine::help" << std::endl;
}

// prints appropriate message checking help <command>
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
    else if (command == "devi"){
        std::cout << "step - compute the standard deviation of the product bid/ask" << std::endl;
        std::cout << "      ie. devi ETH/USDT ask" << std::endl;
    }
    else if (command == "exit"){
        std::cout << "exit - stop execution and shut down the program" << std::endl;
    }
    else{
        std::cout << "CommandLine::helpCMD: Error, no matching command" << std::endl;
    }
}

// list all product types found parsing CSV
void CommandLine::prod(std::vector<std::string>& products){
    for (std::string& s: products){
        std::cout << s << std::endl;
    }
}

// prints minimum price order filtered by "min <product> <bid/ask>" of current time frame
void CommandLine::min(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders){

    std::cout << "Minimum " << product << " " << type << ": " << OrderBook::getLowPrice(filteredOrders) << std::endl;
}

// prints maximum price order filtered by "max <product> <bid/ask>" of current time frame
void CommandLine::max(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders){

    std::cout << "Maximum " << product << " " << type << ": " << OrderBook::getHighPrice(filteredOrders) << std::endl;
}

// avg is simple moving avg over timesteps ([F1+F2..+Fn] / n)
void CommandLine::avg(std::string& product, std::string& type, std::string& steps, double& average){

    std::cout << "The average " << product << " " << type << 
    " price over the last " << steps << " steps was: " << average << std::endl;
}

// predicts the max/min bid/ask for the next time step using the moving avg (10?)
void CommandLine::predict(std::string& minmax, std::string& product, std::string& type, std::string& steps, double& average){
    std::cout << "The " << minmax << " price of " << product << " " << type << 
    " price over the last " << steps << " steps is predicted to be: " << average << std::endl;
}

// prints current time frame
void CommandLine::time(std::string& currentTime, int& timestep, int& csvline){
    
    std::cout << "Current time: " << currentTime << ", Step: " << timestep 
    << ", Line: "<< csvline << std::endl;
}

// move to next time step, total: 2147 steps in 20200601.csv
void CommandLine::step(std::string& currentTime, int& timestep){
    
    std::cout << "Going to next time frame... \n";
        std::this_thread::sleep_for (std::chrono::milliseconds(800));
    std::cout << "Moved to new: " << currentTime << ", Step: " << timestep << std::endl;
}

// prints product's standard deviation of current and last 10 steps 
void CommandLine::devi(std::string& product, std::string& type, std::pair<double, double> deviationPair){
    std::cout << "The standard deviation of " << product << " " << type << ":" << std::endl;
    std::cout << "  over the current step is:   " << deviationPair.first << std::endl; 
    std::cout << "  over the last 10 steps was: " << deviationPair.second << std::endl;
}

// terminate program
void CommandLine::exit(){
    std::cout << "Self destruct countdown initiated...\n";
    std::this_thread::sleep_for (std::chrono::milliseconds(800));
    std::cout << "3...\n";
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    std::cout << "2...\n";
    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
    std::cout << "1...";
    std::this_thread::sleep_for (std::chrono::milliseconds(800));
    std::cout << "!";
    std::this_thread::sleep_for (std::chrono::milliseconds(800));
    std::cout << "\n\nd8888b.  .d88b.   .d88b.  .88b  d88. db \n";
    std::cout << "88  `8D .8P  Y8. .8P  Y8. 88'YbdP`88 88 \n";
    std::cout << "88oooY' 88    88 88    88 88  88  88 YP \n";
    std::cout << "88~~~b. 88    88 88    88 88  88  88    \n";
    std::cout << "88   8D `8b  d8' `8b  d8' 88  88  88 db \n";
    std::cout << "Y8888P'  `Y88P'   `Y88P'  YP  YP  YP YP \n";
    std::cout << "\nThank you for using advisorBot. Goodbye!" << std::endl;
}

/** TODO: refractor code: link MerkelMain -> CommandLine -> orderBook
        to move orderBook usage into seperate functions in CommandLine class
        userInput and currentTime will have to be moved into CommandLine*/