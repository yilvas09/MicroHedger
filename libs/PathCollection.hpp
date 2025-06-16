#include <vector>
#include "libs/LOB.hpp"
#include "libs/Random.hpp"
#include "libs/DeltaHedger.hpp"

class Path
{
private:
    DeltaHedger hedger;

    std::vector<LOB> lobs;
    std::vector<double> hedger_deltas;
    std::vector<double> hedger_gammas;
    std::vector<double> fund_prices;

public:
    Path(double hedger_pos, double hedger_iv);
    ~Path();
};

class PathCollection
{
private:
    int n_paths;
    RandomInfo ran_info;
    std::vector<Path> snapshots; // vector of size n_paths

public:
    PathCollection(int n, const RandomInfo &ri);
    ~PathCollection();

    void GeneratePaths();
    void CalcLiquidityMetrics(std::vector<double>& res);
};
