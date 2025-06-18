#include <iostream>
#include <vector>
#include "libs/LOB.hpp"
#include "libs/Random.hpp"
#include "libs/DeltaHedger.hpp"
#include "libs/PathCollection.hpp"

int main()
{
    const int T = 8;
    const int H = 10;
    const int Q = 4;
    const int total_time = T * H * Q;
    const int seed = 9999;
    const int n_samples = 100;

    const double p0 = 5.0;
    const std::vector<double> aps0 = {5.02, 5.04, 5.06};
    const std::vector<double> avs0(aps0.size(), 10.0);
    const std::vector<double> bps0 = {4.94, 4.96, 4.98};
    const std::vector<double> bvs0(bps0.size(), 10.0);
    const double decay_coefficient = 0.05;
    const LOB lob0(decay_coefficient, aps0, avs0, bps0, bvs0);
    const double vol_news = 0.0;
    const double order_arrival_intensity = 1.0;
    const double p_otype = 0.25; // prob of getting market orders
    const double p_info = 0.3; // prob of getting informed orders
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

    std::vector<int> normal_paths_id;
    paths.FindPathsWithStatus(0, normal_paths_id);

    return 0;
}