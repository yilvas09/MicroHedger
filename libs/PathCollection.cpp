#include <vector>
#include <memory>
#include "PathCollection.hpp"

Path::Path(
    const PathInfo &_path_info,
    const RandomInfo &_ran_info)
    : n_days(_path_info.n_days),
      n_hours(_path_info.n_hours),
      n_quarters(_path_info.n_quarters),
      hedger(_path_info.hedger_opt_pos, _path_info.hedger_implied_vol),
      ran_info(_ran_info)
{
    status = 0;
    lobs.resize(1, _path_info.lob_0);
    hedger_deltas.resize(1, 0.0);
    hedger_gammas.resize(1, 0.0);
    fund_prices.resize(1, _path_info.p_0);
}

Path::~Path()
{
}

void Path::ClearPath()
{
    hedger.ClearOrderAndInventories();
    lobs.resize(0);
    hedger_deltas.resize(0);
    hedger_gammas.resize(0);
    fund_prices.resize(0);
}

void Path::GenOnePath()
{
    Random rd(ran_info);
    for (int day = 0; day < n_days; day++)
    {
        double time = day;
        hedger.ResetGammaContract(time, lobs.back());
        for (int hour = 0; hour < n_hours; hour++)
        {
            // news arrives and fundamental price changes
            const double ph = fund_prices.back();
            fund_prices.push_back(rd.GenerateShockedPrice(ph));
            for (int quar = 0; quar < n_quarters; quar++)
            {
                // create a copy of current LOB
                LOB currLOB(lobs.back());
                const int n_ticks = rd.GenerateNumOrders();
                std::vector<std::vector<Bar>> exe_orders;
                for (int tick = 0; tick < n_ticks; tick++)
                {
                    currLOB.DecayOrders();
                    double p = 0.0, v = 0.0;
                    int s = 0; // sell: 1, buy: -1
                    enum OrderType order_type;
                    rd.GenerateOrder(order_type, p, v, s, currLOB.mid(), ph); // generate a new order
                    // update LOB to include the order and report what orders are exercised
                    std::vector<Bar> exe_order = currLOB.AbsorbGeneralOrder(order_type, p, v, s);
                    exe_orders.push_back(exe_order);
                }
                lobs.push_back(currLOB);
                if (currLOB.oneSideEmpty())
                {
                    status = -1;
                    return;
                }

                // based on execution results of this quarter hedger knows his state
                if (!hedger.IsMyOrderExecuted(exe_orders))
                {
                    // he either removes posted but unexecuted order (if any)
                    currLOB.CancelLimitOrder(Utils::sgn(hedger.getOrderVolume()), hedger.getOrderPrice(), abs(hedger.getOrderVolume()));
                    // and submit new order based on current LOB
                    double p_hedger = 0.0, v_hedger = 0.0, t_q = (double)quar / n_quarters;
                    int s_hedger = 0;
                    hedger.PostOrder(p_hedger, v_hedger, s_hedger, exe_orders, currLOB, t_q);
                    // LOB absorb hedger's order
                    std::vector<Bar> exe_order_hedger = currLOB.AbsorbGeneralOrder(LIMITORDER, p_hedger, v_hedger, s_hedger);
                    // hedger updates inventories
                    hedger.UpdateInventories(std::vector<std::vector<Bar>>(1, exe_order_hedger));
                }
                else
                {
                    // or he updates his inventories
                    hedger.UpdateInventories(exe_orders);
                }
            }
            // delta update from hedger's reevaluation, calculate gamma
            time = day + (hour + 1) * 1. / n_hours;
            hedger.ReCalcGreeks(time, lobs.back());
        }
    }
}

PathCollection::PathCollection(int n,
                               const PathInfo &pi,
                               const RandomInfo &ri)
    : n_paths(n),
      path_info(pi),
      ran_info(ri)
{
    Path p_temp(pi, ri);
    snapshots.resize(1, p_temp);
    for (int i = 1; i < n_paths; i++)
    {
        RandomInfo ri_i(ri.seed + i, ri);
        std::unique_ptr<Path> ptr_pth(new Path(pi, ri_i));
        snapshots.push_back(*ptr_pth);
    }
}

PathCollection::~PathCollection()
{
}

// this function should updates std::vector<Path> snapshots
void PathCollection::GeneratePaths()
{
    for (int i = 0; i < n_paths; i++)
        snapshots[i].GenOnePath();
}

void PathCollection::CalcLiquidityMetrics(std::vector<double> &res)
{
}

void PathCollection::FindPathsWithStatus(int status, std::vector<int> &indices)
{
    indices.resize(0);
    for (int i = 0; i < snapshots.size(); i++)
    {
        if (snapshots[i].Status() == status)
            indices.push_back(i);
    }
}