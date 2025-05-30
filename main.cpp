#include <iostream>
#include <vector>
#include <random>
#include "libs/LOB.hpp"

int main()
{
    const int T = 3;
    const int H = 2;
    const int Q = 5;
    const int total_time = T * H * Q;

    const double p0 = 1.0;
    const double vol_news = 1.0;

    std::default_random_engine norm_generator;
    std::normal_distribution<double> norm_distribution(0.0, vol_news);

    // initialise with an empty LOB
    std::vector<LOB> snapshots(1);
    std::vector<double> fundamental_prices(1, p0);
    int tau = 0;
    int delta = 0;
    for (int t = 0; t < T; t++)
    {
        // reset gamma contract - calculate delta and gamma
        // hedger.resetGammaContract()
        for (int h = 0; h < H; h++)
        {
            // news arrives and fundamental price changesz
            const double p_h = fundamental_prices[h];
            fundamental_prices.push_back(p_h + norm_distribution(norm_generator));
            for (int q = 0; q < Q; q++)
            {
                // generate the number of orders n
                int N = 3;
                // and the specs for each of the n orders
                for (int n = 0; n < N; n++)
                {
                    // decay existing orders
                    // update LOB to include the order
                    // report what orders are exercised so that hedger knows his state

                    // hedger removes posted but unexecuted order (if any)
                    // hedger submit orders based on current LOB, and LOB absorb hedger's order
                }

                tau++;
            }
            // delta update from hedger's reevaluation, calculate gamma
        }
    }

    // handle the final time (T, 0, 0)

    return 0;
}