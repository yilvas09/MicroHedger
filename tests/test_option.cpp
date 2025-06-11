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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    // Constructor should not throw and object should be created successfully
    BOOST_CHECK(true); // If we reach here, constructor worked
}

BOOST_AUTO_TEST_CASE(test_default_constructor)
{
    Option option;
    // Constructor should not throw and object should be created successfully
    BOOST_CHECK(true); // If we reach here, constructor worked
}

BOOST_AUTO_TEST_CASE(test_constructor_negative_times)
{
    // Test with negative inception time
    Option option1(STRADDLE, -1.0, 1.0, 100.0, 1.0);
    BOOST_CHECK(true);
    
    // Test with negative maturity time
    Option option2(STRADDLE, 0.0, -1.0, 100.0, 1.0);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_constructor_negative_strike)
{
    Option option(STRADDLE, 0.0, 1.0, -50.0, 1.0);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_constructor_positive_position)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 2.5);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_constructor_negative_position)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, -1.5);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_constructor_zero_position)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 0.0);
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

// test Option::Delta
BOOST_AUTO_TEST_SUITE(OptionDeltaTests)

BOOST_AUTO_TEST_CASE(test_delta_straddle_at_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 0.2;
    double spot = 105.0;
    double t = 0.99; // very close to maturity
    
    double delta = option.Delta(vol, spot, t);
    
    // Near expiry, delta should be more extreme
    BOOST_CHECK_GE(delta, 0.0);
}

BOOST_AUTO_TEST_CASE(test_delta_straddle_high_volatility)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 1.0; // high volatility
    double spot = 110.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    
    // High volatility should moderate the delta
    BOOST_CHECK_GE(delta, 0.0);
    BOOST_CHECK_LE(delta, 1.0);
}

BOOST_AUTO_TEST_CASE(test_delta_position_scaling)
{
    double vol = 0.2;
    double spot = 110.0;
    double t = 0.5;
    
    // Test with different position sizes
    Option option1(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    Option option2(STRADDLE, 0.0, 1.0, 100.0, 2.0);
    Option option3(STRADDLE, 0.0, 1.0, 100.0, -1.0);
    Option option4(STRADDLE, 0.0, 1.0, 100.0, 0.0);
    
    double delta1 = option1.Delta(vol, spot, t);
    double delta2 = option2.Delta(vol, spot, t);
    double delta3 = option3.Delta(vol, spot, t);
    double delta4 = option4.Delta(vol, spot, t);
    
    // Delta should scale linearly with position
    BOOST_CHECK_CLOSE(delta2, 2.0 * delta1, EPSILON);
    BOOST_CHECK_CLOSE(delta3, -delta1, EPSILON);
    BOOST_CHECK_CLOSE(delta4, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_delta_fractional_position)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 0.5);
    double vol = 0.2;
    double spot = 110.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    
    // With 0.5 position, delta should be half of unit position
    Option unit_option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double unit_delta = unit_option.Delta(vol, spot, t);
    
    BOOST_CHECK_CLOSE(delta, 0.5 * unit_delta, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_delta_invalid_option_type)
{
    // Assuming there's another option type that's not implemented
    // This test may need adjustment based on available option types
    // For now, we'll test with a potentially invalid type cast
    Option option(static_cast<OptionType>(999), 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 0.2;
    double spot = 100.0; // at the money
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // At the money, gamma should be at its maximum (positive)
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_in_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 0.2;
    double spot = 110.0; // in the money
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Gamma should be positive but lower than at-the-money
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_out_of_the_money)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 0.2;
    double spot = 90.0; // out of the money
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Gamma should be positive but lower than at-the-money
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_near_expiry)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.99; // very close to maturity
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Near expiry, gamma should be higher due to 1/sqrt(t_mat - t) term
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_straddle_high_volatility)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double vol = 1.0; // high volatility
    double spot = 100.0;
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // Higher volatility should reduce gamma
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_gamma_position_scaling)
{
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    // Test with different position sizes
    Option option1(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    Option option2(STRADDLE, 0.0, 1.0, 100.0, 3.0);
    Option option3(STRADDLE, 0.0, 1.0, 100.0, -2.0);
    Option option4(STRADDLE, 0.0, 1.0, 100.0, 0.0);
    
    double gamma1 = option1.Gamma(vol, spot, t);
    double gamma2 = option2.Gamma(vol, spot, t);
    double gamma3 = option3.Gamma(vol, spot, t);
    double gamma4 = option4.Gamma(vol, spot, t);
    
    // Gamma should scale linearly with position
    BOOST_CHECK_CLOSE(gamma2, 3.0 * gamma1, EPSILON);
    BOOST_CHECK_CLOSE(gamma3, -2.0 * gamma1, EPSILON);
    BOOST_CHECK_CLOSE(gamma4, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_gamma_fractional_position)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 0.25);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    double gamma = option.Gamma(vol, spot, t);
    
    // With 0.25 position, gamma should be quarter of unit position
    Option unit_option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    double unit_gamma = unit_option.Gamma(vol, spot, t);
    
    BOOST_CHECK_CLOSE(gamma, 0.25 * unit_gamma, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_gamma_negative_position)
{
    Option option_long(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    Option option_short(STRADDLE, 0.0, 1.0, 100.0, -1.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    double gamma_long = option_long.Gamma(vol, spot, t);
    double gamma_short = option_short.Gamma(vol, spot, t);
    
    // Short position should have negative gamma
    BOOST_CHECK_GE(gamma_long, 0.0);
    BOOST_CHECK_LE(gamma_short, 0.0);
    BOOST_CHECK_CLOSE(gamma_short, -gamma_long, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_gamma_invalid_option_type)
{
    Option option(static_cast<OptionType>(999), 0.0, 1.0, 100.0, 1.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    BOOST_CHECK_THROW(option.Gamma(vol, spot, t), std::invalid_argument);
}

BOOST_AUTO_TEST_SUITE_END()

// test position effects across Delta and Gamma
BOOST_AUTO_TEST_SUITE(OptionPositionTests)

BOOST_AUTO_TEST_CASE(test_position_consistency_delta_gamma)
{
    double vol = 0.2;
    double spot = 105.0;
    double t = 0.5;
    double position = 2.5;
    
    // Create options with same parameters but different positions
    Option unit_option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
    Option scaled_option(STRADDLE, 0.0, 1.0, 100.0, position);
    
    double unit_delta = unit_option.Delta(vol, spot, t);
    double unit_gamma = unit_option.Gamma(vol, spot, t);
    double scaled_delta = scaled_option.Delta(vol, spot, t);
    double scaled_gamma = scaled_option.Gamma(vol, spot, t);
    
    // Both delta and gamma should scale by the same factor
    BOOST_CHECK_CLOSE(scaled_delta, position * unit_delta, EPSILON);
    BOOST_CHECK_CLOSE(scaled_gamma, position * unit_gamma, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_large_position_size)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1000.0);
    double vol = 0.2;
    double spot = 100.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // At the money, delta should still be 0 regardless of position size
    BOOST_CHECK_CLOSE(delta, 0.0, EPSILON);
    // Gamma should be large and positive
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_very_small_position)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1e-6);
    double vol = 0.2;
    double spot = 110.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // Both should be very small but still positive (for this scenario)
    BOOST_CHECK_GE(delta, 0.0);
    BOOST_CHECK_LE(delta, 1e-5);
    BOOST_CHECK_GE(gamma, 0.0);
    BOOST_CHECK_LE(gamma, 1e-5);
}

BOOST_AUTO_TEST_SUITE_END()

// Edge cases and boundary conditions
BOOST_AUTO_TEST_SUITE(OptionEdgeCases)

BOOST_AUTO_TEST_CASE(test_very_small_time_to_maturity)
{
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 10.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 100.0, 1.0);
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
    Option option(STRADDLE, 0.0, 1.0, 1e6, 1.0);
    double vol = 0.2;
    double spot = 1.1e6;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // Should handle large numbers without overflow
    BOOST_CHECK_GE(delta, 0.0);
    BOOST_CHECK_GE(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_position_with_edge_cases)
{
    // Test position scaling with edge case parameters
    Option option(STRADDLE, 0.0, 1.0, 100.0, -0.5);
    double vol = 1e-6; // very small volatility
    double spot = 105.0;
    double t = 0.5;
    
    double delta = option.Delta(vol, spot, t);
    double gamma = option.Gamma(vol, spot, t);
    
    // With negative position, both should be negative
    BOOST_CHECK_LE(delta, 0.0);
    BOOST_CHECK_LE(gamma, 0.0);
}

BOOST_AUTO_TEST_SUITE_END()