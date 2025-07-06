#include <iostream>
#include <vector>
#include "libs/PathCollection.hpp"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

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
    double p_otype = 0.7; // prob of getting limit orders (1-u)
    // p_otype = 0.63;
    const double p_info = 0.25; // prob of getting informed orders (i)
    const double vol_min = 0;
    const double vol_max = 1;
    const double m_spr = -0.1;
    const double v_spr = 0.1;
    const double option_pos = 0;
    const double implied_vol = 0.089;
    const double tick_size = 0.01;

    Bar::SetTickSize(tick_size);

    PathInfo pi_benchmark(T, H, Q, p0, lob0, option_pos, implied_vol);
    RandomInfo ri_benchmark(seed, vol_news, order_arrival_intensity,
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

    PathCollection paths(n_samples, pi_benchmark, ri_benchmark);
    paths.GeneratePaths();
    paths.PrintSimulationResults();

    // std::vector<int> normal_paths_id;
    // paths.FindPathsWithStatus(0, normal_paths_id);
    // std::vector<double> results;
    // paths.CalcLiquidityMetrics(results);

    plt::figure_size(1200, 780);
    for (int id = 0; id < n_samples; id++)
    {
        std::vector<double> bidVolsTrajectory = paths.getLOBVolumeTrajectories(-1, id);
        plt::plot(bidVolsTrajectory, "b--");

        std::vector<double> askVolsTrajectory = paths.getLOBVolumeTrajectories(1, id);
        plt::plot(askVolsTrajectory, "r--");
    }
    plt::xlabel("time steps");
    plt::ylabel("volume");

    // // Save the image (file format is determined by the extension)
    // plt::save("./vol-trajectory.png");
    // // or show the graph
    plt::show();

    // // Prepare data.
    // int n = 5000;
    // std::vector<double> x(n), y(n), z(n), w(n,2);
    // for(int i=0; i<n; ++i) {
    //     x.at(i) = i*i;
    //     y.at(i) = sin(2*M_PI*i/360.0);
    //     z.at(i) = log(i);
    // }

    // // Test: drawing plots
    // // Set the size of output image to 1200x780 pixels
    // plt::figure_size(1200, 780);
    // // Plot line from given x and y data. Color is selected automatically.
    // plt::plot(x, y);
    // // Plot a red dashed line from given x and y data.
    // plt::plot(x, w,"r--");
    // // Plot a line whose name will show up as "log(x)" in the legend.
    // plt::named_plot("log(x)", x, z);
    // // Set x-axis to interval [0,1000000]
    // plt::xlim(0, 1000*1000);
    // // Add graph title
    // plt::title("Sample figure");
    // // Enable legend.
    // plt::legend();

    // // Save the image (file format is determined by the extension)
    // // plt::save("./basic.png");
    // // or show the graph
    // plt::show();

    return 0;
}