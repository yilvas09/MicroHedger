#include <vector>
#include "LOB.hpp"
#include "Random.hpp"
#include "DeltaHedger.hpp"

struct PathInfo
{
    int n_days;
    int n_hours;
    int n_quarters;
    LOB lob_0; // includes decay coefficients
    double p_0;
    double hedger_opt_pos;
    double hedger_implied_vol;

    PathInfo(int _n_days,
             int _n_hours,
             int _n_quarters,
             double _p_0,
             const LOB &_lob_0,
             double _h_opt_pos,
             double _h_iv)
        : n_days(_n_days),
          n_hours(_n_hours),
          n_quarters(_n_quarters),
          p_0(_p_0),
          lob_0(_lob_0),
          hedger_opt_pos(_h_opt_pos),
          hedger_implied_vol(_h_iv)
    {
    }

    static void GenerateScenarios(std::vector<PathInfo> &scens,
                                  const Parameter &param_name,
                                  const std::vector<double> &range,
                                  const PathInfo &template_pi);
};

class Path
{
private:
    const int n_days;
    const int n_hours;
    const int n_quarters;
    const RandomInfo ran_info;

    int status;
    DeltaHedger hedger;

    std::vector<LOB> lobs;             // quarter-wise
    std::vector<double> mid_prices;    // tick-wise mid prices;
    std::vector<double> hedger_deltas; // hour-wise
    std::vector<double> hedger_gammas; // hour-wise
    std::vector<double> fund_prices;   // hour-wise

public:
    friend class PathCollection;
    Path(const PathInfo &_path_info,
         const RandomInfo &_ran_info);
    ~Path();

    inline int Status() const { return status; }

    void ClearPath();
    void GenOnePath();
};

class PathCollection
{
private:
    int n_paths;
    PathInfo path_info;
    RandomInfo ran_info;
    std::vector<Path> snapshots; // vector of size n_paths

public:
    PathCollection(int n, const PathInfo &pi, const RandomInfo &ri);
    ~PathCollection();

    std::vector<double> getLOBVolumeTrajectories(int s, int path_id) const;

    void GeneratePaths();
    void CalcLiquidityMetrics(std::vector<double> &res) const;
    void FindPathsWithStatus(int status, std::vector<int> &indices) const;
    void PrintSimulationResults() const;
};
