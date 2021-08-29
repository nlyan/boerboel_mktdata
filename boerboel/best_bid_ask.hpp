#pragma once

#include <boerboel/decimal.hpp>

#include <iosfwd>
#include <string>

namespace boerboel 
{

struct BestBidAsk {
public:
    std::string symbol;
    Decimal     bid_price;
    Decimal     ask_price;
    Decimal     bid_size;
    Decimal     ask_size;
};

bool operator== (BestBidAsk const&, BestBidAsk const&) noexcept;

std::istream& operator>> (std::istream& is, BestBidAsk&);
std::ostream& operator<< (std::ostream& os, BestBidAsk const&);

} // namespace boerboel
