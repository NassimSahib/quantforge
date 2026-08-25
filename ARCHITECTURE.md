# QuantForge Architecture

## Design goals

QuantForge follows a few explicit rules:

1. Domain objects own their invariants.
2. Pricing engines are stateless unless configuration state is genuinely required.
3. Calculation and presentation stay separate.
4. Sequential and parallel Monte Carlo remain independently benchmarkable.
5. Deterministic tests are preferred where possible.
6. Abstractions are added only when they serve a concrete responsibility.

## Solution structure

```text
QuantForge
|
+-- QuantForge.Core
|   +-- domain / market
|   +-- instruments
|   +-- portfolio
|   +-- pricing
|   +-- I/O
|   +-- reporting
|   +-- benchmarking
|
+-- QuantForge.CLI
|   +-- CliApplication
|   +-- main
|
+-- QuantForge.Tests
    +-- GoogleTest suite
```

## Domain layer

### MarketData

Contains spot, risk-free rate, dividend yield, and volatility.

Validation:
- spot finite and strictly positive,
- rates finite,
- volatility finite and non-negative.

Maturity belongs to the instrument, not market data.

### Instrument hierarchy

```text
Instrument
    ^
    |
EuropeanOption
```

`EuropeanOption` adds strike, Call/Put type, and payoff logic.

### Position / Portfolio

A `Position` owns a:

```cpp
std::shared_ptr<const Instrument>
```

plus signed quantity.

A `Portfolio` owns:

```cpp
std::vector<Position>
```

Positive quantity means long; negative quantity means short.

## Black-Scholes

`BlackScholesContext` precomputes:

```text
sqrt(T)
sigma*sqrt(T)
discount factors
d1
d2
N(d1)
N(d2)
phi(d1)
```

This supports both pricing and Greeks without recomputing shared quantities.

```text
EuropeanOption + MarketData
            |
            v
BlackScholesContext
       |           |
       v           v
    price       Greeks
```

## Monte Carlo

`MonteCarloPricer` owns `paths` and `seed`.

For each path:

```text
Z ~ N(0,1)
   |
   v
terminal S_T
   |
   v
payoff(S_T)
   |
   v
accumulate
```

After all paths:

```text
mean payoff
   |
discount
   |
price today
```

RNG:
- `std::mt19937_64`
- `std::normal_distribution<double>`

## Portfolio analytics

`PortfolioAnalytics` aggregates price and Greeks using signed quantities.

```text
unit result x quantity = position contribution
```

## Pricing comparison

`PricingComparison` orchestrates:
- Black-Scholes price,
- Monte Carlo price,
- absolute error,
- optional relative error,
- timings.

`PricingComparisonResult` only stores data.

## CSV ingestion

`CsvPortfolioLoader` validates file structure and strict conversion.

Domain classes validate financial invariants.

This keeps parsing validation separate from business validation.

## Reporting

`PortfolioReportBuilder` builds structured report data.

`ConsoleReportWriter` only formats output.

```text
calculation != presentation
```

## Parallel Monte Carlo

`ParallelMonteCarloPricer` partitions paths across workers.

```text
basePaths = paths / workers
remainder = paths % workers
```

Each worker owns:
- RNG state,
- a local payoff sum.

Workers write one final value to different positions in `partialSums`.

After all `std::jthread`s join, `std::accumulate` performs the reduction.

No mutex is used in the hot simulation loop.

## Benchmarking

`PricingBenchmark` compares analytical Black-Scholes and sequential Monte Carlo.

`MonteCarloScalingBenchmark` compares:
- sequential,
- 1 worker,
- 2 workers,
- 4 workers,
- 8 workers,
- up to `hardware_concurrency()`.

## CLI

Three modes exist:

```text
portfolio mode
benchmark mode
scaling mode
```

`main()` stays minimal and forwards `argc`, `argv`, `stdout`, and `stderr` to `CliApplication`.

## Current scope

Implemented:
- European options,
- constant market parameters,
- Black-Scholes,
- Greeks,
- sequential Monte Carlo,
- portfolio aggregation,
- CSV,
- reports,
- benchmarks,
- multithreaded Monte Carlo.

Out of current V1 scope:
- American/exotic options,
- stochastic volatility,
- live data,
- database/networking,
- production trade lifecycle.
