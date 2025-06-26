// test_pathcollection.cpp
#define BOOST_TEST_MODULE PathCollectionTest
#include <boost/test/included/unit_test.hpp>
#include "../libs/PathCollection.hpp"

const double EPSILON = 1e-9;

// several test cases for backward compatibility 
BOOST_AUTO_TEST_SUITE(PathCollectionBkwdCompatibilityTests)

BOOST_AUTO_TEST_CASE(test_backward_compatibility_case_1)
{
    const int T = 5;
    const int H = 5;
    const int Q = 4;
    const int total_time = T * H * Q;
    const int seed = 9999;
    const int n_samples = 10;

    const double p0 = 5.0;
    const std::vector<double> aps0 = {5.02, 5.04, 5.06};
    const std::vector<double> avs0(aps0.size(), 10.0);
    const std::vector<double> bps0 = {4.94, 4.96, 4.98};
    const std::vector<double> bvs0(bps0.size(), 10.0);
    const double decay_coefficient = 0.00;
    const LOB lob0(decay_coefficient, aps0, avs0, bps0, bvs0);
    const double vol_news = 0.0;
    const double order_arrival_intensity = 1.0;
    const double p_otype = 0.1; // prob of getting market orders
    const double p_info = 0.3;  // prob of getting informed orders
    const double vol_min = 0;
    const double vol_max = 1;
    const double m_spr = -0.1;
    const double v_spr = 0.1;
    const double option_pos = 80;
    const double implied_vol = 0.089;

    PathInfo pi(T, H, Q, p0, lob0, option_pos, implied_vol);
    RandomInfo ri(seed, vol_news, order_arrival_intensity,
                  p_otype, p_info, vol_min, vol_max, m_spr, v_spr, 0.5);

    PathCollection paths(n_samples, pi, ri);
    paths.GeneratePaths();
    std::vector<double> res;
    paths.CalcLiquidityMetrics(res);

    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[0]), std::string("0.5000"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[1]), std::string("0.0027"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[2]), std::string("0.0345"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[3]), std::string("0.0637"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[4]), std::string("0.0000"));
}

BOOST_AUTO_TEST_CASE(test_backward_compatibility_case_2)
{
    const int T = 5;
    const int H = 5;
    const int Q = 4;
    const int total_time = T * H * Q;
    const int seed = 9999;
    const int n_samples = 10;

    const double p0 = 5.0;
    const std::vector<double> aps0 = {5.02, 5.04, 5.06};
    const std::vector<double> avs0(aps0.size(), 10.0);
    const std::vector<double> bps0 = {4.94, 4.96, 4.98};
    const std::vector<double> bvs0(bps0.size(), 10.0);
    const double decay_coefficient = 0.00;
    const LOB lob0(decay_coefficient, aps0, avs0, bps0, bvs0);
    const double vol_news = 0.1;
    const double order_arrival_intensity = 1.0;
    const double p_otype = 0.25; // prob of getting market orders
    const double p_info = 0.3;  // prob of getting informed orders
    const double vol_min = 0;
    const double vol_max = 1;
    const double m_spr = -0.1;
    const double v_spr = 0.1;
    const double option_pos = 10;
    const double implied_vol = 0.089;

    PathInfo pi(T, H, Q, p0, lob0, option_pos, implied_vol);
    RandomInfo ri(seed, vol_news, order_arrival_intensity,
                  p_otype, p_info, vol_min, vol_max, m_spr, v_spr, 0.5);

    PathCollection paths(n_samples, pi, ri);
    paths.GeneratePaths();
    std::vector<double> res;
    paths.CalcLiquidityMetrics(res);

    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[0]), std::string("0.1000"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[1]), std::string("0.0024"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[2]), std::string("0.1428"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[3]), std::string("0.0642"));
    BOOST_CHECK_EQUAL(boost::str(boost::format("%1$.4f") % res[4]), std::string("0.1535"));
}

BOOST_AUTO_TEST_SUITE_END()