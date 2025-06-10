// test_option.cpp
#define BOOST_TEST_MODULE OptionTest
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include <stdexcept>
#include "../libs/Option.hpp"

const double EPSILON = 1e-9;

// test simple functions of class Option
BOOST_AUTO_TEST_SUITE(OptionBasicTests)

BOOST_AUTO_TEST_CASE(test_constructor)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    // Constructor should not throw and object should be created successfully
    BOOST_CHECK(true); // If we reach here, constructor worked
}

BOOST_AUTO_TEST_CASE(test_constructor_negative_times)
{
    // Test with negative inception time
    Option option1(STRADDLE, -1.0, 1.0, 100.0);
    BOOST_CHECK(true);
    
    // Test with negative maturity time
    Option option2(STRADDLE, 0.0, -1.0, 100.0);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_constructor_negative_strike)
{
    Option option(STRADDLE, 0.0, 1.0, -50.0);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

// test Option::Delta
BOOST_AUTO_TEST_SUITE(OptionDeltaTests)

BOOST_AUTO_TEST_CASE(test_delta_straddle_at_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 100.0; // at the money
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    
    // At the money (spot == strike), delta should be 0
    // because (spot - strike) / vol / sqrt(t_mat - t) = 0
    // and 2 * normalCDF(0) - 1 = 2 * 0.5 - 1 = 0
    BOOST_CHECK_CLOSE(delta, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_delta_straddle_in_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 110.0; // in the money
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    
    // For spot > strike, delta should be positive
    // The exact value depends on the normal CDF calculation
    BOOST_CHECK_GE(delta, 0.0);
    BOOST_CHECK_LE(delta, 1.0); // delta should be bounded by 1
}

BOOST_AUTO_TEST_CASE(test_delta_straddle_out_of_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 90.0; // out of the money
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    
    // For spot < strike, delta should be negative
    BOOST_CHECK_LE(delta, 0.0);
    BOOST_CHECK_GE(delta, -1.0); // delta should be bounded by -1
}

BOOST_AUTO_TEST_CASE(test_delta_straddle_near_expiry)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 105.0;
    double t = 0.99; // very close to maturity
    
    double delta = option.Delta(vol, spot, t);
    
    // Near expiry, delta should be more extreme
    BOOST_CHECK_GE(delta, 0.0);
}

BOOST_AUTO_TEST_CASE(test_delta_straddle_high_volatility)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 1.0; // high volatility
    double spot = 110.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    
    // High volatility should moderate the delta
    BOOST_CHECK_GE(delta, 0.0);
    BOOST_CHECK_LE(delta, 1.0);
}

BOOST_AUTO_TEST_CASE(test_delta_invalid_option_type)
{
    // Assuming there's another option type that's not implemented
    // This test may need adjustment based on available option types
    // For now, we'll test with a potentially invalid type cast
    Option option(static_cast<OptionType>(999), 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    BOOST_CHECK_THROW(option.Delta(vol, spot, t), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

// test Option::Gamma
BOOST_AUTO_TEST_SUITE(OptionGammaTests)

BOOST_AUTO_TEST_CASE(test_gamma_straddle_at_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 100.0; // at the money
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // At the money, gamma should be at its maximum (positive)
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_in_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 110.0; // in the money
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Gamma should be positive but lower than at-the-money
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_out_of_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 90.0; // out of the money
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Gamma should be positive but lower than at-the-money
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_near_expiry)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.99; // very close to maturity
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Near expiry, gamma should be higher due to 1/sqrt(t_mat - t) term
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_high_volatility)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 1.0; // high volatility
    double spot = 100.0;
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Higher volatility should reduce gamma
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_invalid_option_type)
{
    Option option(static_cast<OptionType>(999), 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    BOOST_CHECK_THROW(option.Gamma(vol, spot, t), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

// Edge cases and boundary conditions
BOOST_AUTO_TEST_SUITE(OptionEdgeCases)

BOOST_AUTO_TEST_CASE(test_very_small_time_to_maturity)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.999999; // very close to maturity
    
    // This should not throw, but gamma might be very large
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    BOOST_CHECK_CLOSE(delta, 0.0, 1e-3); // should still be close to 0 at the money
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_very_small_volatility)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 1e-6; // very small volatility
    double spot = 105.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // With very small volatility, delta should be close to 1 or -1
    BOOST_CHECK_GE(delta, 0.9);
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_very_large_volatility)
{
    Option option(STRADDLE, 0.0, 1.0, 10.0);
    double vol = 100.0; // very large volatility
    double spot = 12.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // With very large volatility, delta should be close to 0
    BOOST_CHECK_LE(std::abs(delta), 0.1);
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_zero_spot_price)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = 0.0;
    double t = 0.5;
    
    // This might cause division by zero in gamma calculation
    // The behavior depends on the implementation
    double delta = option.Delta(vol, spot, t);
    // Gamma calculation has division by spot, so it might be undefined
    // BOOST_CHECK_THROW(option.Gamma(vol, spot, t), ...); // might throw or return inf
}

BOOST_AUTO_TEST_CASE(test_negative_spot_price)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0);
    double vol = 0.2;
    double spot = -50.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // Should handle negative spot prices
    BOOST_CHECK_LE(delta, 0.0);
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_large_numbers)
{
    Option option(STRADDLE, 0.0, 1.0, 1e6);
    double vol = 0.2;
    double spot = 1.1e6;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // Should handle large numbers without overflow
    BOOST_CHECK_GE(delta, 0.0);
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_SUITE_END()