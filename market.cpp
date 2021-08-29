#include <boerboel/market.hpp>

#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>

namespace bmi = boost::multi_index;

namespace boerboel
{

class MarketImpl final
{
public:
    bool
    insert_or_update (BestBidAsk const& bba);

    std::shared_ptr<BestBidAsk>
    lookup_symbol (std::string_view symbol) const;

private:
    struct by_symbol {};

    boost::multi_index_container<
        std::shared_ptr<BestBidAsk>,
        bmi::indexed_by<
            bmi::hashed_unique<
                bmi::tag<by_symbol>,
                bmi::member<BestBidAsk, std::string, &BestBidAsk::symbol>
            >
        >
    > bba_map_;
};


bool
MarketImpl::insert_or_update (BestBidAsk const& bba)
{
    auto& symbol_index = bba_map_.get<by_symbol>();

    auto found = symbol_index.find (bba.symbol);
    if (found == symbol_index.end()) {
        return bba_map_.insert (std::make_shared<BestBidAsk>(bba)).second;
    } else {
        return bba_map_.replace (found, std::make_shared<BestBidAsk>(bba));
    }

    return true;
}


std::shared_ptr<BestBidAsk>
MarketImpl::lookup_symbol (std::string_view symbol) const
{
    std::shared_ptr<BestBidAsk> bba;
    auto& symbol_index = bba_map_.get<by_symbol>();

    auto found = symbol_index.find (symbol.data());
    if (found != symbol_index.end()) {
        bba = *found;
    }

    return bba;
}


Market::Market(): impl_(std::make_unique<MarketImpl>())
{
}


Market::~Market()
{
}


void
Market::on_market_data (char const* const data)
{
    boost::iostreams::stream input { boost::iostreams::array_source
                                     (data, std::strlen(data)) };
    BestBidAsk bba;
    while (input >> bba) {
        this->insert_or_update (std::move (bba));
    }
}


void
Market::insert_or_update (BestBidAsk const& bba)
{
    impl_->insert_or_update (bba);
}


std::shared_ptr<BestBidAsk const>
Market::lookup_symbol (std::string_view symbol) const
{
    return impl_->lookup_symbol (symbol);
}

} // namespace boerboe

