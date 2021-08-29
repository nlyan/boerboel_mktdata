#include <boerboel/mktdata.hpp>

#include <catch2/catch_test_macros.hpp>
#include <mutex>
#include <sstream>

using namespace boerboel;

char const* const VOD_BBA_TEST_STR = "VOD 122.82 122.88 500 420";

char const* const MARKET_TEST_STR = R"(VOD 122.82 122.88 500 420
BARC 185.56 185.64 100 8
GSK 1472.60 1473.00 80 90
)";

char const* const MARKET_UPDATE_STR = "VOD 122.71 123.44 100 20";

SCENARIO( "Best bid/asks can be serialized and deserialized", "[BestBidAsk]" ) {
    GIVEN( "A default construct BestBidAsk" ) {
        BestBidAsk bba;

        THEN ( "The symbol is empty" ) {
            REQUIRE (bba.symbol.empty());
        }

        THEN ( "The numeric fields are all zero" ) {
            REQUIRE (bba.ask_price == 0);
            REQUIRE (bba.bid_price == 0);
            REQUIRE (bba.ask_size == 0);
            REQUIRE (bba.bid_size == 0);
        }

        WHEN ( "We stream in a a valid serialization ") {
            std::stringstream iss (VOD_BBA_TEST_STR);
            iss >> bba;

            THEN( "The stream is not failed or bad" ) {
                REQUIRE (!iss.fail());
                REQUIRE (!iss.bad());
            }

            THEN( "The stream is fully consumed" ) {
                REQUIRE (iss.eof());
            }

            THEN( "The values are as we expect" ) {
                REQUIRE (bba.symbol == "VOD");
                REQUIRE (bba.bid_price == Decimal ("122.82"));
                REQUIRE (bba.ask_price == Decimal ("122.88"));
                REQUIRE (bba.bid_size == 500);
                REQUIRE (bba.ask_size == 420);
            }

            THEN( "The serialized output is the same as the input" ) {
                std::stringstream oss;
                oss << bba;
                REQUIRE (oss.str() == VOD_BBA_TEST_STR);
            }
        }
    }
}

SCENARIO( "Market data can be loaded and updated", "[Market]" ) {
    GIVEN( "An empty market" ) {
        Market market;

        THEN( "Vodafone cannot be found" ) {
            auto instr = market.lookup_symbol ("VOD");
            REQUIRE (instr == nullptr);
        }

        WHEN( "Vodafone is inserted" ) {
            BestBidAsk const instr_input {"VOD", Decimal("122.82"), Decimal("122.88"),
                                               Decimal("500"), Decimal("420") };
            market.insert_or_update (instr_input);

            THEN( "Vodafone can then be found" ) {
                auto instr = market.lookup_symbol ("VOD");
                REQUIRE (instr != nullptr);
            }

            THEN( "Vodafone is stored as expected" ) {
                auto instr = market.lookup_symbol ("VOD");
                REQUIRE (instr != nullptr);
                REQUIRE (*instr == instr_input);
                REQUIRE (instr->symbol == "VOD");
                REQUIRE (instr->bid_price == Decimal ("122.82"));
                REQUIRE (instr->ask_price == Decimal ("122.88"));
                REQUIRE (instr->bid_size == 500);
                REQUIRE (instr->ask_size == 420);
            }

            AND_WHEN ("Vodafone is updated") {
                BestBidAsk const instr_input {"VOD", Decimal("122.71"), Decimal("123.44"),
                                                Decimal("100"), Decimal("20") };
                market.insert_or_update (instr_input);

                THEN( "Vodafone can still be found" ) {
                    auto instr = market.lookup_symbol ("VOD");
                    REQUIRE (instr != nullptr);
                }

                THEN( "The new data is stored as expected" ) {
                    auto instr = market.lookup_symbol ("VOD");
                    REQUIRE (instr != nullptr);
                    REQUIRE (*instr == instr_input);
                    REQUIRE (instr->symbol == "VOD");
                    REQUIRE (instr->bid_price == Decimal ("122.71"));
                    REQUIRE (instr->ask_price == Decimal ("123.44"));
                    REQUIRE (instr->bid_size == 100);
                    REQUIRE (instr->ask_size == 20);
                }
            }
        }

        WHEN( "The market is loaded" ) {
            market.on_market_data (MARKET_TEST_STR);

            THEN( "Vodafone can then be found" ) {
                auto instr = market.lookup_symbol ("VOD");
                REQUIRE (instr != nullptr);
            }

            THEN( "Vodafone is stored as expected" ) {
                auto instr = market.lookup_symbol ("VOD");
                REQUIRE (instr != nullptr);
                REQUIRE (instr->symbol == "VOD");
                REQUIRE (instr->bid_price == Decimal ("122.82"));
                REQUIRE (instr->ask_price == Decimal ("122.88"));
                REQUIRE (instr->bid_size == 500);
                REQUIRE (instr->ask_size == 420);
            }

            THEN( "Barclays is stored as expected" ) {
                auto instr = market.lookup_symbol ("BARC");
                REQUIRE (instr != nullptr);
                REQUIRE (instr->symbol == "BARC");
                REQUIRE (instr->bid_price == Decimal ("185.56"));
                REQUIRE (instr->ask_price == Decimal ("185.64"));
                REQUIRE (instr->bid_size == 100);
                REQUIRE (instr->ask_size == 8);
            }

            THEN( "Glaxo is stored as expected" ) {
                auto instr = market.lookup_symbol ("GSK");
                REQUIRE (instr != nullptr);
                REQUIRE (instr->symbol == "GSK");
                REQUIRE (instr->bid_price == Decimal ("1472.60"));
                REQUIRE (instr->ask_price == Decimal ("1473.00"));
                REQUIRE (instr->bid_size == 80);
                REQUIRE (instr->ask_size == 90);
            }

            AND_WHEN ("The market is updated") {
                market.on_market_data (MARKET_UPDATE_STR);

                THEN( "Barclays is unchanged" ) {
                    auto instr = market.lookup_symbol ("BARC");
                    REQUIRE (instr != nullptr);
                    REQUIRE (instr->symbol == "BARC");
                    REQUIRE (instr->bid_price == Decimal ("185.56"));
                    REQUIRE (instr->ask_price == Decimal ("185.64"));
                    REQUIRE (instr->bid_size == 100);
                    REQUIRE (instr->ask_size == 8);
                }

                THEN( "Glaxo is unchanged" ) {
                    auto instr = market.lookup_symbol ("GSK");
                    REQUIRE (instr != nullptr);
                    REQUIRE (instr->symbol == "GSK");
                    REQUIRE (instr->bid_price == Decimal ("1472.60"));
                    REQUIRE (instr->ask_price == Decimal ("1473.00"));
                    REQUIRE (instr->bid_size == 80);
                    REQUIRE (instr->ask_size == 90);
                }

                THEN( "Vodafone has been updated" ) {
                    auto instr = market.lookup_symbol ("VOD");
                    REQUIRE (instr != nullptr);
                    REQUIRE (instr->symbol == "VOD");
                    REQUIRE (instr->bid_price == Decimal ("122.71"));
                    REQUIRE (instr->ask_price == Decimal ("123.44"));
                    REQUIRE (instr->bid_size == 100);
                    REQUIRE (instr->ask_size == 20);
                }
            }
        }
    }
}
