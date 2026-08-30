# QuantForge Architecture

QuantForge is structured as a small quantitative-finance engine with a clear separation between:

```text
financial domain
pricing
portfolio analytics
I/O
reporting
benchmarking
application orchestration
testing
build infrastructure
```

The goal is not to create a large generic financial framework, but a compact codebase whose financial assumptions, ownership model, performance characteristics, and execution flow can be explained end-to-end.

---

# Design goals

QuantForge follows several explicit architectural rules:

1. Domain objects own their invariants.
2. Pricing logic is separated from portfolio orchestration.
3. Pricing engines remain stateless unless configuration state is genuinely required.
4. Calculation and presentation stay separate.
5. Sequential and parallel Monte Carlo remain independently usable and benchmarkable.
6. Deterministic behavior is preferred where it improves reproducibility.
7. Parsing validation and financial-domain validation remain separate.
8. Performance measurement is separated from correctness testing.
9. Platform-specific build details should not leak into financial code.
10. Abstractions are introduced only when they serve a concrete responsibility.

---

# Repository structure

```text
quantforge/
|
+-- .github/
|   +-- workflows/
|       +-- ci.yml
|
+-- docs/
|   +-- ARCHITECTURE.md
|   +-- PERFORMANCE.md
|   +-- TESTING.md
|   +-- USAGE.md
|
+-- examples/
|   +-- portfolio.csv
|
+-- QuantForge.Core/
|   +-- CMakeLists.txt
|   +-- domain
|   +-- pricing
|   +-- portfolio
|   +-- I/O
|   +-- reporting
|   +-- benchmarking
|
+-- QuantForge.CLI/
|   +-- CMakeLists.txt
|   +-- CliApplication
|   +-- main
|
+-- QuantForge.Tests/
|   +-- CMakeLists.txt
|   +-- GoogleTest suite
|
+-- CMakeLists.txt
+-- QuantForge.slnx
+-- README.md
```

The repository keeps both:

```text
CMake
```

for portable builds, and:

```text
QuantForge.slnx
```

for native Visual Studio development.

---

# High-level dependency graph

The main application dependencies are:

```text
                 QuantForge.Core
                 /             \
                /               \
               v                 v
      QuantForge.CLI      QuantForge.Tests
                                |
                                v
                           GoogleTest
```

`QuantForge.Core` contains financial and computational logic.

`QuantForge.CLI` depends on Core but does not contain pricing logic itself.

`QuantForge.Tests` validates Core behavior through GoogleTest.

---

# Core architecture

Conceptually, the Core library contains several layers:

```text
Domain
  |
  v
Pricing
  |
  v
Portfolio analytics
  |
  +----------+
  |          |
  v          v
Reporting  Benchmarking
```

I/O enters through portfolio loading and constructs validated domain objects.

---

# Domain layer

## MarketData

`MarketData` represents the market environment required by the current pricing models.

It stores:

```text
spot
risk-free rate
dividend yield
volatility
```

Validation rules include:

```text
spot > 0
volatility >= 0
all values finite
```

Risk-free rates and dividend yields may be negative.

Maturity deliberately does **not** belong to `MarketData`.

It belongs to the financial instrument.

This keeps:

```text
market state
```

separate from:

```text
contract definition
```

---

# Instrument hierarchy

The current hierarchy is:

```text
Instrument
    ^
    |
EuropeanOption
```

`Instrument` contains common instrument information such as:

```text
ID
maturity
```

and defines polymorphic behavior.

`EuropeanOption` adds:

```text
strike
Call / Put type
payoff logic
```

Current instrument classification uses:

```text
InstrumentType::EuropeanOption
```

and option direction uses:

```text
OptionType::Call
OptionType::Put
```

---

## Payoff model

At terminal spot \(S_T\):

```text
Call payoff = max(S_T - K, 0)

Put payoff  = max(K - S_T, 0)
```

This belongs to the instrument.

Portfolio direction is handled separately.

---

# Position ownership model

A `Position` stores:

```cpp
std::shared_ptr<const Instrument>
```

plus:

```cpp
std::int64_t quantity
```

The ownership model provides:

```text
shared lifetime
+
read-only instrument access
```

through the position.

Position direction is represented by the sign of quantity:

```text
quantity > 0  -> long
quantity < 0  -> short
```

This deliberately keeps two concepts separate:

```text
Call / Put
    =
payoff type

Long / Short
    =
portfolio exposure
```

---

# Portfolio

A `Portfolio` owns:

```cpp
std::vector<Position>
```

Its responsibility is intentionally small:

```text
store positions
preserve ordering
provide read-only access
```

It does not perform pricing itself.

This avoids turning the portfolio container into a large object responsible for unrelated financial calculations.

---

# Black-Scholes architecture

The analytical pricing path is built around:

```text
EuropeanOption
      +
MarketData
      |
      v
BlackScholesContext
      |
      +----------------+
      |                |
      v                v
BlackScholesPricer  BlackScholesGreeks
```

---

## BlackScholesContext

Pricing and Greeks require many of the same intermediate values:

```text
sqrt(T)
sigma * sqrt(T)
discount factors
d1
d2
N(d1)
N(d2)
phi(d1)
```

Rather than recomputing them independently, `BlackScholesContext` calculates and stores them once.

The context stores scalar values instead of references to the original option or market-data objects.

This makes its lifetime simple and removes unnecessary ownership concerns.

---

## BlackScholesPricer

The pricer supports conceptually two paths:

```text
price(option, marketData)
```

and:

```text
price(context)
```

The first provides a convenient domain-level interface.

The second reuses an already-constructed context.

---

## Zero-volatility case

The standard Black-Scholes context requires:

```text
sigma > 0
```

because \(d_1\) and \(d_2\) contain division by volatility.

Therefore zero volatility is handled explicitly by the higher-level pricing API using deterministic discounted intrinsic value.

This keeps the regular Black-Scholes context mathematically well-defined.

---

# Greeks architecture

`Greeks` is a plain result structure containing:

```text
Delta
Gamma
Vega
Theta
Rho
```

`BlackScholesGreeks` consumes a `BlackScholesContext`.

This keeps:

```text
input data
pricing context
price result
risk result
```

as distinct concepts.

---

# Sequential Monte Carlo

`MonteCarloPricer` owns configuration:

```text
number of paths
seed
```

The current project supports European terminal-payoff options, so terminal spot can be simulated directly.

For every path:

```text
Z ~ N(0,1)
      |
      v
simulate S_T
      |
      v
option.payoff(S_T)
      |
      v
accumulate payoff
```

After all paths:

```text
total payoff
     |
     v
average payoff
     |
     v
discount by exp(-rT)
     |
     v
price today
```

---

## Random-number generation

The sequential Monte Carlo engine uses:

```cpp
std::mt19937_64
std::normal_distribution<double>
```

with an explicit seed.

The generator is created once for a pricing call rather than reseeded for every path.

This gives deterministic reproducibility for identical inputs.

---

# Parallel Monte Carlo

`ParallelMonteCarloPricer` is implemented separately from the sequential pricer.

This is deliberate.

It preserves:

```text
a simple sequential reference implementation
```

and:

```text
an independently benchmarkable parallel implementation
```

rather than hiding both behaviors behind one complex execution path.

---

## Path partitioning

For:

```text
paths
workers
```

the work is distributed using:

```text
basePaths = paths / workers
remainder = paths % workers
```

The first `remainder` workers receive one additional path.

Example:

```text
10 paths
3 workers

worker 0 -> 4 paths
worker 1 -> 3 paths
worker 2 -> 3 paths
```

The actual worker count is limited by the number of paths.

---

# Parallel worker model

Each worker owns:

```text
its RNG
its distribution
its local payoff accumulator
```

Workers do **not** repeatedly update one global payoff sum.

Instead:

```text
worker 0 -> partialSums[0]
worker 1 -> partialSums[1]
worker 2 -> partialSums[2]
...
```

Each thread writes its final result into a distinct slot.

After all workers complete:

```cpp
std::accumulate(...)
```

reduces the partial sums.

---

## Why no mutex in the hot loop?

A design such as:

```text
all workers
    |
    v
shared payoffSum
```

would create a data race.

Protecting the shared value with a mutex for every simulated path would also introduce significant contention.

QuantForge instead uses:

```text
worker-local accumulation
        |
        v
one final write per worker
        |
        v
reduction after join
```

This removes synchronization from the Monte Carlo inner loop.

---

## Thread lifetime

Parallel workers use:

```cpp
std::jthread
```

The implementation relies on RAII for thread lifetime management.

When the thread container leaves scope, its `std::jthread` objects join automatically before the reduction is performed.

This avoids manual thread-join bookkeeping.

---

# RNG partitioning

Each worker uses its own seeded random-number generator.

This provides deterministic behavior for a fixed:

```text
path count
seed
worker count
```

Changing the number of workers changes the random stream partitioning.

Therefore:

```text
1 worker
2 workers
4 workers
8 workers
```

are not expected to generate exactly the same Monte Carlo realization.

They remain estimators of the same theoretical option value.

The current seed-per-worker approach is sufficient for this project, but stronger random-stream partitioning strategies could be explored in future versions.

---

# Portfolio analytics

`PortfolioAnalytics` performs portfolio-level aggregation.

Conceptually:

```text
for each Position
      |
      v
price / Greeks per unit
      |
      v
multiply by signed quantity
      |
      v
add to portfolio total
```

Therefore:

```text
unit result * quantity = position contribution
```

This supports both long and short exposures without changing instrument pricing logic.

---

# Pricing comparison

`PricingComparison` orchestrates:

```text
Black-Scholes price
Monte Carlo price
absolute error
relative error
Black-Scholes timing
Monte Carlo timing
```

The result is stored in:

```text
PricingComparisonResult
```

rather than mixed with presentation logic.

---

## Optional relative error

Relative error is undefined when the analytical Black-Scholes reference price is zero.

Therefore the result uses:

```cpp
std::optional<double>
```

instead of inventing a sentinel value.

---

# CSV ingestion

`CsvPortfolioLoader` reads:

```csv
id,type,option_type,strike,maturity,quantity
```

The parser is responsible for:

```text
file access
header validation
field count
strict numeric conversion
text-to-enum conversion
```

Domain classes remain responsible for:

```text
positive strike
positive maturity
non-zero quantity
finite values
```

This deliberately separates:

```text
input-format validation
```

from:

```text
financial-domain validation
```

---

# Reporting architecture

Reporting is split into two responsibilities.

## PortfolioReportBuilder

The builder performs calculations and produces structured report data.

```text
Portfolio
    |
    v
pricing per position
    |
    +--> BS price
    +--> MC price
    +--> error
    +--> position value
    +--> timings
    |
    v
PortfolioReport
```

It also aggregates:

```text
portfolio value
Greeks
timings
```

---

## ConsoleReportWriter

`ConsoleReportWriter` only formats a completed report.

It does not perform pricing.

This keeps:

```text
calculation
```

separate from:

```text
presentation
```

and makes the reporting logic independently testable through `std::ostringstream`.

---

# Benchmarking architecture

QuantForge contains two benchmark layers.

---

## PricingBenchmark

`PricingBenchmark` compares:

```text
Black-Scholes closed-form pricing
vs
sequential Monte Carlo
```

It records:

```text
price
iterations
total time
average time
Monte Carlo throughput
MC / BS runtime ratio
```

---

## MonteCarloScalingBenchmark

`MonteCarloScalingBenchmark` compares:

```text
sequential baseline
1 worker
2 workers
4 workers
8 workers
...
hardware concurrency
```

For each configuration it records:

```text
price
time
throughput
speedup
```

The sequential implementation measured in the same run is used as the speedup baseline.

This avoids comparing unrelated benchmark executions.

---

## Hardware concurrency

The benchmark uses:

```cpp
std::thread::hardware_concurrency()
```

as a hint for candidate worker counts.

It is not treated as the optimal worker count.

Measured results already show that the optimal configuration can vary significantly between operating systems and toolchains.

---

# CLI architecture

The executable intentionally contains very little financial logic.

`main()` forwards:

```text
argc
argv
stdout
stderr
```

to:

```text
CliApplication
```

Conceptually:

```text
OS / shell
    |
    v
main()
    |
    v
CliApplication
```

`CliApplication` handles three modes.

---

## Portfolio mode

```text
CLI arguments
     |
     v
MarketData
     +
CsvPortfolioLoader
     |
     v
Portfolio
     |
     v
PortfolioReportBuilder
     |
     v
ConsoleReportWriter
```

---

## Pricing benchmark mode

```text
CLI arguments
     |
     v
PricingBenchmark
     |
     v
BenchmarkReportWriter
```

---

## Scaling mode

```text
CLI arguments
     |
     v
MonteCarloScalingBenchmark
     |
     v
MonteCarloScalingReportWriter
```

This keeps command-line parsing and application orchestration out of the Core financial classes.

---

# Testing architecture

The test layer is a separate executable:

```text
QuantForge.Tests
      |
      +--> QuantForge.Core
      |
      +--> GoogleTest
```

GoogleTest is retrieved through CMake `FetchContent`.

CTest discovers and runs the individual tests.

The current suite contains:

```text
141 tests
```

covering domain rules, pricing, portfolio logic, Monte Carlo, parsing, reporting, benchmarking, and parallel behavior.

Detailed strategy:

```text
docs/TESTING.md
```

---

# CMake architecture

The repository uses multiple `CMakeLists.txt` files:

```text
CMakeLists.txt
|
+-- QuantForge.Core/CMakeLists.txt
|
+-- QuantForge.CLI/CMakeLists.txt
|
+-- QuantForge.Tests/CMakeLists.txt
```

The root file orchestrates the build.

---

## Core target

```text
QuantForge.Core
```

is a static library.

Platform output examples:

```text
Windows:
QuantForge.Core.lib

Linux:
libQuantForge.Core.a
```

It publicly exposes its include directory and C++20 requirement to dependent targets.

---

## CLI target

```text
QuantForge.CLI
```

is an executable linked against:

```text
QuantForge.Core
```

The dependency is expressed directly in the CMake target graph.

---

## Test target

```text
QuantForge.Tests
```

is linked against:

```text
QuantForge.Core
GTest::gtest_main
```

Tests can be disabled using:

```text
BUILD_TESTING=OFF
```

in which case GoogleTest is not needed.

---

# Cross-platform build model

The same CMake project has been validated with two toolchains:

```text
Windows
   |
   v
CMake
   |
   v
MSVC

Linux
   |
   v
CMake
   |
   v
GCC
```

No financial or pricing source code required platform-specific modification during the Linux/GCC validation.

---

# Continuous Integration architecture

GitHub Actions validates the repository independently from the development machine.

```text
Push / Pull Request
        |
        v
   GitHub Actions
      /       \
     /         \
    v           v
Ubuntu       Windows
 GCC           MSVC
    |           |
  CMake       CMake
    |           |
  Build       Build
    |           |
  CTest       CTest
    |           |
141 tests   141 tests
```

The two jobs are independent:

```text
Linux / GCC
Windows / MSVC
```

This makes platform-specific failures immediately visible on Pull Requests.

---

# CI vs benchmarking

Continuous integration validates:

```text
correctness
portability
build integrity
```

It deliberately does not validate fixed performance thresholds.

Published benchmarks are run separately because CI runners are not stable performance environments.

This preserves a clean separation between:

```text
Does the code work?
```

and:

```text
How fast does the code run?
```

---

# Current supported scope

Implemented in the current V1:

- European Call / Put options
- constant spot-market inputs
- constant risk-free rate
- constant dividend yield
- constant volatility
- Black-Scholes pricing
- analytical Greeks
- sequential Monte Carlo
- parallel Monte Carlo
- portfolio aggregation
- CSV portfolio ingestion
- pricing comparison
- console reports
- pricing benchmarks
- parallel scaling benchmarks
- CMake
- Windows / MSVC
- Linux / GCC
- GoogleTest / CTest
- GitHub Actions CI

---

# Intentionally outside V1 scope

The following are intentionally excluded from the initial release:

- American options
- exotic options
- path-dependent derivatives
- stochastic volatility
- local volatility
- live market data
- yield-curve framework
- database integration
- networking
- production trade lifecycle
- distributed computing
- production-grade RNG stream management
- calibration infrastructure

Possible future extensions are tracked separately from the core V1 architecture.

---

# Architectural summary

The complete execution model can be summarized as:

```text
                 Market / Portfolio input
                          |
                          v
                     Domain objects
                          |
           +--------------+--------------+
           |                             |
           v                             v
   Analytical pricing             Monte Carlo
           |                      /          \
           |                 Sequential    Parallel
           |                      |          |
           +-----------+----------+----------+
                       |
                       v
               Portfolio analytics
                       |
                       v
                  Report models
                       |
              +--------+--------+
              |                 |
              v                 v
          Console           Benchmarks
```

Around this runtime architecture:

```text
CMake
  |
  +--> Windows / MSVC
  |
  +--> Linux / GCC

GoogleTest + CTest
  |
  +--> 141 correctness tests

GitHub Actions
  |
  +--> Windows validation
  |
  +--> Linux validation
```

The result is a compact C++20 quantitative-finance project whose financial model, ownership decisions, concurrency strategy, build process, correctness checks, and performance behavior remain independently understandable.
