#include <boerboel/mktdata.hpp>

#include <boost/range/as_literal.hpp>
#include <iostream>

namespace boerboel {

void
on_market_data (char const* const data)
{
    auto input = boost::as_literal (data);
    std::cout << size (input) << "\n";
}

} // namespace boerboel
