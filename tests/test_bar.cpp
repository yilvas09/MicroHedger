// test_bar.cpp
#define BOOST_TEST_MODULE BarTest
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include "../libs/Bar.hpp"

const double EPSILON = 1e-9;

// test simple functions of class Bar
BOOST_AUTO_TEST_SUITE(BarBasicTests)

BOOST_AUTO_TEST_CASE(test_default_constructor)
{
    Bar bar;
    BOOST_CHECK_CLOSE(bar.Price(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(bar.Volume(), 0.0, EPSILON);
    BOOST_CHECK_EQUAL(bar.IsEmptyBar(), TRUE);
    BOOST_CHECK_EQUAL(bar.IsEmptyVolume(), TRUE);
    BOOST_CHECK_EQUAL(bar.IsEmpty(), TRUE);
}

BOOST_AUTO_TEST_CASE(test_parameterized_constructor)
{
    Bar bar(100.5, 250.0);
    BOOST_CHECK_CLOSE(bar.Price(), 100.5, EPSILON);
    BOOST_CHECK_CLOSE(bar.Volume(), 250.0, EPSILON);
    BOOST_CHECK_EQUAL(bar.IsEmptyBar(), FALSE);
    BOOST_CHECK_EQUAL(bar.IsEmptyVolume(), FALSE);

    bar = Bar(100.5, 0.0);
    BOOST_CHECK_EQUAL(bar.IsEmptyBar(), FALSE);
    BOOST_CHECK_EQUAL(bar.IsEmptyVolume(), TRUE);
    BOOST_CHECK_EQUAL(bar.IsEmpty(), FALSE);
}

BOOST_AUTO_TEST_CASE(test_negative_values)
{
    Bar bar(-50.0, -100.0);
    BOOST_CHECK_CLOSE(bar.Price(), -50.0, EPSILON);
    BOOST_CHECK_CLOSE(bar.Volume(), -100.0, EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(BarPriceComparisonTests)

BOOST_AUTO_TEST_CASE(test_price_higher_than)
{
    Bar bar(100.0, 10);

    BOOST_CHECK_EQUAL(bar.PriceHigherThan(99.98), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceHigherThan(100.00), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceHigherThan(100.11), FALSE);

    BOOST_CHECK_EQUAL(bar.PriceHigherEqual(100.00), TRUE);
}

BOOST_AUTO_TEST_CASE(test_price_same_as)
{
    Bar bar(100.0, 10);

    BOOST_CHECK_EQUAL(bar.PriceSameAs(99.98), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(100.0), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(100.0 + __DBL_EPSILON__ / 2), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(100.11), FALSE);
}

BOOST_AUTO_TEST_CASE(test_price_lower_than)
{
    Bar bar(100.0, 10);

    BOOST_CHECK_EQUAL(bar.PriceLowerThan(99.98), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceLowerThan(100.00), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceLowerThan(100.01), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceLowerThan(100.11), TRUE);

    BOOST_CHECK_EQUAL(bar.PriceLowerEqual(100.00), TRUE);
}

BOOST_AUTO_TEST_SUITE_END()

// test Bar::ExecuteAgainst
BOOST_AUTO_TEST_SUITE(BarExecuteAgainstTests)

BOOST_AUTO_TEST_CASE(test_execute_partial_volume)
{
    Bar bar(100.0, 500.0);
    double incoming_volume = 200.0;

    int result = bar.ExecuteAgainst(incoming_volume);

    // 200 executed, 300 left
    BOOST_CHECK_EQUAL(result, 1); // bar is still alive
    BOOST_CHECK_CLOSE(bar.Volume(), 300.0, EPSILON);
    BOOST_CHECK_CLOSE(incoming_volume, 0.0, EPSILON); // incoming order fully executed
}

BOOST_AUTO_TEST_CASE(test_execute_exact_volume)
{
    Bar bar(100.0, 200.0);
    double incoming_volume = 200.0;

    int result = bar.ExecuteAgainst(incoming_volume);

    // incoming order fully executed
    BOOST_CHECK_EQUAL(result, 0); // bar should be removed
    BOOST_CHECK_CLOSE(bar.Volume(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(incoming_volume, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_execute_excess_volume)
{
    Bar bar(100.0, 150.0);
    double incoming_volume = 300.0;

    int result = bar.ExecuteAgainst(incoming_volume);

    // bar is empty, incoming order not fully executed
    BOOST_CHECK_EQUAL(result, 0); // bar should be removed
    BOOST_CHECK_CLOSE(bar.Volume(), 0.0, EPSILON);
    BOOST_CHECK_CLOSE(incoming_volume, 150.0, EPSILON); // 150 left in incoming order
}

BOOST_AUTO_TEST_CASE(test_execute_zero_volume)
{
    Bar bar(100.0, 200.0);
    double incoming_volume = 0.0;

    int result = bar.ExecuteAgainst(incoming_volume);

    // nothing should happens
    BOOST_CHECK_EQUAL(result, 1); // bar is still alive
    BOOST_CHECK_CLOSE(bar.Volume(), 200.0, EPSILON);
    BOOST_CHECK_CLOSE(incoming_volume, 0.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_execute_very_small_volume)
{
    Bar bar(100.0, 1e-10); // very small volume
    double incoming_volume = 1e-11;

    int result = bar.ExecuteAgainst(incoming_volume);

    BOOST_CHECK_EQUAL(result, 1); // bar is still alive
    BOOST_CHECK_CLOSE(bar.Volume(), 1e-10 - 1e-11, 1e-12);
}

BOOST_AUTO_TEST_SUITE_END()

// test Bar::AddVolumesBy
BOOST_AUTO_TEST_SUITE(BarAddVolumeTests)

BOOST_AUTO_TEST_CASE(test_add_positive_volume)
{
    Bar bar(100.0, 200.0);
    bar.AddVolumesBy(50.0);

    BOOST_CHECK_CLOSE(bar.Volume(), 250.0, EPSILON);
    BOOST_CHECK_CLOSE(bar.Price(), 100.0, EPSILON); // price remains unchanged
}

BOOST_AUTO_TEST_CASE(test_add_negative_volume)
{
    Bar bar(100.0, 200.0);
    bar.AddVolumesBy(-50.0);

    BOOST_CHECK_CLOSE(bar.Volume(), 150.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_add_zero_volume)
{
    Bar bar(100.0, 200.0);
    bar.AddVolumesBy(0.0);

    BOOST_CHECK_CLOSE(bar.Volume(), 200.0, EPSILON);
}

BOOST_AUTO_TEST_CASE(test_add_multiple_volumes)
{
    Bar bar(100.0, 100.0);
    bar.AddVolumesBy(25.0);
    bar.AddVolumesBy(75.0);
    bar.AddVolumesBy(-50.0);

    BOOST_CHECK_CLOSE(bar.Volume(), 150.0, EPSILON);
}

BOOST_AUTO_TEST_SUITE_END()

// Edge cases and exceptions
BOOST_AUTO_TEST_SUITE(BarEdgeCases)

BOOST_AUTO_TEST_CASE(test_very_large_numbers)
{
    Bar bar(1e10, 1e15);
    double incoming_volume = 5e14;

    int result = bar.ExecuteAgainst(incoming_volume);

    BOOST_CHECK_EQUAL(result, 1);
    BOOST_CHECK_CLOSE(bar.Volume(), 6e14, 1e5);
}

BOOST_AUTO_TEST_CASE(test_very_small_numbers)
{
    Bar bar(1e-10, 1e-14);
    bar.AddVolumesBy(5e-15);

    BOOST_CHECK_CLOSE(bar.Volume(), 1.5e-14, 1e-15);
}

BOOST_AUTO_TEST_SUITE_END()

// test cases for non-zero tick size should be placed at the end of the tests
// as set tick_size is not revertible (for safety consideration)
BOOST_AUTO_TEST_SUITE(BarNonZeroTickSizeTests)

BOOST_AUTO_TEST_CASE(test_nonzero_ticksize)
{
    Bar::SetTickSize(0.1);
    Bar bar(100.45, 250.0);
    BOOST_CHECK_CLOSE(bar.Price(), 100.5, EPSILON);
    BOOST_CHECK_CLOSE(bar.Volume(), 250.0, EPSILON);
    BOOST_CHECK_EQUAL(bar.IsEmptyBar(), FALSE);
    BOOST_CHECK_EQUAL(bar.IsEmptyVolume(), FALSE);

    bar = Bar(100.42, 0.0);
    BOOST_CHECK_CLOSE(bar.Price(), 100.4, EPSILON);
    BOOST_CHECK_EQUAL(bar.IsEmptyBar(), FALSE);
    BOOST_CHECK_EQUAL(bar.IsEmptyVolume(), TRUE);
    BOOST_CHECK_EQUAL(bar.IsEmpty(), FALSE);

    BOOST_CHECK_THROW(Bar::SetTickSize(0.01), std::logic_error);
}

BOOST_AUTO_TEST_CASE(test_nonzero_ticksize_price_higher_than)
{
    Bar bar(100.0, 10);

    BOOST_CHECK_EQUAL(bar.PriceHigherThan(99.88), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceHigherThan(100.01), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceHigherThan(100.11), FALSE);

    BOOST_CHECK_EQUAL(bar.PriceHigherEqual(100.01), TRUE);
}

BOOST_AUTO_TEST_CASE(test_nonzero_ticksize_price_same_as)
{
    Bar bar(100.0, 10);

    BOOST_CHECK_EQUAL(bar.PriceSameAs(99.88), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(99.98), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(100.0), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(100.0 + __DBL_EPSILON__ / 2), TRUE);
    BOOST_CHECK_EQUAL(bar.PriceSameAs(100.11), FALSE);
}

BOOST_AUTO_TEST_CASE(test_nonzero_ticksize_price_lower_than)
{
    Bar bar(100.0, 10);

    BOOST_CHECK_EQUAL(bar.PriceLowerThan(99.88), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceLowerThan(100.01), FALSE);
    BOOST_CHECK_EQUAL(bar.PriceLowerThan(100.11), TRUE);

    BOOST_CHECK_EQUAL(bar.PriceLowerEqual(100.01), TRUE);
}

BOOST_AUTO_TEST_SUITE_END()