// test_lob.cpp
#define BOOST_TEST_MODULE LOBTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <cmath>
#include "../libs/LOB.hpp"

const double EPSILON = 1e-9;

// test simple functions of class LOB
BOOST_AUTO_TEST_SUITE(LOBBasicTests)

BOOST_AUTO_TEST_CASE(test_default_constructor) {
    LOB lob;
    
    BOOST_CHECK_CLOSE(lob.bid(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.ask(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), 0.0, EPSILON);
    BOOST_CHECK(lob.oneSideEmpty());
}

BOOST_AUTO_TEST_CASE(test_parameterized_constructor) {
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), 100.0, EPSILON);
    BOOST_CHECK(!lob.oneSideEmpty());
}

BOOST_AUTO_TEST_CASE(test_empty_sides) {
    std::vector<double> ask_prices = {101.0, 102.0};
    std::vector<double> ask_volumes = {100.0, 200.0};
    std::vector<double> empty_prices;
    std::vector<double> empty_volumes;
    
    LOB lob(ask_prices, ask_volumes, empty_prices, empty_volumes);
    
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 0.0, EPSILON);
    BOOST_CHECK(lob.oneSideEmpty());
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::ContainsPrice
BOOST_AUTO_TEST_SUITE(LOBContainsPriceTests)

BOOST_AUTO_TEST_CASE(test_contains_price_basic) {
    std::vector<double> ask_prices = {101.0, 102.0};
    std::vector<double> ask_volumes = {100.0, 200.0};
    std::vector<double> bid_prices = {99.0, 98.0};
    std::vector<double> bid_volumes = {150.0, 100.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    BOOST_CHECK_EQUAL(lob.ContainsPrice(101.0), 1);  // ask side
    BOOST_CHECK_EQUAL(lob.ContainsPrice(99.0), -1);  // bid side
    BOOST_CHECK_EQUAL(lob.ContainsPrice(100.0), 0);  // between bid-ask
    BOOST_CHECK_EQUAL(lob.ContainsPrice(105.0), 0);  // not found
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::PriceLocation
BOOST_AUTO_TEST_SUITE(LOBPriceLocationTests)

BOOST_AUTO_TEST_CASE(test_price_location_asks) {
    std::vector<double> ask_prices = {101.0, 102.0, 104.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 100.0), 0);   // lowest ask price
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 101.5), 1);   // between 101 and 102
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 103.0), 2);   // between 102 and 104
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 105.0), 3);   // highest ask price
}

BOOST_AUTO_TEST_CASE(test_price_location_bids) {
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {100.0};
    std::vector<double> bid_prices = {97.0, 98.0, 99.0};
    std::vector<double> bid_volumes = {100.0, 200.0, 150.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 96.0), 0);   // lowest bid price
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 97.5), 1);   // between 97 and 98
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 98.5), 2);   // between 98 and 99
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 100.0), 3);  // highest bid price
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::AddLimitOrder
BOOST_AUTO_TEST_SUITE(LOBAddLimitOrderTests)

BOOST_AUTO_TEST_CASE(test_add_new_ask_order) {
    LOB lob;
    
    lob.AddLimitOrder(1, 101.0, 100.0);  // add ask/sell order
    
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_EQUAL(lob.ContainsPrice(101.0), 1);
}

BOOST_AUTO_TEST_CASE(test_add_new_bid_order) {
    LOB lob;
    
    lob.AddLimitOrder(-1, 99.0, 150.0);  // add bid/buy order
    
    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);
    BOOST_CHECK_EQUAL(lob.ContainsPrice(99.0), -1);
}

BOOST_AUTO_TEST_CASE(test_add_multiple_orders_same_side) {
    LOB lob;
    
    lob.AddLimitOrder(1, 102.0, 100.0);  // ask
    lob.AddLimitOrder(1, 101.0, 150.0);  // ask, lower price
    lob.AddLimitOrder(1, 103.0, 200.0);  // ask, higher price
    
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON); // lowest ask should be 101
}

BOOST_AUTO_TEST_CASE(test_add_volume_to_existing_price) {
    LOB lob;
    
    lob.AddLimitOrder(1, 101.0, 100.0);  // initial ask order
    lob.AddLimitOrder(1, 101.0, 50.0);   // adding volume at the same price
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::AbsorbMarketOrder
BOOST_AUTO_TEST_SUITE(LOBAbsorbMarketOrderTests)

BOOST_AUTO_TEST_CASE(test_market_buy_order_partial_execution) {
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    std::vector<Bar> executed_orders;
    double volume = 150.0;  // buy 150 shares
    
    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, -1);
    
    // should execute 100 at 101.0 and 50 at 102.0
    BOOST_CHECK_EQUAL(executed_orders.size(), 2);
    BOOST_CHECK_CLOSE(executed_orders[0].Price(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(executed_orders[0].Volume(), 100.0, EPSILON);
    BOOST_CHECK_CLOSE(executed_orders[1].Price(), 102.0, EPSILON);
    BOOST_CHECK_CLOSE(executed_orders[1].Volume(), 50.0, EPSILON);
    
    // VWAP = (100*101 + 50*102) / 150
    double expected_vwap = (100.0 * 101.0 + 50.0 * 102.0) / 150.0;
    BOOST_CHECK_CLOSE(vwap, expected_vwap, EPSILON);
    
    BOOST_CHECK_CLOSE(volume, 0.0, EPSILON); // fully executed
}

BOOST_AUTO_TEST_CASE(test_market_sell_order_full_execution) {
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {100.0};
    std::vector<double> bid_prices = {99.0, 98.0};
    std::vector<double> bid_volumes = {150.0, 200.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    std::vector<Bar> executed_orders;
    double volume = 100.0;  // sell 100 shares
    
    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, 1);
    
    // should execute 100 at price 99.0
    BOOST_CHECK_EQUAL(executed_orders.size(), 1);
    BOOST_CHECK_CLOSE(executed_orders[0].Price(), 99.0, EPSILON);
    BOOST_CHECK_CLOSE(executed_orders[0].Volume(), 100.0, EPSILON);
    
    BOOST_CHECK_CLOSE(vwap, 99.0, EPSILON);
    BOOST_CHECK_CLOSE(volume, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_market_order_insufficient_liquidity) {
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {50.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};
    
    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    std::vector<Bar> executed_orders;
    double volume = 100.0;  // buy 100 shares, but only 50 shares are available
    
    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, -1);
    
    BOOST_CHECK_EQUAL(executed_orders.size(), 1);
    BOOST_CHECK_CLOSE(executed_orders[0].Volume(), 50.0, EPSILON);
    BOOST_CHECK_CLOSE(volume, 50.0, EPSILON); // 50 shares not executed
}

BOOST_AUTO_TEST_CASE(test_market_order_wrong_sign) {
    LOB lob;
    std::vector<Bar> executed_orders;
    double volume = 100.0;
    
    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, 0); // wrong sign, error msg
    
    BOOST_CHECK_CLOSE(vwap, 0.0, EPSILON);
    BOOST_CHECK_EQUAL(executed_orders.size(), 0);
}

BOOST_AUTO_TEST_SUITE_END()

// integrated testing under various scenarios
BOOST_AUTO_TEST_SUITE(LOBIntegrationTests)

struct LOBFixture {
    LOBFixture() {
        // create a standard LOB for testing
        std::vector<double> ask_prices = {101.0, 102.0, 103.0, 105.0};
        std::vector<double> ask_volumes = {100.0, 200.0, 150.0, 300.0};
        std::vector<double> bid_prices = {99.0, 98.0, 97.0, 95.0};
        std::vector<double> bid_volumes = {150.0, 100.0, 200.0, 250.0};
        
        lob = new LOB(ask_prices, ask_volumes, bid_prices, bid_volumes);
    }
    
    ~LOBFixture() {
        delete lob;
    }
    
    LOB* lob;
};

BOOST_FIXTURE_TEST_SUITE(LOBWithFixture, LOBFixture)

BOOST_AUTO_TEST_CASE(test_complex_market_order_scenario) {
    std::vector<Bar> executed_orders;
    double volume = 250.0;  // large order
    
    double vwap = lob->AbsorbMarketOrder(executed_orders, volume, -1);
    
    // should execute 100 at 101 and 150 at 102 = 250
    BOOST_CHECK_EQUAL(executed_orders.size(), 2);
    
    double expected_vwap = (100.0 * 101.0 + 150.0 * 102.0) / 250.0;
    BOOST_CHECK_CLOSE(vwap, expected_vwap, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_spread_calculation) {
    double spread = lob->ask() - lob->bid();
    BOOST_CHECK_CLOSE(spread, 2.0, EPSILON); // 101 - 99 = 2
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

// performance and stress-tests
BOOST_AUTO_TEST_SUITE(LOBPerformanceTests)

BOOST_AUTO_TEST_CASE(test_large_lob_creation) {
    // large LOB
    std::vector<double> ask_prices, ask_volumes, bid_prices, bid_volumes;
    
    for (int i = 0; i < 1000; ++i) {
        ask_prices.push_back(101.0 + i * 0.01);
        ask_volumes.push_back(100.0 + i);
        bid_prices.push_back(100.0 - i * 0.01);
        bid_volumes.push_back(100.0 + i);
    }
    
    LOB large_lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    
    BOOST_CHECK_CLOSE(large_lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(large_lob.bid(), 100.0, EPSILON);
    BOOST_CHECK(!large_lob.oneSideEmpty());
}

BOOST_AUTO_TEST_SUITE_END()