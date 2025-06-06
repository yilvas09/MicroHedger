// test_random.cpp
#define BOOST_TEST_MODULE RandomTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include "../libs/Random.hpp"

const double EPSILON = 1e-9;
const double STATISTICAL_TOLERANCE = 3; // 3% relative tolerance for statistical tests

// Test basic constructor and initialization
BOOST_AUTO_TEST_SUITE(RandomBasicTests)

BOOST_AUTO_TEST_CASE(test_constructor_initialization)
{
    int seed = 12345;
    double vol_news = 0.01;
    double order_intensity = 5.0;
    double prob_otype = 0.7;
    double prob_info = 0.3;
    double vol_min = 10.0, vol_max = 100.0;
    double mean_spread = 0.5, vol_spread = 0.1;

    Random rng(seed, vol_news, order_intensity, prob_otype, prob_info, 
               vol_min, vol_max, mean_spread, vol_spread);

    // Constructor should not throw and object should be created successfully
    BOOST_CHECK(true); // If we reach here, constructor succeeded
}

BOOST_AUTO_TEST_CASE(test_multiple_instances_different_seeds)
{
    Random rng1(123, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    Random rng2(456, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);

    // Different seeds should produce different sequences
    double price1 = rng1.GenerateShockedPrice(100.0);
    double price2 = rng2.GenerateShockedPrice(100.0);
    
    // With different seeds, prices should be different (high probability)
    BOOST_CHECK_NE(price1, price2);
}

BOOST_AUTO_TEST_CASE(test_same_seed_reproducibility)
{
    int seed = 12345;
    Random rng1(seed, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    Random rng2(seed, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);

    // Same seed should produce same sequence
    double price1 = rng1.GenerateShockedPrice(100.0);
    double price2 = rng2.GenerateShockedPrice(100.0);
    
    BOOST_CHECK_CLOSE(price1, price2, EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()

// Test GenerateShockedPrice function
BOOST_AUTO_TEST_SUITE(RandomGenerateShockedPriceTests)

BOOST_AUTO_TEST_CASE(test_generate_shocked_price_basic)
{
    Random rng(12345, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    double initial_price = 100.0;
    
    double shocked_price = rng.GenerateShockedPrice(initial_price);
    
    // Shocked price should be different from initial (with high probability)
    // and should be reasonable (not too far from initial)
    BOOST_CHECK_GT(shocked_price, initial_price - 5.0); // not too low
    BOOST_CHECK_LT(shocked_price, initial_price + 5.0); // not too high
}

BOOST_AUTO_TEST_CASE(test_generate_shocked_price_zero_volatility)
{
    Random rng(12345, 0.0, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    double initial_price = 100.0;
    
    double shocked_price = rng.GenerateShockedPrice(initial_price);
    
    // With zero volatility, shocked price should equal initial price
    BOOST_CHECK_CLOSE(shocked_price, initial_price, EPSILON);
}

 // TODO: Resolve errors here!
BOOST_AUTO_TEST_CASE(test_generate_shocked_price_statistical_properties)
{
    Random rng(12345, 0.05, 5.0, 0.7, 0.3, 100.0, 100.0, 0.5, 0.1);
    double initial_price = 10.0;
    int num_samples = 10000;
    
    std::vector<double> price_changes;
    for (int i = 0; i < num_samples; ++i) {
        double shocked_price = rng.GenerateShockedPrice(initial_price);
        price_changes.push_back(shocked_price - initial_price);
    }
    
    // Calculate sample mean and variance
    double mean = 0.0;
    for (double change : price_changes) {
        mean += change;
    }
    mean /= num_samples;
    
    double variance = 0.0;
    for (double change : price_changes) {
        variance += (change - mean) * (change - mean);
    }
    variance /= (num_samples - 1);
    
    // Normal distribution should have mean ≈ 0 and variance ≈ vol_news²
    BOOST_CHECK_SMALL(mean, STATISTICAL_TOLERANCE);
    BOOST_CHECK_CLOSE(variance, 0.05 * 0.05, STATISTICAL_TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()

// Test GenerateNumOrders function
BOOST_AUTO_TEST_SUITE(RandomGenerateNumOrdersTests)

BOOST_AUTO_TEST_CASE(test_generate_num_orders_basic)
{
    Random rng(12345, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    
    int num_orders = rng.GenerateNumOrders();
    
    // Should generate non-negative number of orders
    BOOST_CHECK_GE(num_orders, 0);
}

BOOST_AUTO_TEST_CASE(test_generate_num_orders_zero_intensity)
{
    Random rng(12345, 0.01, 0.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    
    int num_orders = rng.GenerateNumOrders();
    
    // With zero intensity, should generate 0 orders
    BOOST_CHECK_EQUAL(num_orders, 0);
}

BOOST_AUTO_TEST_CASE(test_generate_num_orders_statistical_properties)
{
    double lambda = 3.0; // order intensity
    Random rng(12345, 0.1, lambda, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    int num_samples = 10000;
    
    std::vector<int> order_counts;
    for (int i = 0; i < num_samples; ++i) {
        order_counts.push_back(rng.GenerateNumOrders());
    }
    
    // Calculate sample mean
    double mean = 0.0;
    for (int count : order_counts) {
        mean += count;
    }
    mean /= num_samples;
    
    // Poisson distribution should have mean ≈ lambda
    BOOST_CHECK_CLOSE(mean, lambda, STATISTICAL_TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()

// Test GenerateOrder function
BOOST_AUTO_TEST_SUITE(RandomGenerateOrderTests)

BOOST_AUTO_TEST_CASE(test_generate_order_basic_parameters)
{
    Random rng(12345, 0.01, 5.0, 0.7, 0.3, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    double p_mid = 100.0, p_fund = 99.5;
    
    rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
    
    // Check basic constraints
    BOOST_CHECK(o_type == LIMITORDER || o_type == MARKETORDER);
    BOOST_CHECK_GE(v, 10.0); // volume should be >= vol_min
    BOOST_CHECK_LE(v, 100.0); // volume should be <= vol_max
    BOOST_CHECK(s == 1 || s == -1); // sign should be 1 or -1
}

BOOST_AUTO_TEST_CASE(test_generate_order_market_order_informed)
{
    // Set prob_otype to 0 so we always get market orders
    // Set prob_info to 1 so we always get informed orders
    Random rng(12345, 0.01, 5.0, 0.0, 1.0, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    double p_mid = 100.0, p_fund = 99.0; // fundamental < mid
    
    rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
    
    BOOST_CHECK_EQUAL(o_type, MARKETORDER);
    // Informed agent should sell (s = 1) when p_mid > p_fund
    BOOST_CHECK_EQUAL(s, 1);
}

BOOST_AUTO_TEST_CASE(test_generate_order_market_order_informed_opposite)
{
    Random rng(12345, 0.01, 5.0, 0.0, 1.0, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    double p_mid = 99.0, p_fund = 100.0; // fundamental > mid
    
    rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
    
    BOOST_CHECK_EQUAL(o_type, MARKETORDER);
    // Informed agent should buy (s = -1) when p_mid < p_fund
    BOOST_CHECK_EQUAL(s, -1);
}

BOOST_AUTO_TEST_CASE(test_generate_order_limit_order_informed)
{
    // Set prob_otype to 1 so we always get limit orders
    // Set prob_info to 1 so we always get informed orders
    Random rng(12345, 0.01, 5.0, 1.0, 1.0, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    double p_mid = 100.0, p_fund = 99.0;
    
    rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
    
    BOOST_CHECK_EQUAL(o_type, LIMITORDER);
    // For limit orders, price should be set based on fundamental price
    // Price should be reasonable relative to fundamental price
    BOOST_CHECK_GT(p, p_fund - 2.0);
    BOOST_CHECK_LT(p, p_fund + 2.0);
}

BOOST_AUTO_TEST_CASE(test_generate_order_limit_order_uninformed)
{
    // Set prob_otype to 1 so we always get limit orders
    // Set prob_info to 0 so we always get uninformed orders
    Random rng(12345, 0.01, 5.0, 1.0, 0.0, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    double p_mid = 100.0, p_fund = 99.0;
    
    rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
    
    BOOST_CHECK_EQUAL(o_type, LIMITORDER);
    // For uninformed limit orders, price should be based on mid price
    // Price should be reasonable relative to mid price
    BOOST_CHECK_GT(p, p_mid - 2.0);
    BOOST_CHECK_LT(p, p_mid + 2.0);
}

BOOST_AUTO_TEST_CASE(test_generate_order_volume_distribution)
{
    Random rng(12345, 0.01, 5.0, 0.7, 0.3, 50.0, 150.0, 0.5, 0.1);
    int num_samples = 10000;
    
    std::vector<double> volumes;
    for (int i = 0; i < num_samples; ++i) {
        OrderType o_type;
        double p, v;
        int s;
        double p_mid = 100.0, p_fund = 99.0;
        
        rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
        volumes.push_back(v);
    }
    
    // All volumes should be within [vol_min, vol_max]
    for (double vol : volumes) {
        BOOST_CHECK_GE(vol, 50.0);
        BOOST_CHECK_LE(vol, 150.0);
    }
    
    // Calculate sample mean - should be approximately (vol_min + vol_max) / 2
    double mean = 0.0;
    for (double vol : volumes) {
        mean += vol;
    }
    mean /= num_samples;
    
    BOOST_CHECK_CLOSE(mean, 100.0, STATISTICAL_TOLERANCE); // (50 + 150) / 2 = 100
}

BOOST_AUTO_TEST_CASE(test_generate_order_type_distribution)
{
    double prob_otype = 0.3; // 30% probability of limit orders
    Random rng(12345, 0.01, 5.0, prob_otype, 0.3, 10.0, 100.0, 0.5, 0.1);
    int num_samples = 10000;
    
    int limit_order_count = 0;
    for (int i = 0; i < num_samples; ++i) {
        OrderType o_type;
        double p, v;
        int s;
        double p_mid = 100.0, p_fund = 99.0;
        
        rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
        if (o_type == LIMITORDER) {
            limit_order_count++;
        }
    }
    
    double actual_prob = static_cast<double>(limit_order_count) / num_samples;
    BOOST_CHECK_CLOSE(actual_prob, prob_otype, STATISTICAL_TOLERANCE);
}

BOOST_AUTO_TEST_SUITE_END()

// Integration tests combining multiple functions
BOOST_AUTO_TEST_SUITE(RandomIntegrationTests)

BOOST_AUTO_TEST_CASE(test_complete_simulation_step)
{
    Random rng(12345, 0.05, 3.0, 0.6, 0.4, 20.0, 200.0, 1.0, 0.2);
    
    double initial_price = 100.0;
    double p_mid = 100.5;
    double p_fund = 99.8;
    
    // Generate shocked price
    double shocked_price = rng.GenerateShockedPrice(initial_price);
    BOOST_CHECK_NE(shocked_price, initial_price); // Should be different (high probability)
    
    // Generate number of orders
    int num_orders = rng.GenerateNumOrders();
    BOOST_CHECK_GE(num_orders, 0);
    
    // Generate orders
    for (int i = 0; i < std::min(num_orders, 10); ++i) { // Limit to 10 for testing
        OrderType o_type;
        double p, v;
        int s;
        
        rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
        
        // Verify each order is valid
        BOOST_CHECK(o_type == LIMITORDER || o_type == MARKETORDER);
        BOOST_CHECK_GE(v, 20.0);
        BOOST_CHECK_LE(v, 200.0);
        BOOST_CHECK(s == 1 || s == -1);
        
        if (o_type == LIMITORDER) {
            BOOST_CHECK_GT(p, 0.0); // Price should be positive
        }
    }
}

BOOST_AUTO_TEST_CASE(test_deterministic_sequence_with_fixed_seed)
{
    int seed = 42;
    Random rng1(seed, 0.02, 2.5, 0.5, 0.2, 10.0, 50.0, 0.3, 0.05);
    Random rng2(seed, 0.02, 2.5, 0.5, 0.2, 10.0, 50.0, 0.3, 0.05);
    
    // Generate identical sequences
    for (int i = 0; i < 10; ++i) {
        double price1 = rng1.GenerateShockedPrice(100.0);
        double price2 = rng2.GenerateShockedPrice(100.0);
        BOOST_CHECK_CLOSE(price1, price2, EPSILON);
        
        int orders1 = rng1.GenerateNumOrders();
        int orders2 = rng2.GenerateNumOrders();
        BOOST_CHECK_EQUAL(orders1, orders2);
        
        OrderType o_type1, o_type2;
        double p1, p2, v1, v2;
        int s1, s2;
        
        rng1.GenerateOrder(o_type1, p1, v1, s1, 100.0, 99.5);
        rng2.GenerateOrder(o_type2, p2, v2, s2, 100.0, 99.5);
        
        BOOST_CHECK_EQUAL(o_type1, o_type2);
        BOOST_CHECK_CLOSE(v1, v2, EPSILON);
        BOOST_CHECK_EQUAL(s1, s2);
        if (o_type1 == LIMITORDER) {
            BOOST_CHECK_CLOSE(p1, p2, EPSILON);
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()

// Edge cases and error handling
BOOST_AUTO_TEST_SUITE(RandomEdgeCaseTests)

BOOST_AUTO_TEST_CASE(test_extreme_parameters)
{
    // Test with extreme but valid parameters
    Random rng(12345, 1.0, 0.1, 0.99, 0.01, 1.0, 1000.0, 10.0, 5.0);
    
    // Should not crash
    double price = rng.GenerateShockedPrice(100.0);
    int orders = rng.GenerateNumOrders();
    
    OrderType o_type;
    double p, v;
    int s;
    rng.GenerateOrder(o_type, p, v, s, 100.0, 99.0);
    
    // Basic sanity checks
    BOOST_CHECK_GE(v, 1.0);
    BOOST_CHECK_LE(v, 1000.0);
    BOOST_CHECK(s == 1 || s == -1);
}

BOOST_AUTO_TEST_CASE(test_boundary_probabilities)
{
    // Test with probability = 0
    Random rng1(12345, 0.01, 5.0, 0.0, 0.0, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    
    // With prob_otype = 0, should always generate market orders
    for (int i = 0; i < 10; ++i) {
        rng1.GenerateOrder(o_type, p, v, s, 100.0, 99.0);
        BOOST_CHECK_EQUAL(o_type, MARKETORDER);
    }
    
    // Test with probability = 1
    Random rng2(12345, 0.01, 5.0, 1.0, 1.0, 10.0, 100.0, 0.5, 0.1);
    
    // With prob_otype = 1, should always generate limit orders
    for (int i = 0; i < 10; ++i) {
        rng2.GenerateOrder(o_type, p, v, s, 100.0, 99.0);
        BOOST_CHECK_EQUAL(o_type, LIMITORDER);
    }
}

BOOST_AUTO_TEST_CASE(test_equal_mid_and_fundamental_prices)
{
    Random rng(12345, 0.01, 5.0, 0.0, 1.0, 10.0, 100.0, 0.5, 0.1);
    
    OrderType o_type;
    double p, v;
    int s;
    double p_mid = 100.0, p_fund = 100.0; // Equal prices
    
    // When prices are equal, informed market order sign should be determined by random sign
    rng.GenerateOrder(o_type, p, v, s, p_mid, p_fund);
    
    BOOST_CHECK_EQUAL(o_type, MARKETORDER);
    BOOST_CHECK(s == 1 || s == -1);
}

BOOST_AUTO_TEST_SUITE_END()