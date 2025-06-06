#include <iostream>
#include <vector>
#include <random>
#include "libs/LOB.hpp"
#include "libs/Random.hpp"
#include "libs/DeltaHedger.hpp"

int main()
{
    const int T = 3;
    const int H = 2;
    const int Q = 5;
    const int total_time = T * H * Q;
    const int seed = 1;

    const double p0 = 10.0;
    const double vol_news = 1.0;
    const double order_arrival_intensity = 1.0;
    const double decay_coefficient = 0.5;
    const double p_otype = 0.5;
    const double p_info = 0.5;
    const double vol_min = 0;
    const double vol_max = 10;
    const double m_spr = 0.05;
    const double v_spr = 0.1;

    Random rd(seed, vol_news, order_arrival_intensity,
              p_otype, p_info, vol_min, vol_max, m_spr, v_spr);
    DeltaHedger hedger(0.0, 0.0);
    std::vector<LOB> snapshots(1); // initialise with an empty LOB
    std::vector<double> fundamental_prices(1, p0);

    int tau = 0, tick = 0;
    int delta = 0;
    for (int t = 0; t < T; t++)
    {
        // reset gamma contract - calculate delta and gamma
        hedger.ResetGammaContract();
        for (int h = 0; h < H; h++)
        {
            // news arrives and fundamental price changesz
            const double p_h = fundamental_prices[h];
            fundamental_prices.push_back(rd.GenerateShockedPrice(p_h));
            for (int q = 0; q < Q; q++)
            {
                // create a copy of current LOB - TODO check copy constructor
                LOB currLOB = snapshots[tau];
                // generate the number of orders n ~ Pois(lambda)
                const int N = rd.GenerateNumOrders();
                std::vector<std::vector<Bar>> exe_results;
                for (int n = 0; n < N; n++)
                {
                    currLOB.DecayOrders(decay_coefficient); // decay existing orders
                    double p = 0.0, v = 0.0;
                    int s = 0;
                    enum OrderType order_type;
                    rd.GenerateOrder(order_type, p, v, s, currLOB.mid(), p_h); // generate a new order
                    // update LOB to include the order and report what orders are exercised
                    std::vector<Bar> exe_res = currLOB.AbsorbGeneralOrder(order_type, p, v, s);
                    exe_results.push_back(exe_res);
                    tick++;
                }
                // based on execution results of this quarter hedger knows his state
                // he then removes posted but unexecuted order (if any)
                // and submit new order based on current LOB
                double p_hedger = 0.0, v_hedger = 0.0;
                int s_hedger = 0;
                hedger.Act(p_hedger, v_hedger, s_hedger, exe_results);
                // LOB absorb hedger's order
                currLOB.AddLimitOrder(s_hedger, p_hedger, v_hedger);
                // snapshot granularity is quarter-wise; not tick-wise yet
                snapshots.push_back(currLOB);
                std::cout << "tau = " << tau << "; t = " << t << ", h = " << h << ", q = " << q << std::endl;
                tau++;
            }
            // delta update from hedger's reevaluation, calculate gamma
            hedger.ReCalcDelta();
        }
    }

    return 0;
}