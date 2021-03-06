#pragma once

#include <vector>
#include <string>

#include "OrderBookEntry.h"

class CommandLine{
    public:
        CommandLine();
        
        // stores all recognized string words checked during input
        std::vector<std::string> commands{};

        // if string exists in commands, return true
        bool commandExists(std::string str);

        // main print functions of advisorBot, redirected from MerkelMain
        void help();
        void helpCMD(std::string command);
        void prod(std::vector<std::string>& products);
        void min(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders);
        void max(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders);
        void avg(std::string& product, std::string& type, std::string& steps, double& average);
        void predict(std::string& minmax, std::string& product, std::string& type, std::string& steps, double& average);
        void time(std::string& currentTime,  int& timestep);
        void step(std::string& currentTime, int& timestep);
        void devi(std::string& product, std::string& type, std::pair<double, double> deviationPair);
        void exit();

    private:
        

};