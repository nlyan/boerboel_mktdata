#include <boerboel/best_bid_ask.hpp>


#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/sequence/comparison/equal_to.hpp>
#include <boost/fusion/include/struct.hpp>

BOOST_FUSION_ADAPT_STRUCT (boerboel::BestBidAsk,
                           symbol,
                           bid_price, ask_price,
                           bid_size, ask_size);

namespace boerboel
{

bool
operator== (BestBidAsk const& a, BestBidAsk const& b) noexcept
{
    return boost::fusion::equal_to (a, b);
}

std::istream&
operator>> (std::istream& is, BestBidAsk& bba)
{
    boost::fusion::for_each (bba, [&is](auto&& field) {
        is >> field;
    });
    return is;
}

std::ostream&
operator<< (std::ostream& os, BestBidAsk const& bba)
{
    boost::fusion::for_each (bba, [&os, first = true](auto&& field) mutable {
        os << (first ? "" : " ") << field;
        first = false;
    });
    return os;
}

} // namespace boerboe

