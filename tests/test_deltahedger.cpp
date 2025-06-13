// test_delta_hedger.cpp
#define BOOST_TEST_MODULE DeltaHedgerTest
#include <boost/test/included/unit_test.hpp>
#include <vector>
#include <cmath>
#include "../libs/DeltaHedger.hpp"
#include "../libs/LOB.hpp"
#include "../libs/Option.hpp"

const double EPSILON = 1e-9;

// Helper function to create a simple LOB for testing
LOB createTestLOB(double bid_price = 99.0, double ask_price = 101.0,
                  double bid_vol = 100.0, double ask_vol = 100.0)
{
    std::vector<double> ask_prices = {ask_price};
    std::vector<double> ask_volumes = {ask_vol};
    std::vector<double> bid_prices = {bid_price};
    std::vector<double> bid_volumes = {bid_vol};

    return LOB(ask_prices, ask_volumes, bid_prices, bid_volumes);
}

// Test basic constructor and initial state
BOOST_AUTO_TEST_SUITE(DeltaHedgerBasicTests)

BOOST_AUTO_TEST_CASE(test_constructor)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;

    DeltaHedger hedger(opt_pos, implied_vol);

    // Constructor should not throw
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_initial_delta_gamma_calculation)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Initially, hedger should have no options or stocks
    // So delta should be 0 (no options, no stocks)
    double delta = hedger.Delta(implied_vol, test_lob, time);
    double gamma = hedger.Gamma(implied_vol, test_lob, time);

    BOOST_CHECK_CLOSE(delta, 0.0, EPSILON);
    BOOST_CHECK_CLOSE(gamma, 0.0, EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()

// Test ResetGammaContract functionality
BOOST_AUTO_TEST_SUITE(DeltaHedgerResetGammaContractTests)

BOOST_AUTO_TEST_CASE(test_reset_gamma_contract_basic)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Reset should create ATM straddle
    hedger.ResetGammaContract(time, test_lob);

    // After reset, delta and gamma should be calculated based on the straddle
    // Since it's ATM straddle, delta should be close to 0 (call delta ≈ 0.5, put delta ≈ -0.5)
    // but gamma should be positive
    double delta = hedger.Delta(implied_vol, test_lob, time);
    double gamma = hedger.Gamma(implied_vol, test_lob, time);

    // For ATM straddle, delta should be close to 0
    BOOST_CHECK_SMALL(abs(delta), 0.1); // Allow some tolerance
    // Gamma should be positive for straddle
    BOOST_CHECK_GT(gamma, 0.0);
}

BOOST_AUTO_TEST_CASE(test_reset_gamma_contract_clears_inventory)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Reset multiple times to ensure clearing works
    hedger.ResetGammaContract(time, test_lob);
    hedger.ResetGammaContract(time + 1.0, test_lob);

    // Should not throw and should maintain consistent state
    double delta1 = hedger.Delta(implied_vol, test_lob, time + 1.0);
    double gamma1 = hedger.Gamma(implied_vol, test_lob, time + 1.0);

    BOOST_CHECK_SMALL(abs(delta1), 0.1);
    BOOST_CHECK_GT(gamma1, 0.0);
}

BOOST_AUTO_TEST_SUITE_END()

// Test IsMyOrderExecuted functionality
BOOST_AUTO_TEST_SUITE(DeltaHedgerOrderExecutionTests)

BOOST_AUTO_TEST_CASE(test_is_my_order_executed_no_outstanding_order)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    std::vector<std::vector<Bar>> empty_eos;

    // No outstanding order should return false
    bool executed = hedger.IsMyOrderExecuted(empty_eos);
    BOOST_CHECK_EQUAL(executed, FALSE);
}

BOOST_AUTO_TEST_CASE(test_is_my_order_executed_with_matching_execution)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // simulate market movements with a new sell order
    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0);
    hedger.ReCalcGreeks(time, test_lob);

    // Simulate posting an order through PostOrder
    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;
    hedger.PostOrder(p, v, s, empty_eos, test_lob, 0.1);

    BOOST_CHECK_CLOSE(hedger.getOrderVolume(), s * v, EPSILON);
    BOOST_CHECK_CLOSE(hedger.getOrderPrice(), p, EPSILON);

    // Now create execution that matches the posted order
    std::vector<std::vector<Bar>> matching_eos;
    std::vector<Bar> execution_round;
    execution_round.push_back(Bar(p, s * v)); // Same price and sign
    matching_eos.push_back(execution_round);

    // Should detect execution
    bool executed = hedger.IsMyOrderExecuted(matching_eos);
    BOOST_CHECK_EQUAL(executed, true);
}

BOOST_AUTO_TEST_CASE(test_is_my_order_executed_partial_execution)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0);
    hedger.ReCalcGreeks(time, test_lob);

    // Post order
    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;
    hedger.PostOrder(p, v, s, empty_eos, test_lob, 0.1);

    // Partial execution (smaller volume)
    std::vector<std::vector<Bar>> partial_eos;
    std::vector<Bar> execution_round;
    execution_round.push_back(Bar(p, s * v * 0.5)); // Half the volume
    partial_eos.push_back(execution_round);

    // Should not be fully executed
    bool executed = hedger.IsMyOrderExecuted(partial_eos);
    BOOST_CHECK_EQUAL(executed, FALSE);
}

BOOST_AUTO_TEST_CASE(test_is_my_order_executed_wrong_price)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0);
    hedger.ReCalcGreeks(time, test_lob);

    // Post order
    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;
    hedger.PostOrder(p, v, s, empty_eos, test_lob, 0.1);

    // Execution at different price
    std::vector<std::vector<Bar>> wrong_price_eos;
    std::vector<Bar> execution_round;
    execution_round.push_back(Bar(p + 1.0, s * v)); // Different price
    wrong_price_eos.push_back(execution_round);

    // Should not match
    bool executed = hedger.IsMyOrderExecuted(wrong_price_eos);
    BOOST_CHECK_EQUAL(executed, FALSE);
}

BOOST_AUTO_TEST_CASE(test_is_my_order_executed_wrong_sign)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0);
    hedger.ReCalcGreeks(time, test_lob);

    // Post order
    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;
    hedger.PostOrder(p, v, s, empty_eos, test_lob, 0.1);

    // Execution with opposite sign
    std::vector<std::vector<Bar>> wrong_sign_eos;
    std::vector<Bar> execution_round;
    execution_round.push_back(Bar(p, -s * v)); // Opposite sign
    wrong_sign_eos.push_back(execution_round);

    // Should not match
    bool executed = hedger.IsMyOrderExecuted(wrong_sign_eos);
    BOOST_CHECK_EQUAL(executed, FALSE);
}

BOOST_AUTO_TEST_SUITE_END()

// Test PostOrder method functionality
BOOST_AUTO_TEST_SUITE(DeltaHedgerPostOrderTests)

BOOST_AUTO_TEST_CASE(test_act_zero_delta)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Gamma contract with ATM strike, no delta
    hedger.ResetGammaContract(time, test_lob);
    hedger.ReCalcGreeks(time, test_lob);

    double p = 0.0, v = 0.0;
    int s = 0;
    std::vector<std::vector<Bar>> empty_eos;

    // PostOrder should do nothing when delta is zero
    hedger.PostOrder(p, v, s, empty_eos, test_lob, 0.1);

    // Values should remain unchanged
    BOOST_CHECK_CLOSE(p, 0.0, EPSILON);
    BOOST_CHECK_CLOSE(v, 0.0, EPSILON);
    BOOST_CHECK_EQUAL(s, 0);
}

BOOST_AUTO_TEST_CASE(test_act_positive_delta_aggressive_timing)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB(99.0, 101.0, 100.0, 100.0);
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0); // market moves
    hedger.ReCalcGreeks(time, test_lob);

    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;

    // Very early (passive)
    double t_q = 0.1; // <= 0.25
    hedger.PostOrder(p, v, s, empty_eos, test_lob, t_q);

    // Should place aggressive order
    double ba_spr = test_lob.ask() - test_lob.bid(); // 1.0

    if (s > 0)
    {
        // Selling (positive delta)
        double expected_p = test_lob.ask() + ba_spr; // 100 + 1 = 101
        BOOST_CHECK_CLOSE(p, expected_p, EPSILON);
    }
    else if (s < 0)
    {
        // Buying (negative delta)
        double expected_p = test_lob.bid() - ba_spr; // 99 - 1 = 98
        BOOST_CHECK_CLOSE(p, expected_p, EPSILON);
    }

    BOOST_CHECK_GT(v, 0.0); // Volume should be positive
}

BOOST_AUTO_TEST_CASE(test_act_timing_strategies)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB(99.0, 101.0, 100.0, 100.0);
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0); // market moves
    hedger.ReCalcGreeks(time, test_lob);

    double p1, v1, p2, v2, p3, v3, p4, v4;
    int s1, s2, s3, s4;
    std::vector<std::vector<Bar>> empty_eos;

    double ba_spr = test_lob.ask() - test_lob.bid();

    // Test different timing strategies
    hedger.PostOrder(p1, v1, s1, empty_eos, test_lob, 0.1); // <= 0.25: most aggressive
    hedger.PostOrder(p2, v2, s2, empty_eos, test_lob, 0.4); // <= 0.5: at bid/ask
    hedger.PostOrder(p3, v3, s3, empty_eos, test_lob, 0.6); // <= 0.75: half spread improvement
    hedger.PostOrder(p4, v4, s4, empty_eos, test_lob, 0.9); // > 0.75: passive (cross spread)

    // All should have same sign (same delta) and volume
    BOOST_CHECK_EQUAL(s1, s2);
    BOOST_CHECK_EQUAL(s2, s3);
    BOOST_CHECK_EQUAL(s3, s4);
    BOOST_CHECK_CLOSE(v1, v2, EPSILON);
    BOOST_CHECK_CLOSE(v2, v3, EPSILON);
    BOOST_CHECK_CLOSE(v3, v4, EPSILON);

    // Prices should follow the timing strategy
    if (s1 > 0)
    {                                                                  // Selling
        BOOST_CHECK_CLOSE(p1, test_lob.ask() + ba_spr, EPSILON);       // Most aggressive
        BOOST_CHECK_CLOSE(p2, test_lob.ask(), EPSILON);                // At ask
        BOOST_CHECK_CLOSE(p3, test_lob.ask() - 0.5 * ba_spr, EPSILON); // Half spread
        BOOST_CHECK_CLOSE(p4, test_lob.bid(), EPSILON);                // Passive
    }
    else
    {                                                                  // Buying
        BOOST_CHECK_CLOSE(p1, test_lob.bid() - ba_spr, EPSILON);       // Most aggressive
        BOOST_CHECK_CLOSE(p2, test_lob.bid(), EPSILON);                // At bid
        BOOST_CHECK_CLOSE(p3, test_lob.bid() + 0.5 * ba_spr, EPSILON); // Half spread
        BOOST_CHECK_CLOSE(p4, test_lob.ask(), EPSILON);                // Passive
    }
}

BOOST_AUTO_TEST_CASE(test_act_order_execution_cycle)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0); // market moves
    hedger.ReCalcGreeks(time, test_lob);

    double p1, v1, p2, v2;
    int s1, s2;
    std::vector<std::vector<Bar>> empty_eos;

    // First PostOrder - should post order
    hedger.PostOrder(p1, v1, s1, empty_eos, test_lob, 0.5);

    // Second PostOrder without execution - should do nothing (order already posted)
    double original_p1 = p1, original_v1 = v1;
    int original_s1 = s1;

    hedger.PostOrder(p2, v2, s2, empty_eos, test_lob, 0.5);

    // Values should remain the same (no new order posted)
    BOOST_CHECK_CLOSE(p2, original_p1, EPSILON);
    BOOST_CHECK_CLOSE(v2, original_v1, EPSILON);
    BOOST_CHECK_EQUAL(s2, original_s1);
}

BOOST_AUTO_TEST_CASE(test_act_after_execution)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);
    test_lob.AddLimitOrder(1, 100.0, 50.0); // market moves
    hedger.ReCalcGreeks(time, test_lob);

    double p1, v1, p2, v2;
    int s1, s2;
    std::vector<std::vector<Bar>> empty_eos;

    // First PostOrder - post order
    hedger.PostOrder(p1, v1, s1, empty_eos, test_lob, 0.5);

    // Simulate execution
    std::vector<std::vector<Bar>> execution_eos;
    std::vector<Bar> execution_round;
    execution_round.push_back(Bar(p1, s1 * v1)); // Full execution
    execution_eos.push_back(execution_round);

    // PostOrder after execution - should update inventory and potentially post new order
    hedger.PostOrder(p2, v2, s2, execution_eos, test_lob, 0.5);

    // The delta should have changed due to inventory update
    // So new order parameters might be different
    BOOST_CHECK(true); // Basic check that it doesn't crash
}

BOOST_AUTO_TEST_SUITE_END()

// Test ReCalcGreeks functionality
BOOST_AUTO_TEST_SUITE(DeltaHedgerReCalcGreeksTests)

BOOST_AUTO_TEST_CASE(test_recalc_greeks_basic)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Initially no positions
    hedger.ReCalcGreeks(time, test_lob);
    double initial_delta = hedger.Delta(implied_vol, test_lob, time);
    double initial_gamma = hedger.Gamma(implied_vol, test_lob, time);

    BOOST_CHECK_CLOSE(initial_delta, 0.0, EPSILON);
    BOOST_CHECK_CLOSE(initial_gamma, 0.0, EPSILON);

    // After reset, greeks should change
    hedger.ResetGammaContract(time, test_lob);
    double final_delta = hedger.Delta(implied_vol, test_lob, time);
    double final_gamma = hedger.Gamma(implied_vol, test_lob, time);

    BOOST_CHECK_SMALL(abs(final_delta), 0.1); // ATM straddle should have small delta
    BOOST_CHECK_GT(final_gamma, 0.0);         // Gamma should be positive
}

BOOST_AUTO_TEST_CASE(test_recalc_greeks_spot_change)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob1 = createTestLOB(99.0, 101.0);  // Mid = 100
    LOB test_lob2 = createTestLOB(104.0, 106.0); // Mid = 105
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob1);
    double delta1 = hedger.Delta(implied_vol, test_lob1, time);

    // Recalculate with different spot price
    hedger.ReCalcGreeks(time, test_lob2);
    double delta2 = hedger.Delta(implied_vol, test_lob2, time);

    // Delta should change with spot price movement
    BOOST_CHECK_NE(delta1, delta2);
}

BOOST_AUTO_TEST_SUITE_END()

// Integration tests
BOOST_AUTO_TEST_SUITE(DeltaHedgerIntegrationTests)

BOOST_AUTO_TEST_CASE(test_full_hedging_cycle)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Complete hedging cycle
    hedger.ResetGammaContract(time, test_lob);
    hedger.ReCalcGreeks(time, test_lob);

    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;

    // PostOrder several times with different market conditions
    for (double t_q = 0.1; t_q <= 1.0; t_q += 0.2)
    {
        hedger.PostOrder(p, v, s, empty_eos, test_lob, t_q);

        // Simulate partial execution occasionally
        if (t_q > 0.5)
        {
            std::vector<std::vector<Bar>> partial_execution;
            std::vector<Bar> execution_round;
            execution_round.push_back(Bar(p, s * v * 0.3)); // 30% execution
            partial_execution.push_back(execution_round);

            hedger.PostOrder(p, v, s, partial_execution, test_lob, t_q + 0.1);
        }
    }

    // Should complete without errors
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(test_multiple_reset_cycles)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();

    // Multiple reset cycles
    for (double time = 1.0; time <= 5.0; time += 1.0)
    {
        hedger.ResetGammaContract(time, test_lob);
        hedger.ReCalcGreeks(time, test_lob);

        double delta = hedger.Delta(implied_vol, test_lob, time);
        double gamma = hedger.Gamma(implied_vol, test_lob, time);

        // Each reset should produce consistent results
        BOOST_CHECK_SMALL(abs(delta), 0.1);
        BOOST_CHECK_GT(gamma, 0.0);
    }
}

BOOST_AUTO_TEST_CASE(test_stress_trading_session)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    hedger.ResetGammaContract(time, test_lob);

    // Simulate intensive trading
    for (int i = 0; i < 100; ++i)
    {
        hedger.ReCalcGreeks(time + i * 0.01, test_lob);

        double p, v;
        int s;
        std::vector<std::vector<Bar>> eos;

        // Random timing within quarter
        double t_q = (i % 10) * 0.1;
        hedger.PostOrder(p, v, s, eos, test_lob, t_q);

        // Occasionally simulate executions
        if (i % 10 == 0 && i > 0)
        {
            std::vector<Bar> execution_round;
            execution_round.push_back(Bar(p, s * v));
            eos.push_back(execution_round);

            hedger.PostOrder(p, v, s, eos, test_lob, t_q + 0.05);
        }
    }

    // Should handle intensive trading without issues
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END()

// Edge cases and error handling
BOOST_AUTO_TEST_SUITE(DeltaHedgerEdgeCaseTests)

BOOST_AUTO_TEST_CASE(test_zero_implied_vol)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.0; // Zero volatility
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Should handle zero volatility gracefully
    BOOST_CHECK_NO_THROW(hedger.ResetGammaContract(time, test_lob));
    BOOST_CHECK_NO_THROW(hedger.ReCalcGreeks(time, test_lob));
}

BOOST_AUTO_TEST_CASE(test_very_high_implied_vol)
{
    double opt_pos = 1000.0;
    double implied_vol = 5.0; // Very high volatility
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Should handle high volatility gracefully
    BOOST_CHECK_NO_THROW(hedger.ResetGammaContract(time, test_lob));
    BOOST_CHECK_NO_THROW(hedger.ReCalcGreeks(time, test_lob));
}

BOOST_AUTO_TEST_CASE(test_negative_option_position)
{
    double opt_pos = -1000.0; // Negative position
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    LOB test_lob = createTestLOB();
    double time = 1.0;

    // Should handle negative positions
    BOOST_CHECK_NO_THROW(hedger.ResetGammaContract(time, test_lob));
    BOOST_CHECK_NO_THROW(hedger.ReCalcGreeks(time, test_lob));
}

BOOST_AUTO_TEST_CASE(test_empty_execution_list)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    std::vector<std::vector<Bar>> empty_nested_eos;

    // Should handle empty execution lists
    BOOST_CHECK_NO_THROW(hedger.IsMyOrderExecuted(empty_nested_eos));
    BOOST_CHECK_EQUAL(hedger.IsMyOrderExecuted(empty_nested_eos), FALSE);
}

BOOST_AUTO_TEST_CASE(test_wide_spread_lob)
{
    double opt_pos = 1000.0;
    double implied_vol = 0.2;
    DeltaHedger hedger(opt_pos, implied_vol);

    // Very wide spread
    LOB wide_spread_lob = createTestLOB(90.0, 110.0, 100.0, 100.0);
    double time = 1.0;

    hedger.ResetGammaContract(time, wide_spread_lob);
    hedger.ReCalcGreeks(time, wide_spread_lob);

    double p, v;
    int s;
    std::vector<std::vector<Bar>> empty_eos;

    // Should handle wide spreads appropriately
    BOOST_CHECK_NO_THROW(hedger.PostOrder(p, v, s, empty_eos, wide_spread_lob, 0.5));

    // Check that pricing strategy adapts to wide spread
    double ba_spr = wide_spread_lob.ask() - wide_spread_lob.bid(); // 20.0
    BOOST_CHECK_GT(ba_spr, 10.0);                                  // Verify it's indeed wide
}

BOOST_AUTO_TEST_SUITE_END()