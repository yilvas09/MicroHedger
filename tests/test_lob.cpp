// test_lob.cpp
#define BOOST_TEST_MODULE LOBTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <cmath>
#include "../libs/LOB.hpp"

const double EPSILON = 1e-9;

// test simple functions of class LOB
BOOST_AUTO_TEST_SUITE(LOBBasicTests)

BOOST_AUTO_TEST_CASE(test_default_constructor)
{
    LOB lob;

    BOOST_CHECK_CLOSE(lob.bid(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.ask(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), 0.0, EPSILON);
    BOOST_CHECK(lob.oneSideEmpty());
    BOOST_CHECK_THROW(lob.getBarAt(1, 0), std::invalid_argument);
    BOOST_CHECK_THROW(lob.getBarAt(-1, -1), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_parameterized_constructor)
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), 100.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(1, 1), 200.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(-1, -3), 200.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), 100.0, EPSILON);
    BOOST_CHECK(!lob.oneSideEmpty());
    BOOST_CHECK_THROW(lob.getBarAt(1, 3), std::invalid_argument);
    BOOST_CHECK_THROW(lob.getBarAt(-1, -4), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_empty_sides)
{
    std::vector<double> ask_prices = {101.0, 102.0};
    std::vector<double> ask_volumes = {100.0, 200.0};
    std::vector<double> bid_prices; // no bid orders
    std::vector<double> bid_volumes;

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 0.0, EPSILON);
    BOOST_CHECK(lob.oneSideEmpty());
    BOOST_CHECK_NO_THROW(lob.getBarAt(1, 1));
    BOOST_CHECK_THROW(lob.getBarAt(-1, 0), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::ContainsPrice
BOOST_AUTO_TEST_SUITE(LOBContainsPriceTests)

BOOST_AUTO_TEST_CASE(test_contains_price_basic)
{
    std::vector<double> ask_prices = {101.0, 102.0};
    std::vector<double> ask_volumes = {100.0, 200.0};
    std::vector<double> bid_prices = {99.0, 98.0};
    std::vector<double> bid_volumes = {150.0, 100.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    BOOST_CHECK_EQUAL(lob.ContainsPrice(101.0), 1); // ask side
    BOOST_CHECK_EQUAL(lob.ContainsPrice(99.0), -1); // bid side
    BOOST_CHECK_EQUAL(lob.ContainsPrice(100.0), 0); // between bid-ask
    BOOST_CHECK_EQUAL(lob.ContainsPrice(105.0), 0); // not found
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::PriceLocation
BOOST_AUTO_TEST_SUITE(LOBPriceLocationTests)

BOOST_AUTO_TEST_CASE(test_price_location_asks)
{
    std::vector<double> ask_prices = {101.0, 102.0, 104.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 100.0), 0); // lowest ask price
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 101.5), 1); // between 101 and 102
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 103.0), 2); // between 102 and 104
    BOOST_CHECK_EQUAL(lob.PriceLocation(1, 105.0), 3); // highest ask price
}

BOOST_AUTO_TEST_CASE(test_price_location_bids)
{
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {100.0};
    std::vector<double> bid_prices = {97.0, 98.0, 99.0};
    std::vector<double> bid_volumes = {100.0, 200.0, 150.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 96.0), 0);  // lowest bid price
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 97.5), 1);  // between 97 and 98
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 98.5), 2);  // between 98 and 99
    BOOST_CHECK_EQUAL(lob.PriceLocation(-1, 100.0), 3); // highest bid price
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::AddLimitOrder
BOOST_AUTO_TEST_SUITE(LOBAddLimitOrderTests)

BOOST_AUTO_TEST_CASE(test_add_new_ask_order)
{
    LOB lob;

    lob.AddLimitOrder(1, 101.0, 100.0); // add ask/sell order

    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_EQUAL(lob.ContainsPrice(101.0), 1);
}

BOOST_AUTO_TEST_CASE(test_add_new_bid_order)
{
    LOB lob;

    lob.AddLimitOrder(-1, 99.0, 150.0); // add bid/buy order

    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);
    BOOST_CHECK_EQUAL(lob.ContainsPrice(99.0), -1);
}

BOOST_AUTO_TEST_CASE(test_add_multiple_orders_same_side)
{
    LOB lob;

    lob.AddLimitOrder(1, 102.0, 100.0); // ask
    lob.AddLimitOrder(1, 101.0, 150.0); // ask, lower price
    lob.AddLimitOrder(1, 103.0, 200.0); // ask, higher price

    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON); // lowest ask should be 101
}

BOOST_AUTO_TEST_CASE(test_add_volume_to_existing_price)
{
    LOB lob;

    lob.AddLimitOrder(1, 101.0, 100.0); // initial ask order
    lob.AddLimitOrder(1, 101.0, 50.0);  // adding volume at the same price

    BOOST_CHECK_CLOSE(lob.getPriceAt(1, 0), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(1, 0), 150.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_add_orders_other_side)
{
    LOB lob;

    lob.AddLimitOrder(1, 102.0, 100.0); // ask
    lob.AddLimitOrder(1, 101.0, 150.0); // ask, lower price
    lob.AddLimitOrder(1, 103.0, 200.0); // ask, higher price

    lob.AddLimitOrder(-1, 101.0, 50.0); // buy order at ask price
    BOOST_CHECK_CLOSE(lob.getVolumeAt(1, lob.PriceLocation(1, 101.0)), 100.0, EPSILON);

    lob.AddLimitOrder(-1, 101.0, 100.0);          // buy order at ask price
    BOOST_CHECK_CLOSE(lob.ask(), 102.0, EPSILON); // the 101 bar is gone

    // cannot post orders at a price higher than ask price
    BOOST_CHECK_THROW(lob.AddLimitOrder(-1, 103.0, 50.0), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::AbsorbMarketOrder
BOOST_AUTO_TEST_SUITE(LOBAbsorbMarketOrderTests)

BOOST_AUTO_TEST_CASE(test_market_buy_order_partial_execution)
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    std::vector<Bar> executed_orders;
    double volume = 150.0; // buy 150 shares

    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, -1);

    // should execute sell orders 100 at 101.0 and 50 at 102.0
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

BOOST_AUTO_TEST_CASE(test_market_sell_order_full_execution)
{
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {100.0};
    std::vector<double> bid_prices = {99.0, 98.0};
    std::vector<double> bid_volumes = {150.0, 200.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    std::vector<Bar> executed_orders;
    double volume = 100.0; // sell 100 shares

    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, 1);

    // should execute buy orders 100 at price 99.0
    BOOST_CHECK_EQUAL(executed_orders.size(), 1);
    BOOST_CHECK_CLOSE(executed_orders[0].Price(), 99.0, EPSILON);
    BOOST_CHECK_CLOSE(executed_orders[0].Volume(), -100.0, EPSILON);

    BOOST_CHECK_CLOSE(vwap, 99.0, EPSILON);
    BOOST_CHECK_CLOSE(volume, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_market_order_insufficient_liquidity)
{
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {50.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    std::vector<Bar> executed_orders;
    double volume = 100.0; // buy 100 shares, but only 50 shares are available

    double vwap = lob.AbsorbMarketOrder(executed_orders, volume, -1);

    BOOST_CHECK_EQUAL(executed_orders.size(), 1);
    BOOST_CHECK_CLOSE(executed_orders[0].Volume(), 50.0, EPSILON); // should execute 50 shares sell orders
    BOOST_CHECK_CLOSE(volume, 50.0, EPSILON); // 50 shares not executed
}

BOOST_AUTO_TEST_CASE(test_market_order_wrong_sign)
{
    LOB lob;
    std::vector<Bar> executed_orders;
    double volume = 100.0;

    BOOST_CHECK_THROW(lob.AbsorbMarketOrder(executed_orders, volume, 0), std::invalid_argument); // wrong sign, error msg
    BOOST_CHECK_NO_THROW(lob.AbsorbMarketOrder(executed_orders, volume, 1));                     // wrong sign, error msg
}

BOOST_AUTO_TEST_SUITE_END()

// test LOB::DecayOrders
BOOST_AUTO_TEST_SUITE(LOBDecayOrdersTests)

BOOST_AUTO_TEST_CASE(test_decay_orders_basic)
{
    // Setup: Create LOB with some orders using constructor
    // Ask prices: 101.0, 102.0 with volumes: 1000, 500
    // Bid prices: 99.0, 98.0 with volumes: 800, 600
    std::vector<double> ask_prices = {101.0, 102.0};
    std::vector<double> ask_volumes = {1000.0, 500.0};
    std::vector<double> bid_prices = {99.0, 98.0};
    std::vector<double> bid_volumes = {800.0, 600.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    // Verify initial state
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), 100.0, EPSILON);

    double ini_ask_vol = lob.getVolumeAt(1, lob.PriceLocation(1, lob.ask()));
    double ini_bid_vol = lob.getVolumeAt(-1, lob.PriceLocation(-1, lob.bid()));

    // Apply decay with coefficient 0.01
    double d_coef = 0.01;
    lob.DecayOrders(d_coef);

    // Verify price levels remain the same
    BOOST_CHECK_CLOSE(lob.mid(), 100.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);

    // Verify that decay occurred, i.e. volumes changed
    double expected_decay_factor_ask = exp(-d_coef * pow(lob.mid() - lob.ask(), 2));
    double expected_decay_factor_bid = exp(-d_coef * pow(lob.mid() - lob.bid(), 2));
    double actual_decay_factor_ask = lob.getVolumeAt(1, lob.PriceLocation(1, lob.ask())) / ini_ask_vol;
    double actual_decay_factor_bid = lob.getVolumeAt(-1, lob.PriceLocation(-1, lob.bid())) / ini_bid_vol;
    BOOST_CHECK_CLOSE(actual_decay_factor_ask, expected_decay_factor_ask, EPSILON);
    BOOST_CHECK_CLOSE(actual_decay_factor_bid, expected_decay_factor_bid, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_decay_orders_zero_coefficient)
{
    // Setup LOB
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {1000.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {800.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    // Store initial values
    double initial_ask = lob.ask();
    double initial_bid = lob.bid();
    double initial_mid = lob.mid();
    double initial_ask_vol = lob.getVolumeAt(1, lob.PriceLocation(1, initial_ask));
    double initial_bid_vol = lob.getVolumeAt(-1, lob.PriceLocation(-1, initial_bid));

    // Apply zero decay coefficient (no decay should occur) TODO: check volume same
    lob.DecayOrders(0.0);

    // Verify nothing changed (both price and volume)
    BOOST_CHECK_CLOSE(lob.ask(), initial_ask, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), initial_bid, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), initial_mid, EPSILON);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(1, 0), initial_ask_vol, EPSILON);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(-1, -1), initial_bid_vol, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_decay_orders_symmetric_decay)
{
    // Test symmetric LOB - orders equidistant from mid should decay equally
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {1000.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {1000.0}; // Same initial volume

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    double p_mid = lob.mid(); // Should be 100.0
    double d_coef = 0.01;

    // Both orders are 1.0 away from mid, so should have same decay factor
    double expected_decay_factor = exp(-d_coef * pow(1.0, 2));

    lob.DecayOrders(d_coef);

    double actual_decay_factor_ask = lob.getVolumeAt(1, lob.PriceLocation(1, lob.ask())) / ask_volumes[0];
    double actual_decay_factor_bid = lob.getVolumeAt(-1, lob.PriceLocation(-1, lob.bid())) / bid_volumes[0];

    // After decay, spread should remain the same since decay is symmetric
    BOOST_CHECK_CLOSE(lob.mid(), p_mid, EPSILON);
    BOOST_CHECK_CLOSE(actual_decay_factor_ask, expected_decay_factor, EPSILON);
    BOOST_CHECK_CLOSE(actual_decay_factor_bid, expected_decay_factor, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_decay_orders_empty_lob)
{
    // Test decay on empty LOB (should not crash)
    LOB lob; // Default constructor creates empty LOB

    BOOST_CHECK_NO_THROW(lob.DecayOrders(0.01));

    // Verify LOB remains empty
    BOOST_CHECK(lob.oneSideEmpty());
}

BOOST_AUTO_TEST_CASE(test_decay_orders_one_side_empty)
{
    // Test with only asks (no bids)
    std::vector<double> ask_prices = {101.0, 102.0};
    std::vector<double> ask_volumes = {1000.0, 500.0};
    std::vector<double> bid_prices = {};
    std::vector<double> bid_volumes = {};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    BOOST_CHECK(lob.oneSideEmpty());

    // Should not crash even with one side empty
    BOOST_CHECK_NO_THROW(lob.DecayOrders(0.01));
}

BOOST_AUTO_TEST_CASE(test_decay_orders_negative_coefficient)
{
    // Test with negative coefficient (volumes should increase)
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {1000.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {800.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    double initial_mid = lob.mid();
    double ini_ask_vol = lob.getVolumeAt(1, lob.PriceLocation(1, lob.ask()));
    double ini_bid_vol = lob.getVolumeAt(-1, lob.PriceLocation(-1, lob.bid()));

    // Apply negative decay coefficient
    double d_coef = -0.01;
    lob.DecayOrders(d_coef);

    // With negative coefficient, decay factor > 1, so volumes should increase
    // Mid should remain the same as prices don't change
    BOOST_CHECK_CLOSE(lob.mid(), initial_mid, EPSILON);

    // Verify that decay occurred, i.e. volumes changed
    double expected_decay_factor_ask = exp(-d_coef * pow(lob.mid() - lob.ask(), 2));
    double expected_decay_factor_bid = exp(-d_coef * pow(lob.mid() - lob.bid(), 2));
    double actual_decay_factor_ask = lob.getVolumeAt(1, lob.PriceLocation(1, lob.ask())) / ini_ask_vol;
    double actual_decay_factor_bid = lob.getVolumeAt(-1, lob.PriceLocation(-1, lob.bid())) / ini_bid_vol;
    BOOST_CHECK_CLOSE(actual_decay_factor_ask, expected_decay_factor_ask, EPSILON);
    BOOST_CHECK_CLOSE(actual_decay_factor_bid, expected_decay_factor_bid, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_decay_orders_multiple_levels)
{
    // Test with multiple price levels to verify all are processed
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {1000.0, 500.0, 200.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {800.0, 600.0, 300.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    double initial_mid = lob.mid();

    lob.DecayOrders(0.01);

    // Verify best bid/ask remain at same price levels
    BOOST_CHECK_CLOSE(lob.ask(), 101.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.bid(), 99.0, EPSILON);
    BOOST_CHECK_CLOSE(lob.mid(), initial_mid, EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(LOBAbsorbGeneralOrderTests)

BOOST_AUTO_TEST_CASE(test_absorb_general_order_mo)
{
    std::vector<double> ask_prices = {101.0};
    std::vector<double> ask_volumes = {50.0};
    std::vector<double> bid_prices = {99.0};
    std::vector<double> bid_volumes = {150.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);

    std::vector<Bar> executed_orders;
    OrderType o_type = MARKETORDER;
    double volume = 100.0; // buy 100 shares, but only 50 shares are available

    executed_orders = lob.AbsorbGeneralOrder(o_type, 0.0, volume, -1);

    BOOST_CHECK_EQUAL(executed_orders.size(), 1);
    BOOST_CHECK_CLOSE(executed_orders[0].Volume(), 50.0, EPSILON); // 50 shares sell exe.
}

// TODO: invalid inpur, s=0

BOOST_AUTO_TEST_CASE(test_absorb_general_order_same_side_lo_price_exists)
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    OrderType o_type = LIMITORDER;

    double p_new = 102.0, v_new = 50.0;
    int s_new = 1;
    std::vector<Bar> eos = lob.AbsorbGeneralOrder(o_type, p_new, v_new, s_new);

    BOOST_CHECK_EQUAL(eos.size(), 0);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(s_new, lob.PriceLocation(s_new, p_new)), 250.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_absorb_general_order_same_side_lo_new_price)
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    OrderType o_type = LIMITORDER;

    double p_new = 100.0, v_new = 50.0, s_new = -1;
    std::vector<Bar> eos = lob.AbsorbGeneralOrder(o_type, p_new, v_new, s_new);

    BOOST_CHECK_EQUAL(eos.size(), 0);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(s_new, lob.PriceLocation(s_new, p_new)), 50.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_absorb_general_order_other_side_lo_fully_exe)
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    OrderType o_type = LIMITORDER;

    double p_new = 101.0, v_new = 50.0, s_new = -1;
    std::vector<Bar> eos = lob.AbsorbGeneralOrder(o_type, p_new, v_new, s_new);

    BOOST_CHECK_EQUAL(eos.size(), 1);
    BOOST_CHECK_CLOSE(eos[0].Volume(), v_new, EPSILON);
    // the new buy LO is absorbed by existing sell LO and fully executed
    BOOST_CHECK_EQUAL(lob.ContainsPrice(p_new), 1);
    BOOST_CHECK_CLOSE(lob.getVolumeAt(-s_new, lob.PriceLocation(-s_new, p_new)), 50.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_absorb_general_order_other_side_lo_part_exe)
{
    std::vector<double> ask_prices = {101.0, 102.0, 103.0};
    std::vector<double> ask_volumes = {100.0, 200.0, 150.0};
    std::vector<double> bid_prices = {99.0, 98.0, 97.0};
    std::vector<double> bid_volumes = {150.0, 100.0, 200.0};

    LOB lob(ask_prices, ask_volumes, bid_prices, bid_volumes);
    OrderType o_type = LIMITORDER;

    double p_new = 99.0, v_new = 250.0, s_new = 1;
    std::vector<Bar> eos = lob.AbsorbGeneralOrder(o_type, p_new, v_new, s_new);

    BOOST_CHECK_EQUAL(eos.size(), 1);
    BOOST_CHECK_CLOSE(eos[0].Volume(), 150.0, EPSILON);
    // the new buy LO is absorbed by existing sell LO and partly executed
    // outstanding volumes are posted on the other side
    BOOST_CHECK_EQUAL(lob.ContainsPrice(p_new), 1);
}

BOOST_AUTO_TEST_SUITE_END()

// integrated testing under various scenarios
BOOST_AUTO_TEST_SUITE(LOBIntegrationTests)

struct LOBFixture
{
    LOBFixture()
    {
        // create a standard LOB for testing
        std::vector<double> ask_prices = {101.0, 102.0, 103.0, 105.0};
        std::vector<double> ask_volumes = {100.0, 200.0, 150.0, 300.0};
        std::vector<double> bid_prices = {99.0, 98.0, 97.0, 95.0};
        std::vector<double> bid_volumes = {150.0, 100.0, 200.0, 250.0};

        lob = new LOB(ask_prices, ask_volumes, bid_prices, bid_volumes);
    }

    ~LOBFixture()
    {
        delete lob;
    }

    LOB *lob;
};

BOOST_FIXTURE_TEST_SUITE(LOBWithFixture, LOBFixture)

BOOST_AUTO_TEST_CASE(test_complex_market_order_scenario)
{
    std::vector<Bar> executed_orders;
    double volume = 250.0; // large order

    double vwap = lob->AbsorbMarketOrder(executed_orders, volume, -1);

    // should execute 100 at 101 and 150 at 102 = 250
    BOOST_CHECK_EQUAL(executed_orders.size(), 2);

    double expected_vwap = (100.0 * 101.0 + 150.0 * 102.0) / 250.0;
    BOOST_CHECK_CLOSE(vwap, expected_vwap, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_spread_calculation)
{
    double spread = lob->ask() - lob->bid();
    BOOST_CHECK_CLOSE(spread, 2.0, EPSILON); // 101 - 99 = 2
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

// performance and stress-tests
BOOST_AUTO_TEST_SUITE(LOBPerformanceTests)

BOOST_AUTO_TEST_CASE(test_large_lob_creation)
{
    // large LOB
    std::vector<double> ask_prices, ask_volumes, bid_prices, bid_volumes;

    for (int i = 0; i < 1000; ++i)
    {
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