#include <string>
#include <vector>
#include <map>

#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "OrderBook.h"

OrderBook::OrderBook(std::string filename){
    orders = CSVReader::readCSV(filename);
}

// return vector of all known products (organized by ETH, BTC, DOGE, etc) in dataset
std::vector<std::string> OrderBook::getKnownProducts(){

    std::vector<std::string> products;
    std::map<std::string, bool> prodMap;
    // map each entry product type to true, as method to filter
    for (const OrderBookEntry& e : orders){
        prodMap[e.product] = true;
    }
    //flatten map to vector of strings, e.first calls first element prodMap[key, value]
    for (auto const& e : prodMap){
        products.push_back(e.first);
    }
    return products;
}

// returns vector of OrderBookEntry filtered by passed parameters
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, std::string product, std::string timestamp){

    std::vector<OrderBookEntry> orders_sub;

    for (const OrderBookEntry& e: orders){
        if (e.orderType == type && e.product == product && e.timestamp == timestamp){
            orders_sub.push_back(e);
        }
    }
    return orders_sub;
}

// returns vector of OrderBookEntry filtered by passed parameters (no timestamp filter)
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, std::string product){

    std::vector<OrderBookEntry> orders_sub;

    for (const OrderBookEntry& e: orders){
        if (e.orderType == type && e.product == product){
            orders_sub.push_back(e);
        }
    }
    return orders_sub;
}

std::string OrderBook::getEarlistTime(){

    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp){

    std::string next_timestamp = "";
    
    for (const OrderBookEntry& e: orders){
        if (e.timestamp > timestamp){
            next_timestamp = e.timestamp;
            ++timestep;
            break;
        }
    }//end for

    if (next_timestamp == ""){ // end of orders, loop back to start
        next_timestamp = orders[0].timestamp;
        timestep = 1;
    }
    return next_timestamp;
}


// takes a vector of OrderBookEntry and returns highest/lowest prices in the set
double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders){
    double max = orders[0].price;
    for(OrderBookEntry& e : orders){
        if (e.price > max) max = e.price;
    }
    return max;
}

double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders){
    double min = orders[0].price;
    for(OrderBookEntry& e : orders){
        if (e.price < min) min = e.price;
    }
    return min;
}