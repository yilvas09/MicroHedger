# MicroHedger

A computational replication of "Gamma Positioning and Market Quality" by Buis et al. (2024), exploring the impact of dynamic hedgers' gamma positioning on market quality, with a particular focus on liquidity and probability dry-outs, through agent-based simulations.

## Paper Overview

This project replicates the research presented in:

**"Gamma Positioning and Market Quality"**  
*Boyd Buis, Mary Pieterse-Bloem, Willem F.C. Verschoor, Remco C.J. Zwinkels*  
*Journal of Economic Dynamics and Control, Volume 164, 2024*  
*DOI: [10.1016/j.jedc.2024.104880](https://doi.org/10.1016/j.jedc.2024.104880)*

### Abstract

The original paper studies the effect of gamma positioning of dynamic hedgers on market quality through simulations. Key findings include:

- **Positive gamma positioning** reduces volatility and increases market stability, while **negative gamma positioning** increases volatility and makes markets more prone to failures  
- **Price discovery** typically worsens when dynamic hedgers become more prevalent, regardless of positioning sign
- **Policy implications**: Altering net gamma position of dynamic hedgers can serve as a policy instrument to improve market quality, especially for low-liquidity instruments

## Project Objectives

MicroHedger aims to:

1. **Replicate** the zero-intelligence agent-based model from Buis et al. (2024)
2. **Reproduce** key findings regarding gamma positioning effects on market failure rates, in particular the phase diagrams w.r.t. different parameters that are provided as in table 1.
3. **Analytise** trajectories of volumes of the limit-order book all paths, failed and unfailed, under various time horizons to confirm stationarity.

#### Table 1: Failure rate of simulated markets, presented in Buils et al. (2024)

|                | -80 | -60 | -40 | -20 | 0   | 20  | 40  | 60  | 80  |
|----------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| **Panel A: immediacy i**         ||||||||||
| i = 0.3        | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| i = 0.325      | 5%  | 10% | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| i = 0.35       | 50% | 45% | 5%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| i = 0.375      | 95% | 90% | 45% | 0%  | 20% | 0%  | 0%  | 0%  | 0%  |
| i = 0.4        | 100%| 95% | 100%| 90% | 85% | 0%  | 0%  | 0%  | 0%  |
| i = 0.45       | 100%| 100%| 100%| 100%| 100%| 95% | 65% | 10% | 10% |
| i = 0.5        | 100%| 100%| 100%| 100%| 100%| 100%| 100%| 100%| 95% |
| **Panel B: news volatility σₙ**  ||||||||||
| σₙ = 0.01      | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| σₙ = 0.025     | 10% | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| σₙ = 0.05      | 35% | 15% | 10% | 5%  | 0%  | 0%  | 5%  | 10% | 0%  |
| σₙ = 0.075     | 75% | 70% | 55% | 35% | 20% | 20% | 20% | 20% | 40% |
| σₙ = 0.1       | 100%| 100%| 90% | 75% | 65% | 55% | 55% | 70% | 65% |
| σₙ = 0.125     | 100%| 100%| 90% | 85% | 80% | 100%| 95% | 80% | 65% |
| **Panel C: order decay ζ**       ||||||||||
| ζ = 0.05       | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| ζ = 0.075      | 50% | 15% | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| ζ = 0.1        | 70% | 40% | 5%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| ζ = 0.125      | 80% | 80% | 25% | 5%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| ζ = 0.15       | 100%| 100%| 50% | 15% | 0%  | 0%  | 0%  | 0%  | 5%  |
| ζ = 0.175      | 100%| 100%| 85% | 35% | 10% | 5%  | 0%  | 0%  | 0%  |
| **Panel D: market maker spread μₚ** ||||||||||
| μₚ = -0.1      | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| μₚ = -0.15     | 0%  | 5%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| μₚ = -0.2      | 25% | 20% | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| μₚ = -0.25     | 90% | 75% | 20% | 0%  | 0%  | 0%  | 0%  | 0%  | 0%  |
| μₚ = -0.3      | 100%| 100%| 95% | 20% | 0%  | 10% | 40% | 30% | 30% |
| μₚ = -0.35     | 100%| 100%| 100%| 90% | 5%  | 75% | 85% | 75% | 75% |

## Methodology

The simulation framework implements:

- **Zero-intelligence agents** as baseline market participants
- **Dynamic hedgers** with configurable gamma positioning
- **Market quality metrics** including volatility, liquidity, and price discovery measures
- **Stress testing scenarios** to evaluate market resilience

### Key Model Components

- **Agent Types**: note that for the first 4 types, we model 4 different order arrival processes (rather than agents' behaviour).
  - informed investors (liquitidy takers);
  - uninformed investors;
  - informed market makers (liquidity providers);
  - uninformed market makers;
  - dynamic hedgers.
- **Gamma Positioning**: Positive, negative, and neutral gamma scenarios
- **Market Structure**: Continuous double auction with realistic market microstructure
- **Quality Metrics**: **Market failure rate** (and probably bid-ask spreads, price impact, volatility measures, and market depth)

## 🚀 Getting Started

### Prerequisites

```bash
# Boost required
brew install boost # for MacOS using Homebrew
sudo port install boost # MacOS using Mac Ports
```

### Installing the project

```bash
git clone https://github.com/yilvas09/MicroHedger.git
cd MicroHedger
```

### Building the project

```bash
# create a build subfolder for built executables
mkdir build
cd build
# build with cmake and make
cmake ..
make
```

## Experiments and results

### Comparative statics

We first set a list of benchmark parameters as in table 2. We then vary the option position and each of the following parameters, in order to obtain a two-dimension surface of market failure rate.

#### Table 2: Benchmark parameters

| Parameter | Value | Meaning |
|:---------:|:-----:|:-------:|
| Ticksize  | 0.01  | tick size |
| *N*       | 100   | number of paths |
| *T*       | 20    | number of trading sessions ('days') |
| *H*       | 10    | number of trading intervals ('hours') |
| *Q*       | 4     | number of trading sub-intervals ('quarters') |
| *λ*       | 40    | order arrival intensity |
| *u*       | 0.3   | probability of market orders |
| *i*       | 0.3   | probability of informed orders |
| $\sigma_p $ | 0.1 | std.dev. of spreads of incoming limit orders |
| $\mu_p$ | -0.1 | mean of spreads of incoming limit orders |
| $\sigma$ | 0.089 | implied volatility for delta calculation |
| $\nu_{\text{min}}$ | 0 | minimum volume of incoming orders |
| $\nu_{\text{max}}$ | 1 | maximum volume of incoming orders |
| $p_0$ | 5 | initial value of the fundamental price |
| $\mu_0$ | 0 | mean of the fundamental news shock |
| $\sigma_0$ | 0 | std.dev. of the fundamental news shock |
| $\xi$ | 0.05 | order decay parameters |

In particular, when we vary the number of trader sessions/intervals/sub-intervals, we also plot the trajectories bid/ask volumes of the LOB, to confirm all other comparative statics tests are performed under a time horizon long enough to guarantee stationarity.

<!-- ### Quick Start

```python
from microhedger import MarketSimulation, DynamicHedger

# Initialize simulation with positive gamma hedgers
sim = MarketSimulation(
    n_informed=50,
    n_noise=100,
    n_hedgers=25,
    gamma_position='positive'
)

# Run simulation
results = sim.run(n_periods=10000)

# Analyze market quality metrics
results.plot_volatility()
results.calculate_liquidity_metrics()
``` -->

## 📁 Project Structure

```text
MicroHedger
├── CMakeLists.txt
├── README.md
├── main.cpp
├── libs
│   ├── Bar.cpp
│   ├── Bar.hpp
│   ├── CMakeLists.txt
│   ├── DeltaHedger.cpp
│   ├── DeltaHedger.hpp
│   ├── LOB.cpp
│   ├── LOB.hpp
│   ├── Random.cpp
│   ├── Random.hpp
│   ├── Utils.cpp
│   └── Utils.hpp
└── tests
    ├── CMakeLists.txt
    ├── test_bar.cpp
    ├── test_lob.cpp
    ├── test_paired_vector_sort.cpp
    └── test_random.cpp
```

<!-- ## 🔧 Configuration

Simulation parameters can be configured through:

- **YAML configuration files** for experiment setup
- **Command-line arguments** for quick parameter changes
- **Python API** for programmatic control

### Example Configuration

```yaml
simulation:
  n_periods: 10000
  n_informed: 50
  n_noise: 100
  n_hedgers: 25
  
hedgers:
  gamma_position: "positive"  # positive, negative, neutral
  hedge_ratio: 0.5
  rebalance_frequency: 10
  
market:
  tick_size: 0.01
  fundamental_value: 100.0
  volatility: 0.02
``` -->

<!-- ## 📈 Experiments

### Replication Studies

1. **Baseline Comparison**: No hedgers vs. positive/negative gamma
2. **Volatility Analysis**: Market volatility under different gamma scenarios
3. **Liquidity Assessment**: Bid-ask spreads and market depth analysis
4. **Stress Testing**: Market behavior under extreme conditions

### Extended Analysis

1. **Parameter Sensitivity**: Robustness to different model parameters
2. **Alternative Specifications**: Different hedging strategies and frequencies
3. **Market Regime Analysis**: Bull vs. bear market conditions -->

<!-- ## Visualization

The project includes comprehensive visualization tools:

- **Time series plots** of price evolution and volatility
- **Market quality dashboards** with key metrics
- **Comparative analysis** across different gamma scenarios
- **Statistical summaries** and distribution plots -->

<!-- ## Testing

Run the test suite:

```bash
pytest tests/
``` -->

## Documentation

<!-- Detailed documentation is available in the `docs/` directory:

- **Model specification** and mathematical formulation
- **API reference** for all classes and functions
- **Tutorial notebooks** with step-by-step examples
- **Replication guide** with detailed methodology -->

<!-- ## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines. -->

<!-- ### Areas for Contribution

- **Model extensions** with additional agent types
- **Alternative market structures** (e.g., fragmented markets)
- **Performance optimizations** for large-scale simulations
- **Visualization enhancements** and interactive dashboards -->

## 📝 Citation

If you use MicroHedger in your research, please cite both this repository and the original paper:

```bibtex
@article{buis2024gamma,
  title={Gamma positioning and market quality},
  author={Buis, Boyd and Pieterse-Bloem, Mary and Verschoor, Willem FC and Zwinkels, Remco CJ},
  journal={Journal of Economic Dynamics and Control},
  volume={164},
  pages={104880},
  year={2024},
  publisher={Elsevier}
}

@misc{microhedger2024,
  title={MicroHedger: A Replication of Gamma Positioning and Market Quality},
  author={[Your Name]},
  year={2024},
  url={https://github.com/yilvas09/MicroHedger}
}
```

## Contact

For questions, suggestions, or collaboration opportunities, please:

- **Open an issue** on GitHub
- **Contact the maintainer** via email [yutongzhao2000@gmail.com](mailto:yutongzhao2000@gmail.com).
<!-- - **Join the discussion** in GitHub Discussions -->

## Acknowledgments

- **Original authors** Buis, Pieterse-Bloem, Verschoor, and Zwinkels for their foundational research
- **Open source community** for tools and libraries (e.g. Boost) used in this project

---

*This project is part of ongoing research in market microstructure and algorithmic trading. The simulation framework is designed for academic and research purposes.*
