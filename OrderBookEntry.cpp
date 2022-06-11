#include <string.h>

#include "OrderBookEntry.h"

OrderBookEntry::OrderBookEntry(double _price, std::string _timestamp, std::string _product, OrderBookType _orderType):
                                price(_price), timestamp(_timestamp), product(_product), orderType(_orderType) {}
                                

// set enum for ask/bid string from parsing a new entry
OrderBookType OrderBookEntry::stringToOrderBookType(std::string s){
    if (s == "ask"){
        return OrderBookType::ask;
    }
    else if (s == "bid"){
        return OrderBookType::bid;
    }
    return OrderBookType::unknown;
}