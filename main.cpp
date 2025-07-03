#include <iostream>
#include <vector>
#include "libs/PathCollection.hpp"

int main()
{
    const int T = 20;
    const int H = 10;
    const int Q = 4;
    const int total_time = T * H * Q;
    const int seed = 1;
    const int n_samples = 100;

    const double p0 = 5.0;
    const std::vector<double> aps0 = {5.02, 5.04, 5.06};
    const std::vector<double> avs0(aps0.size(), 10.0);
    const std::vector<double> bps0 = {4.94, 4.96, 4.98};
    const std::vector<double> bvs0(bps0.size(), 10.0);
    const double decay_coefficient = 0.05;

    const LOB lob0(decay_coefficient, aps0, avs0, bps0, bvs0);
    const double vol_news = 0.0;
    const double order_arrival_intensity = 40.0;
    const double p_otype = 0.7; // prob of getting limit orders (1-u)
    const double p_info = 0.25;  // prob of getting informed orders (i)
    const double vol_min = 0;
    const double vol_max = 1;
    const double m_spr = -0.1;
    const double v_spr = 0.1;
    const double option_pos = 0;
    const double implied_vol = 0.089;
    const double tick_size = 0.01;

    Bar::SetTickSize(tick_size);

    PathInfo pi(T, H, Q, p0, lob0, option_pos, implied_vol);
    RandomInfo ri(seed, vol_news, order_arrival_intensity,
                  p_otype, p_info, vol_min, vol_max, m_spr, v_spr, 0.5);

    Parameter param_type = N_DAYS;
    const std::vector<double> param_range = {5, 6, 7, 8, 9};

    // std::vector<PathInfo> pi_scenarios;
    // PathInfo::GenerateScenarios(pi_scenarios, param_type, param_range, pi);
    // for (auto &pi_scen : pi_scenarios)
    // {
    //     PathCollection paths(n_samples, pi_scen, ri);
    //     paths.GeneratePaths();
    //     paths.PrintSimulationResults();
    // }

    PathCollection paths(n_samples, pi, ri);
    paths.GeneratePaths();
    paths.PrintSimulationResults();

    // std::vector<int> normal_paths_id;
    // paths.FindPathsWithStatus(0, normal_paths_id);
    // std::vector<double> results;
    // paths.CalcLiquidityMetrics(results);

    return 0;
}