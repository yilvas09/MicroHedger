#include <vector>
#include <memory>
#include "PathCollection.hpp"
#include <boost/format.hpp>

void PathInfo::GenerateScenarios(std::vector<PathInfo> &scens,
                                 const Parameter &param_name,
                                 const std::vector<double> &range,
                                 const PathInfo &pi_template)
{
    const int n = range.size();
    scens.resize(n, pi_template);
    for (int i = 0; i < n; i++)
    {
        PathInfo &scen = scens[i];
        double val = range[i];
        switch (param_name)
        {
        case N_DAYS:
            scen.n_days = val;
            break;
        case N_HOURS:
            scen.n_hours = val;
            break;
        case N_QUARTERS:
            scen.n_quarters = val;
            break;
        case INI_FUNDAMENTAL:
            scen.p_0 = val;
            break;
        case HEDGER_OPTION_POSITION:
            scen.hedger_opt_pos = val;
            break;
        case HEDGER_IMPLIED_VOLATILITY:
            scen.hedger_implied_vol = val;
            break;
        default:
            throw std::invalid_argument("This parameter is not supported yet.");
            break;
        }
    }
}

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
    LOB lob_0(_path_info.lob_0);
    lob_0.setSafetyCheck(true); // activate safety check for recording liquidity crisis
    lobs.resize(1, lob_0);
    mid_prices.resize(1, _path_info.lob_0.mid());
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
    try
    {
        for (int day = 0; day < n_days; day++)
        {
            double time = day;
            hedger.ResetGammaContract(time, lobs.back());
            for (int hour = 0; hour < n_hours; hour++)
            {
                // news arrives and fundamental price changes
                const double ph = rd.GenerateShockedPrice(fund_prices.back());
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
                        mid_prices.push_back(currLOB.mid());
                    }
                    fund_prices.push_back(ph);
                    lobs.push_back(currLOB);

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
    catch (const std::out_of_range &e)
    {
        status = -1;
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

std::vector<double> PathCollection::getLOBVolumeTrajectories(int s, int path_id) const
{
    std::vector<double> volumes;
    auto &lobs = snapshots.at(path_id).lobs;
    for (auto &l : lobs)
        volumes.push_back(s * l.getTotalVolume(s));
    return volumes;
}

// this function should updates std::vector<Path> snapshots
void PathCollection::GeneratePaths()
{
    for (int i = 0; i < n_paths; i++)
        snapshots[i].GenOnePath();
}

void PathCollection::CalcLiquidityMetrics(std::vector<double> &res) const
{
    res.resize(0);
    // 0. failure rate
    std::vector<int> valid_idx;
    FindPathsWithStatus(0, valid_idx);
    int n_valid_paths = valid_idx.size();
    res.push_back(1. - (double)n_valid_paths / n_paths);

    // 1. volatilities
    double v_1 = 0.0, v_2 = 0.0;
    for (int i_path : valid_idx)
    {
        const std::vector<double> &mid_p = snapshots[i_path].mid_prices;
        std::vector<double> mid_p_shocks;
        double mean_shocks = mid_p[0], min_p = mid_p[0], max_p = mid_p[0];
        double v_1_path = 0.0;
        for (int i_p = 1; i_p < mid_p.size() - 1; i_p++)
        {
            double shock = mid_p[i_p] - mid_p[i_p - 1];
            mid_p_shocks.push_back(shock);
            mean_shocks += shock;
            min_p = std::min(min_p, mid_p[i_p]);
            max_p = std::max(max_p, mid_p[i_p]);
        }
        mean_shocks /= (int)mid_p.size();
        for (int i_p = 0; i_p < mid_p_shocks.size() - 1; i_p++)
            v_1_path += std::pow(mean_shocks - mid_p_shocks[i_p], 2);

        // 1.1 volatility 1
        v_1 += (v_1_path / (double)mid_p.size());
        // 1.2. volatility 2
        v_2 += (max_p - min_p);
        // TODO: 1.3-1.4 quantile shocks to be implemented
    }
    v_1 /= n_valid_paths;
    v_2 /= n_valid_paths;
    res.push_back(v_1);
    res.push_back(v_2);

    // 2. liquidity
    double l_1 = 0.0, l_2 = 0.0;
    for (int i_path : valid_idx)
    {
        const std::vector<LOB> &lobs = snapshots[i_path].lobs;
        // 2.1 average bid-ask spread
        // TODO 2.2 total volume
        double l_1_path = 0.0;
        for (int i_p = 0; i_p < lobs.size() - 1; i_p++)
        {
            const LOB &lob = lobs[i_p];
            double ba_spr = lob.ask() - lob.bid();
            l_1_path += ba_spr;
        }
        l_1 += (l_1_path / (double)lobs.size());
    }
    l_1 /= n_valid_paths;
    res.push_back(l_1);

    // 3. price discovery
    double d_1 = 0.0;
    for (int i_path : valid_idx)
    {
        const std::vector<LOB> &lobs = snapshots[i_path].lobs;
        const std::vector<double> &f_ps = snapshots[i_path].fund_prices;
        double d_1_path = 0.0;
        for (int tau = 0; tau < lobs.size() - 1; tau++)
        {
            d_1_path += std::pow(lobs[tau].mid() - f_ps[tau], 2);
        }
        d_1 += d_1_path / (int)lobs.size();
    }
    d_1 /= n_valid_paths;
    res.push_back(d_1);
}

void PathCollection::FindPathsWithStatus(int status, std::vector<int> &indices) const
{
    indices.resize(0);
    for (int i = 0; i < snapshots.size(); i++)
    {
        if (snapshots[i].Status() == status)
            indices.push_back(i);
    }
}

void PathCollection::PrintSimulationResults() const
{
    std::vector<double> results;
    CalcLiquidityMetrics(results);
    std::cout << "==============================" << std::endl;
    for (int i = 0; i < results.size(); i++)
    {
        std::string res_row;
        switch (i)
        {
        case 0:
            res_row += "Market failure rate:\t";
            break;
        case 1:
            res_row += "Volatility - std.dev.:\t";
            break;
        case 2:
            res_row += "Volatility - high-low spread:\t";
            break;
        case 3:
            res_row += "Liquidity - avg. bid-ask spread:\t";
            break;
        case 4:
            res_row += "Price discovery - distance btw. mid and fund.:\t";
            break;
        default:
            break;
        }
        res_row += boost::str(boost::format("%1$.4f\t") % results[i]);
        std::cout << res_row << std::endl;
    }
    std::cout << "==============================" << std::endl;
}