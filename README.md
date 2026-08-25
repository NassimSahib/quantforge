# QuantForge

**QuantForge** is a C++20 mini options desk built to practice quantitative finance, modern C++, testing, performance engineering, and multithreading in one coherent project.

It prices and analyzes portfolios of European options from CSV input using both **Black-Scholes** and **Monte Carlo**, computes portfolio Greeks, compares pricing methods, reports execution times, and includes a multithreaded Monte Carlo implementation with scaling benchmarks.

> **Project status:** pre-v1.0. The current codebase is validated on Windows x64 with MSVC. CMake, Linux/GCC support, and CI are the next milestones before the v1.0 tag.

## Highlights

- European **Call / Put** instruments
- Validated market data
- Analytical **Black-Scholes pricing**
- Analytical **Delta, Gamma, Vega, Theta, Rho**
- Sequential Monte Carlo pricing under risk-neutral GBM
- Deterministic RNG through explicit seeds
- Portfolio positions with long/short quantities
- CSV portfolio ingestion
- Portfolio-level valuation and Greeks aggregation
- Black-Scholes vs Monte Carlo comparison
- Console reporting
- Release-mode benchmarking
- Parallel Monte Carlo with `std::jthread`
- Automatic scaling tests using `std::thread::hardware_concurrency()`
- GoogleTest coverage across pricing, parsing, reporting, portfolio logic, and concurrency

## Performance snapshot

Measured on the development machine in **Release x64**.

### Pricing benchmark

Scenario:

```text
European Call
S = 120
K = 100
T = 1
r = 5%
q = 2%
sigma = 20%
```

| Engine | Workload | Average time | Throughput |
|---|---:|---:|---:|
| Black-Scholes | 1,000,000 prices | **57.54 ns / price** | — |
| Monte Carlo | 100,000 paths / price | **2.60 ms / price** | **38.44 M paths/s** |

In that run, Monte Carlo was about **45,206x** slower per option price than the closed-form Black-Scholes calculation, illustrating the cost difference between an analytical solution and simulation.

### Parallel Monte Carlo scaling

Scaling benchmark: **1,000,000 paths per price**, 5 iterations.

| Workers | Avg. time / price | Throughput | Speedup vs sequential |
|---:|---:|---:|---:|
| Sequential | 45.68 ms | 21.89 M paths/s | 1.00x |
| 1 | 54.60 ms | 18.31 M paths/s | 0.84x |
| 2 | 30.64 ms | 32.64 M paths/s | 1.49x |
| 4 | 17.03 ms | 58.74 M paths/s | 2.68x |
| **8** | **9.87 ms** | **101.32 M paths/s** | **4.63x** |
| 16 | 11.13 ms | 89.84 M paths/s | 4.10x |

The best observed configuration was **8 workers**, reaching more than **101 million Monte Carlo paths per second** and a **4.63x speedup** over the sequential baseline in the same scaling run.

> Benchmark results are machine- and workload-dependent. They are included as reproducible project measurements, not universal performance claims. See [`docs/PERFORMANCE.md`](docs/PERFORMANCE.md).

## Architecture

```text
CSV portfolio
     |
     v
CsvPortfolioLoader
     |
     v
Portfolio + MarketData
     |
     +------------------------+
     |                        |
     v                        v
BlackScholesPricer      MonteCarloPricer
     |                        |
     +-----------+------------+
                 |
                 v
         PricingComparison
                 |
                 v
       PortfolioReportBuilder
                 |
        +--------+--------+
        |                 |
        v                 v
PortfolioAnalytics   ConsoleReportWriter
        |
        v
     Greeks
```

The multithreaded path is kept separate from the sequential implementation:

```text
MonteCarloPricer
    |
    +--> sequential baseline

ParallelMonteCarloPricer
    |
    +--> path partitioning
    +--> worker-local RNG
    +--> local payoff sums
    +--> join
    +--> reduction
```

More detail: [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).

## Pricing

### Black-Scholes

For a dividend-paying underlying:

\[
C = S e^{-qT}N(d_1) - K e^{-rT}N(d_2)
\]

\[
P = K e^{-rT}N(-d_2) - S e^{-qT}N(-d_1)
\]

with

\[
d_1 =
\frac{\ln(S/K) + (r-q+\frac{1}{2}\sigma^2)T}
{\sigma\sqrt{T}}
\]

\[
d_2 = d_1 - \sigma\sqrt{T}
\]

### Monte Carlo

\[
S_T =
S_0 \exp
\left[
\left(r-q-\frac{1}{2}\sigma^2\right)T
+
\sigma\sqrt{T}Z
\right]
\]

with \(Z \sim \mathcal{N}(0,1)\).

The option value is estimated as:

\[
V_0 \approx
e^{-rT}
\frac{1}{N}
\sum_{i=1}^{N}
\text{Payoff}(S_T^{(i)})
\]

## CLI

### Portfolio pricing

```text
QuantForge.CLI <portfolio.csv> <spot> <riskFreeRate> <dividendYield> <volatility> <paths> <seed>
```

Example:

```text
QuantForge.CLI portfolio.csv 120 0.05 0.02 0.20 100000 42
```

### Pricing benchmark

```text
QuantForge.CLI benchmark <bsIterations> <mcIterations> <mcPaths> <seed>
```

Example:

```text
QuantForge.CLI benchmark 1000000 10 100000 42
```

### Monte Carlo scaling benchmark

```text
QuantForge.CLI scaling <iterations> <paths> <seed>
```

Example:

```text
QuantForge.CLI scaling 5 1000000 42
```

Full usage details: [`docs/USAGE.md`](docs/USAGE.md).

## CSV format

```csv
id,type,option_type,strike,maturity,quantity
CALL_001,EUROPEAN_OPTION,CALL,100,1.0,2
PUT_001,EUROPEAN_OPTION,PUT,110,0.5,-3
CALL_002,EUROPEAN_OPTION,CALL,130,2.0,5
```

The CSV describes the **portfolio**. Market conditions are supplied separately to the CLI, allowing the same portfolio to be revalued under different market scenarios.

## Tests

The project uses **GoogleTest**.

Coverage includes market-data invariants, option validation and payoffs, Black-Scholes, Greeks, Monte Carlo reproducibility, CSV parsing, portfolio aggregation, reporting, benchmarking, parallel Monte Carlo, and scaling behavior.

See [`docs/TESTING.md`](docs/TESTING.md).

## Current build environment

Validated:

- C++20
- Visual Studio / MSVC
- Windows x64
- Debug and Release
- GoogleTest

### Next before v1.0

- CMake
- Linux / GCC build
- GitHub Actions CI
- clean build-from-clone instructions

After those milestones, the project will be tagged **v1.0**.

## Roadmap after v1.0

Possible extensions:

- antithetic variates
- control variates
- confidence intervals
- persistent thread pool
- quasi-Monte Carlo
- implied volatility solver
- yield curves
- American / exotic options

The project intentionally stops before becoming an open-ended pricing framework: the goal is a compact, explainable, testable C++ quant-dev portfolio project.

## Disclaimer

QuantForge is an educational and portfolio project. It is not intended for production trading, investment advice, or live risk management.
