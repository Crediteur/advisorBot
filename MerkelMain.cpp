#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds/milliseconds [DO NOT USE PERIODS]
#include <iostream>
#include <string>
#include <algorithm>

#include "MerkelMain.h"
#include "CSVReader.h"
#include "OrderBookEntry.h"
#include "CommandLine.h"

MerkelMain::MerkelMain(){}

void MerkelMain::init(){

    // start on orderBook.timestep = 20, so predict command works
    currentTime = orderBook.getEarlistTime();
    for (int i = 0; i < 19; ++i){
        currentTime = orderBook.getNextTime(currentTime);
    }

    // initalize commandLine vector with recognized commands
    commandLine.commands = {"help", "cmd", "avg", "prod", "min", "max", "avg", "predict", "time", "step"};
    
    // welcome message
    std::cout << "Current time: " << currentTime << std::endl;
    std::this_thread::sleep_for (std::chrono::seconds(1));
    std::cout << "Welcome to advisorBot! Explore the latest cryptocurrency market trends here!" << std::endl;

    // loop of parsing input and main menu
    while (true){
        printMenu();
        getUserOption();
        // processUserOption, update global variable userInput
        processUserOption(); //new class?
    }
}

void MerkelMain::printMenu(){
    
    std::this_thread::sleep_for (std::chrono::seconds(1));
    std::cout << "\nPlease enter a command, or \"help\" for a list of commands" << std::endl;
}

void MerkelMain::getUserOption(){

    std::string line;
    // error check user input
    do{
        userInput.clear();

        // getLine from user
        std::getline(std::cin, line);

        // tokenize string user input
        userInput = CSVReader::tokenise(line, ' ');

    } while(line == "" || !checkUserOption(userInput.at(0)));
    //std::cout << "MerkelMain::getUserOption" << std::endl;
}

// return true if first word of string is valid, otherwise false 
bool MerkelMain::checkUserOption(std::string& userWord){
    if (commandLine.commandExists(userWord)){
        return true;
    }
    std::cout << "\nMerkelMain::checkUserOption: Unknown command, try again" << std::endl;
    return false;
    //std::cout << "MerkelMain::checkUserOption" << std::endl;
}

// call function matching userInput string at index 0, passing appropriate parameters
void MerkelMain::processUserOption(){
    // print help menu, first check if second string is a command
    if(userInput.at(0) == "help"){

        if(userInput.size() == 2 && userInput.at(1) != "help" && commandLine.commandExists(userInput.at(1))){
            commandLine.helpCMD(userInput.at(1)); 
        }
        else{
            commandLine.help();
        }
    }
    // list all product types, pass products from orderbook first
    else if (userInput.at(0) == "prod"){ 
        std::vector<std::string> products = orderBook.getKnownProducts();
        commandLine.prod(products); 
    }

    // returns minimum price order filtered by "min <product> <bid/ask>" of current time frame
    else if (userInput.at(0) == "min"){

        if (userInput.size() < 3){
            std::cout << "CommandLine::min: Invalid parameters, format: min <product> <bid/ask>" << std::endl;
            return;
        }
        else{
            OrderBookType type;
            std::vector<OrderBookEntry> filteredOrders;
            // check product exists in csv
            try {
                type = OrderBookEntry::stringToOrderBookType(userInput.at(2));
                filteredOrders = orderBook.getOrders(type, userInput.at(1), currentTime);
                if (filteredOrders.size() == 0 || type == OrderBookType::unknown){
                    throw std::exception{};
                }
            }
            catch(std::exception& e){
                std::cout << "Unknown product: " << userInput.at(1) << " or Unknown order: " << userInput.at(2) << std::endl;
                return;
            }
            commandLine.min(userInput.at(1), userInput.at(2), filteredOrders);
        }//end else
    }

    // returns maximum price order filtered by "min <product> <bid/ask>" of current time frame
    else if (userInput.at(0) == "max"){ 

        if (userInput.size() < 3){
            std::cout << "CommandLine::min: Invalid parameters, format: max <product> <bid/ask>" << std::endl;
            return;
        }
        else{
            OrderBookType type;
            std::vector<OrderBookEntry> filteredOrders;
            try {
                type = OrderBookEntry::stringToOrderBookType(userInput.at(2));
                filteredOrders = orderBook.getOrders(type, userInput.at(1), currentTime);
                if (filteredOrders.size() == 0 || type == OrderBookType::unknown){
                    throw std::exception{};
                }
            }
            catch(std::exception& e){
                std::cout << "Unknown product: " << userInput.at(1) << " or Order type: " << userInput.at(2) << std::endl;
                return;
            }
            commandLine.max(userInput.at(1), userInput.at(2), filteredOrders);
        }//end else
    }

    else if (userInput.at(0) == "avg"){ commandLine.avg(); }
    else if (userInput.at(0) == "predict"){ commandLine.predict(); }
    else if (userInput.at(0) == "time"){ commandLine.time(currentTime, orderBook.timestep); }
    else if (userInput.at(0) == "step"){ 

        int stepCount = 1;
        if (userInput.size() > 1){
            try{
                stepCount = std::stoi(userInput.at(1));
            }
            catch (std::exception& e){
                std::cout << "CommandLine::step: Error, Unknown parameter: " << userInput.at(1) << std::endl;
            }
        }//end if
        
        for (int i = 0; i < stepCount; ++i){
            currentTime = orderBook.getNextTime(currentTime);
        }
        //call function to print
        commandLine.step(currentTime, orderBook.timestep); 
    }
    
    // else if (userInput.at(0) == "customCommand"){ customCommand(); }
    else{ std::cout << "\nMerkelMain::processUserOption: Unknown command" << std::endl; }

    //std::cout << "MerkelMain::processUserOption" << std::endl;
}