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

    currentTime = orderBook.getEarlistTime();
    /** TODO: link MerkelMain -> CommandLine -> orderBook
        to move orderBook usage into seperate functions in CommandLine class
        userInput and currentTime will have to be moved into CommandLine*/

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
    std::cout << "MerkelMain::checkUserOption: Unknown command, try again" << std::endl;
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
        std::vector<std::string> products = orderBook.getKnownProducts(); // if refactoring, move this call to commandLine
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
                std::cout << "Unknown product: " << userInput.at(1) << " or Type: " << userInput.at(2) << std::endl;
                return;
            }
            commandLine.min(userInput.at(1), userInput.at(2), filteredOrders);
        }//end else
    }

    // returns maximum price order filtered by "min <product> <bid/ask>" of current time frame
    else if (userInput.at(0) == "max"){ 

        if (userInput.size() < 3){
            std::cout << "CommandLine::max: Invalid parameters, format: max <product> <bid/ask>" << std::endl;
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
                std::cout << "Unknown Product: " << userInput.at(1) << ", or Type: " << userInput.at(2) << std::endl;
                return;
            }
            commandLine.max(userInput.at(1), userInput.at(2), filteredOrders);
        }//end else
    }

    // calculates average of product price after moving timesteps 
    // by format "avg <product> <bid/ask> <steps>"
    else if (userInput.at(0) == "avg"){

        if (userInput.size() < 4){
            std::cout << "CommandLine::avg: Invalid parameters, format: avg <product> <bid/ask> <steps>" << std::endl;
            return;
        }

        OrderBookType type;
        std::vector<OrderBookEntry> filteredOrders;
        int stepCount;
        try{
            type = OrderBookEntry::stringToOrderBookType(userInput.at(2));
            filteredOrders = orderBook.getOrders(type, userInput.at(1), currentTime);
            stepCount = std::stoi(userInput.at(3));
            if (filteredOrders.size() == 0 || type == OrderBookType::unknown || stepCount < 0){
                throw std::exception{};
            }
        }
        catch(std::exception& e){
            std::cout << "Unknown Product: " << userInput.at(1) << ", Type: " 
            << userInput.at(2) <<", or Steps: "<< userInput.at(3) <<std::endl;
            return;
        }

        // pass to algorithm function to calculate moving average
        double averageWithStep = avgAlgo(userInput.at(1), type, stepCount);

        //print results
        commandLine.avg(userInput.at(1), userInput.at(2), userInput.at(3), averageWithStep);
        //commandLine.avg(std::string product, std::string type, double sum, int timestep); 
    }

    // calculate the average of max/min bid/ask over a fixed timestep 
    // by format "predict <max/min> <product> <bid/ask> *<steps>"
    else if (userInput.at(0) == "predict"){ 
        
        std::string stepString = "10";
        int stepCount = 10; //set default steps to 10
        if (userInput.size() < 4){
            std::cout << "CommandLine::predict: Invalid parameters, format: predict <max/min> <product> <bid/ask> *<steps>" << std::endl;
            return;
        }
        // check and try to convert optional parameter *<steps> // amazing V parameter 😍👌😃 //use in line 202 !(max||min)
        else if (userInput.size() > 4 && !commandLine.commandExists(userInput.at(1))){
            try{
                stepCount = std::stoi(userInput.at(4));
                if (stepCount < 0 ){ 
                    std::cout << "MerkelMain::predict: Error, steps cannot be negative" << std::endl;
                    return;
                }
            }
            catch (std::exception& e){
                stepCount = 10;
            }
            stepString = userInput.at(4);
        }

        //max/min = 1, product = 2, bid/ask = 3, *steps = 4
        OrderBookType type;
        std::vector<OrderBookEntry> filteredOrders;
        try{
            type = OrderBookEntry::stringToOrderBookType(userInput.at(3));
            filteredOrders = orderBook.getOrders(type, userInput.at(2), currentTime);
            if (filteredOrders.size() == 0 || type == OrderBookType::unknown
                || !(userInput.at(1) == "max" || userInput.at(1) == "min")){
                throw std::exception{};
            }
        }
        catch(std::exception& e){
            std::cout << "Unknown String: " << userInput.at(1) << ", Product: " << userInput.at(2) 
            << ", Type: " << userInput.at(3) <<", or Steps: " << stepCount << std::endl;
            //or Steps: "<< userInput.at(3) <<std::endl;
            return;
        }

        double averagePredict;
        // pass to algorithm function to calculate moving average
        if (stepCount != 10){
            averagePredict = predictAlgo(userInput.at(1), userInput.at(2), type, stepCount); // overload with timestep
        }
        else{
            averagePredict = predictAlgo(userInput.at(1), userInput.at(2), type);
        }

        //print results
        commandLine.predict(userInput.at(1), userInput.at(2), userInput.at(3), stepString, averagePredict);
        //commandLine.avg(std::string product, std::string type, double sum, int timestep); 
    }
    
    else if (userInput.at(0) == "time"){ commandLine.time(currentTime, orderBook.timestep); }
    else if (userInput.at(0) == "step"){ 
        
        if (userInput.size() == 1 && moveSteps(1)){
            commandLine.step(currentTime, orderBook.timestep);
        }
        else if (userInput.size() == 2 && moveSteps(userInput.at(1))){
            commandLine.step(currentTime, orderBook.timestep);
        }
        else{
            std::cout << "CommandLine::step: Error, Unknown parameter: " << userInput.at(1) << std::endl;
        }
    }
    
    // else if (userInput.at(0) == "customCommand"){ customCommand(); }
    else{ std::cout << "\nMerkelMain::processUserOption: Unknown command" << std::endl; }//end else

    //std::cout << "MerkelMain::processUserOption" << std::endl;
}

// for single step use pass "1"
bool MerkelMain::moveSteps(int steps = 1){
    
    if (steps < 0){
        std::cout << "MerkelMain::moveSteps: Error, steps cannot be a negative value" << std::endl;
        return false;
    }
    for (int i = 0; i < steps; ++i){
        currentTime = orderBook.getNextTime(currentTime);
    }
    return true;
}
bool MerkelMain::moveSteps(std::string steps = "1"){
    
    int intSteps;
    try{
        intSteps = std::stoi(steps);
    }
    catch (std::exception& e){
        std::cout << "MerkelMain::moveSteps: Error, cannot convert string \"" << steps << "\" to int"<< std::endl;
        return false;
    }

    if (intSteps < 0){
        std::cout << "MerkelMain::moveSteps: Error, steps cannot be a negative value" << std::endl;
        return false;
    }
    
    for (int i = 0; i < intSteps; ++i){
        currentTime = orderBook.getNextTime(currentTime);
    }
    return true;
}

// gets filtered orders by parameter, calculates the sum over each time step and then returns the average
double MerkelMain::avgAlgo(std::string product, OrderBookType type, int stepCount){

    double stepSum;
    double totalAvg = 0;

    for (int i = 0; i < stepCount; ++i){
        
        stepSum = 0;
        std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
        
        for (OrderBookEntry& e : filteredOrders){
            stepSum += e.price;
        }
        totalAvg += stepSum / filteredOrders.size();
        moveSteps(1);
    }//end for

    totalAvg = totalAvg / stepCount;
    return totalAvg;
}

// calculate the average of max/min bid/ask over a fixed or optional time frame
double MerkelMain::predictAlgo(std::string maxMin, std::string product, OrderBookType type){

    int stepCount = 10;
    double totalSum = 0;
       
    if (maxMin == "max"){
        for (int i = 0; i < stepCount; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getHighPrice(filteredOrders);
            moveSteps(1);
        }//end for
    }
    else if (maxMin == "min"){
        for (int i = 0; i < stepCount; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getLowPrice(filteredOrders);
            moveSteps(1);
        }//end for
    }
    else{
        std::cout << "MerkelMain::predictAlgo: Error, string parameter " << maxMin << std::endl;
    }
    totalSum = totalSum / stepCount;
    return totalSum;
}

// predict algo with optional parameter *<step>
double MerkelMain::predictAlgo(std::string maxMin, std::string product, OrderBookType type, int timeSteps){

    double totalSum = 0;   
    if (maxMin == "max"){
        for (int i = 0; i < timeSteps; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getHighPrice(filteredOrders);
            moveSteps(1);
        }//end for
    }
    else if (maxMin == "min"){
        for (int i = 0; i < timeSteps; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getLowPrice(filteredOrders);
            moveSteps(1);
        }//end for
    }
    else{
        std::cout << "MerkelMain::predictAlgo: Error, string parameter " << maxMin << std::endl;
    }
    totalSum = totalSum / timeSteps;
    return totalSum;
}