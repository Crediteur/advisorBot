#pragma once

#include <string>
#include <vector>

#include "CSVReader.h"
#include "OrderBookEntry.h"

class OrderBook{
    public:
        // default initialization, passes file path to CSVReader
        OrderBook(std::string filename);

        // return vector of all known products (organized by ETH, BTC, DOGE, etc) in dataset
        std::vector<std::string> getKnownProducts();

        // returns vector of OrderBookEntry filtered by passed parameters
        std::vector<OrderBookEntry> getOrders(OrderBookType type, std::string product, std::string timestamp);
        std::vector<OrderBookEntry> getOrders(OrderBookType type, std::string product);


        std::string getEarlistTime();

        // moves to next time frame, also increments timestep var
        std::string getNextTime(std::string timestamp);
        int timestep = 1;

        /** TODO:: std::string getPrevTime(std::string timestamp); */

        // takes a vector of OrderBookEntry and returns highest/lowest prices in the set
        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);

    private:
        // contains all OrderBookEntry parsed from the CSV reader
        std::vector<OrderBookEntry> orders;
};