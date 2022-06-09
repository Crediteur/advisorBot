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

        void help();
        void helpCMD(std::string command);
        void prod(std::vector<std::string>& products);
        void min(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders);
        void max(std::string& product, std::string& type, std::vector<OrderBookEntry>& filteredOrders);
        void avg();
        void predict();
        void time(std::string& currentTime,  int& timestep);
        void step(std::string& currentTime, int& timestep);
        //void customCommand();

    private:
        void avgAlgo();
        void predictAlgo();

};