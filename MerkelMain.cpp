#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds/milliseconds [DO NOT USE PERIODS]
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>

#include "MerkelMain.h"
#include "CSVReader.h"
#include "OrderBookEntry.h"
#include "CommandLine.h"

MerkelMain::MerkelMain(){}

void MerkelMain::init(){

    currentTime = orderBook.getEarlistTime();
    
    // welcome message
    std::cout << "Current time: " << currentTime << std::endl;
    std::this_thread::sleep_for (std::chrono::milliseconds(800));
    std::cout << "\nWelcome to advisorBot! Explore the latest cryptocurrency market trends here!" << std::endl;

    // loop of parsing input and main menu
    while (loopStatus){
        printMenu();
        getUserOption();
        // processUserOption, update global variable userInput
        processUserOption(); //new class?
    }
}

void MerkelMain::printMenu(){
    
        std::this_thread::sleep_for (std::chrono::milliseconds(800));
    std::cout << "\nEnter a command, or \"help\" for a list of commands" << std::endl;
}

//get user input by parsing line, does a quick string check
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
}

// return true if first word of string is valid, otherwise false 
bool MerkelMain::checkUserOption(std::string& userWord){
    if (commandLine.commandExists(userWord)){
        return true;
    }
    std::cout << "MerkelMain::checkUserOption: Unknown command, try again" << std::endl;
    return false;
}

// call function matching userInput string at index 0, passing appropriate parameters
void MerkelMain::processUserOption(){

    // print help menu, first check if second string is a command
    if(userInput.at(0) == "help"){

        if(userInput.size() == 2 && userInput.at(1) != "help" 
        && commandLine.commandExists(userInput.at(1))){
            commandLine.helpCMD(userInput.at(1)); 
        }
        else { commandLine.help(); };
    }
    // list all product types, pass products from orderbook first
    else if (userInput.at(0) == "prod"){ 
        // if refactoring, move this call to commandLine
        std::vector<std::string> products = orderBook.getKnownProducts();
        commandLine.prod(products); 
    }

    // returns minimum price order filtered by "min <product> <bid/ask>" of current time frame
    else if (userInput.at(0) == "min"){

        if (userInput.size() < 3){
            std::cout << "CommandLine::min: Invalid parameters, format: min <product> <bid/ask>" 
            << std::endl;
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
                std::cout << "Unknown product: " << userInput.at(1) << " or Type: " 
                << userInput.at(2) << std::endl;
                return;
            }
            commandLine.min(userInput.at(1), userInput.at(2), filteredOrders);
        }//end else
    }

    // returns maximum price order filtered by "min <product> <bid/ask>" of current time frame
    else if (userInput.at(0) == "max"){ 

        if (userInput.size() < 3){
            std::cout << "CommandLine::max: Invalid parameters, format: max <product> <bid/ask>" 
            << std::endl;
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
                std::cout << "Unknown Product: " << userInput.at(1) << ", or Type: " << userInput.at(2) 
                << std::endl;
                return;
            }
            commandLine.max(userInput.at(1), userInput.at(2), filteredOrders);
        }//end else
    }

    // calculates average of product price after moving timesteps 
    // by format "avg <product> <bid/ask> <steps>"
    else if (userInput.at(0) == "avg"){

        if (userInput.size() < 4){
            std::cout << "CommandLine::avg: Invalid parameters, format: avg <product> <bid/ask> <steps>" 
            << std::endl;
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
    }

    // calculate the average of max/min bid/ask over a fixed timestep 
    // by format "predict <max/min> <product> <bid/ask> *<steps>"
    else if (userInput.at(0) == "predict"){ 
        
        std::string stepString = "10";
        int stepCount = 10; //set default steps to 10
        if (userInput.size() < 4){
            std::cout << "CommandLine::predict: Invalid parameters, format: predict <max/min> <product> <bid/ask> *<steps>"
            << std::endl;
            return;
        }
        // check and try to convert optional parameter *<steps>
        else if (userInput.size() > 4){
            try{
                stepCount = std::stoi(userInput.at(4));
                stepString = userInput.at(4);
                if (stepCount < 0 ){ 
                    std::cout << "MerkelMain::predict: Error, steps cannot be negative" << std::endl;
                    return;
                }
            }
            catch (std::exception& e){
                stepCount = 10;
            }
        }

        //max/min = 1, product = 2, bid/ask = 3, *steps = 4
        OrderBookType type;
        std::vector<OrderBookEntry> filteredOrders;
        try{
            type = OrderBookEntry::stringToOrderBookType(userInput.at(3));
            filteredOrders = orderBook.getOrders(type, userInput.at(2), currentTime);
            if (filteredOrders.size() == 0 || type == OrderBookType::unknown
                || !(userInput.at(1) == "max" || userInput.at(1) == "min")){ 
                    //alternative: || !commandLine.commandExists(userInput.at(1))){
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
    }
    
    else if (userInput.at(0) == "time"){ 
        std::cout << "line: " << orderBook.csvline << std::endl; 
        
        commandLine.time(currentTime, orderBook.timestep, orderBook.csvline); 
    }

    else if (userInput.at(0) == "step"){ 
        
        if (userInput.size() == 1 && moveStep(1)){
            commandLine.step(currentTime, orderBook.timestep);
        }
        else if (userInput.size() == 2 && moveStep(userInput.at(1))){
            commandLine.step(currentTime, orderBook.timestep);
        }
        else{
            std::cout << "CommandLine::step: Error, Unknown parameter: " 
            << userInput.at(1) << std::endl;
        }
    }
    
    // calculates standard deviation of product over last 10 time frames
    // format: devi <product> <bid/ask>
    else if (userInput.at(0) == "devi"){ 

        if (userInput.size() < 3){
            std::cout << "CommandLine::avg: Invalid parameters, format: devi <product> <bid/ask>" 
            << std::endl;
            return;
        }

        OrderBookType type;
        std::vector<OrderBookEntry> filteredOrders;
        try{
            type = OrderBookEntry::stringToOrderBookType(userInput.at(2));
            filteredOrders = orderBook.getOrders(type, userInput.at(1), currentTime);
            if (filteredOrders.size() == 0 || type == OrderBookType::unknown){
                throw std::exception{};
            }
        }
        catch(std::exception& e){
            std::cout << "Unknown Product: " << userInput.at(1) << ", or Type: " 
            << userInput.at(2) << std::endl;
            return;
        }
        
        // pass to algorithm function to calculate standard deviation
        std::pair<double, double> deviationPair = standardDevi(userInput.at(1), type);

        //print results
        commandLine.devi(userInput.at(1), userInput.at(2), deviationPair);
    }

    else if (userInput.at(0) == "exit"){
        loopStatus = 0;
        commandLine.exit();
    }
    else{ std::cout << "\nMerkelMain::processUserOption: Unknown command" << std::endl; }
}//end funct processUserOption

// for single step use pass "1"
bool MerkelMain::moveStep(int steps = 1){
    
    if (steps < 0){
        std::cout << "MerkelMain::moveStep: Error, steps cannot be a negative value" << std::endl;
        return false;
    }
    for (int i = 0; i < steps; ++i){
        currentTime = orderBook.getNextTime(currentTime);
    }
    return true;
}
bool MerkelMain::moveStep(std::string& steps){
    
    int intSteps;
    try{
        intSteps = std::stoi(steps);
    }
    catch (std::exception& e){
        std::cout << "MerkelMain::moveStep: Error, cannot convert string \"" << steps << "\" to int"<< std::endl;
        return false;
    }

    if (intSteps < 0){
        std::cout << "MerkelMain::moveStep: Error, steps cannot be a negative value" << std::endl;
        return false;
    }
    
    for (int i = 0; i < intSteps; ++i){
        currentTime = orderBook.getNextTime(currentTime);
    }
    return true;
}

// gets filtered orders by parameter, calculates the sum over each time step and then returns the average
double MerkelMain::avgAlgo(std::string& product, OrderBookType& type, int stepCount){

    double stepSum;
    double totalAvg = 0;

    for (int i = 0; i < stepCount; ++i){
        
        stepSum = 0;
        std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
        
        for (const OrderBookEntry& e : filteredOrders){
            stepSum += e.price;
        }
        totalAvg += stepSum / filteredOrders.size();
        moveStep(1);
    }//end for

    totalAvg = totalAvg / stepCount;
    return totalAvg;
}

// calculate the average of max/min bid/ask over a fixed or optional time frame
double MerkelMain::predictAlgo(std::string& maxMin, std::string& product, OrderBookType& type){

    int stepCount = 10;
    double totalSum = 0;
       
    if (maxMin == "max"){
        for (int i = 0; i < stepCount; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getHighPrice(filteredOrders);
            moveStep(1);
        }//end for
    }
    else if (maxMin == "min"){
        for (int i = 0; i < stepCount; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getLowPrice(filteredOrders);
            moveStep(1);
        }//end for
    }
    else{
        std::cout << "MerkelMain::predictAlgo: Error, string parameter " << maxMin << std::endl;
    }
    totalSum = totalSum / stepCount;
    return totalSum;
}

// predict algo with optional parameter *<step>
double MerkelMain::predictAlgo(std::string& maxMin, std::string& product, OrderBookType& type, int timeSteps){

    double totalSum = 0;   
    if (maxMin == "max"){
        for (int i = 0; i < timeSteps; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getHighPrice(filteredOrders);
            moveStep(1);
        }//end for
    }
    else if (maxMin == "min"){
        for (int i = 0; i < timeSteps; ++i){
            
            std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
            totalSum += OrderBook::getLowPrice(filteredOrders);
            moveStep(1);
        }//end for
    }
    else{
        std::cout << "MerkelMain::predictAlgo: Error, string parameter " << maxMin << std::endl;
    }
    totalSum = totalSum / timeSteps;
    return totalSum;
}

// calculates standard deviation of current and last 10 frames
std::pair<double, double> MerkelMain::standardDevi(std::string& product, OrderBookType& type){

    // stepMean and total track calculation of individual points along each time frame
    int stepCount = 10;
    std::vector<double> stepMean;
    double total;
    double muStep;
    double muTotal;

    // calculate mean for each time frame as a point
    for (int i = 0; i < stepCount; ++i){
        muStep = 0;
        std::vector<OrderBookEntry> filteredOrders = orderBook.getOrders(type, product, currentTime);
        for (const OrderBookEntry& e : filteredOrders){
            muStep += e.price;
        }
        muTotal += muStep / filteredOrders.size();
        stepMean.push_back(muStep / filteredOrders.size());
        moveStep(1);
    }
    muTotal = muTotal / stepCount;
    // sum up individual step by (mean - mu)
    for (double& num: stepMean){
        total += pow(num - muTotal, 2);
    }
    total = sqrt(total / stepCount);

    // calculate standard deviation for stand alone time frame
    double currentTotal = 0;
    double currentMu = 0;
    std::vector<OrderBookEntry> currentOrders = orderBook.getOrders(type, product, currentTime);
    for (const OrderBookEntry& e: currentOrders){
        currentMu += e.price;
    }
    currentMu = currentMu / currentOrders.size();

    for (const OrderBookEntry& e: currentOrders){
        currentTotal += pow(e.price - currentMu, 2);
    }
    currentTotal = sqrt (currentTotal / currentOrders.size());
    
    //return as pair
    return std::make_pair (currentTotal, total); 
}
