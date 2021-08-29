#pragma once

#include <boerboel/best_bid_ask.hpp>

#include <memory>
#include <string_view>

namespace boerboel 
{

class MarketImpl;

class Market
{
public:
    Market();
    ~Market() noexcept;

    void
    on_market_data (char const* data);

    void
    insert_or_update (BestBidAsk const& best_bid_ask);

    std::shared_ptr<BestBidAsk const>
    lookup_symbol (std::string_view symbol) const;

private:
    std::unique_ptr<MarketImpl> impl_;
};


} // namespace boerboel
