# QuantForge Testing Strategy

QuantForge uses **GoogleTest** for correctness testing and **CTest** for cross-platform test discovery and execution.

The current suite contains **141 tests** covering financial-domain invariants, pricing models, Monte Carlo behavior, portfolio logic, parsing, reporting, benchmarking infrastructure, and multithreaded pricing.

The complete suite has been validated on:

```text
Windows / MSVC
Linux / GCC
```

and is executed automatically on both platforms through GitHub Actions.

---

## Testing goals

The test suite is designed to validate:

- domain invariants
- financial formulas
- edge cases
- deterministic behavior where determinism is expected
- statistical consistency where exact equality is inappropriate
- portfolio aggregation
- strict CSV parsing
- report generation
- benchmark result correctness
- parallel Monte Carlo correctness
- cross-platform behavior

Performance itself is deliberately separated from correctness testing.

---

# Running the tests

## Linux / GCC

Configure and build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run all tests:

```bash
ctest --test-dir build --output-on-failure
```

Expected result:

```text
100% tests passed
0 tests failed out of 141
```

---

## Windows / MSVC

Configure and build:

```powershell
cmake -S . -B build
cmake --build build --config Release --parallel
```

Run all tests:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

The suite can also be run through Visual Studio's Test Explorer.

---

# Test architecture

The test executable depends on:

```text
QuantForge.Tests
      |
      +--> QuantForge.Core
      |
      +--> GoogleTest
```

CMake retrieves GoogleTest automatically through `FetchContent`.

The tests are registered with CTest using:

```cmake
include(GoogleTest)

gtest_discover_tests(
    QuantForge.Tests
)
```

This allows the same GoogleTest suite to be discovered and executed consistently on Windows and Linux.

---

# MarketData

Tests validate the invariants of the market-data object.

Covered cases include:

- valid market data
- zero risk-free rate
- negative risk-free rate
- zero dividend yield
- negative dividend yield
- zero volatility
- zero spot rejection
- negative spot rejection
- negative volatility rejection
- positive infinity rejection
- NaN rejection

Important domain rules include:

```text
spot > 0
volatility >= 0
rates must be finite
```

Negative rates and dividend yields are intentionally supported.

---

# Instrument and EuropeanOption

Tests cover:

- valid instrument construction
- instrument ID validation
- maturity validation
- instrument classification
- European Call payoff
- European Put payoff
- strike validation
- terminal spot validation

The payoff definitions are validated independently from portfolio position direction.

```text
Call / Put
```

defines the instrument payoff.

```text
Long / Short
```

is represented later through the signed position quantity.

---

# Position

Tests validate:

- non-null instrument requirement
- non-zero quantity
- positive quantity
- negative quantity
- instrument access
- quantity access

A position stores an immutable instrument reference through:

```cpp
std::shared_ptr<const Instrument>
```

The tests therefore also protect the ownership assumptions used throughout the portfolio layer.

---

# Portfolio

Tests cover:

- empty portfolio
- position insertion
- portfolio size
- position ordering
- read-only access to stored positions

The portfolio itself deliberately performs very little financial logic.

Pricing and risk aggregation are tested separately.

---

# Black-Scholes context

`BlackScholesContext` tests validate the precomputed quantities shared by pricing and Greeks.

Covered values include:

- spot
- strike
- maturity
- rates
- volatility
- square root of maturity
- volatility-time term
- discount factors
- \(d_1\)
- \(d_2\)
- normal CDF values
- normal PDF value

The context requires strictly positive volatility because the usual Black-Scholes \(d_1\) and \(d_2\) expressions divide by volatility.

Zero volatility is handled explicitly by the higher-level pricing API.

---

# Black-Scholes pricing

Tests validate:

- European Call pricing
- European Put pricing
- known numerical reference values
- zero-volatility deterministic pricing
- pricing through a precomputed context
- pricing through the convenience option/market API

The zero-volatility case uses the deterministic discounted payoff rather than attempting to construct a regular Black-Scholes context.

---

# Greeks

Analytical Black-Scholes tests cover:

- Delta
- Gamma
- Vega
- Theta
- Rho

Numerical tolerances are used where appropriate.

The tests validate the formulas rather than forcing exact floating-point equality when the result depends on transcendental functions.

---

# Sequential Monte Carlo

Tests cover:

- zero-path rejection
- zero-volatility deterministic pricing
- same-seed reproducibility
- European Call proximity to Black-Scholes
- European Put proximity to Black-Scholes

The Monte Carlo implementation uses:

```text
std::mt19937_64
std::normal_distribution<double>
explicit seed
```

so identical sequential configurations produce deterministic realizations.

---

## Statistical testing principle

The suite deliberately does **not** test:

```text
more Monte Carlo paths must always produce a price closer to Black-Scholes
```

That is not a valid deterministic property.

Monte Carlo convergence is statistical:

```text
error scale ~ 1 / sqrt(N)
```

but an individual larger sample may still happen to lie farther from the analytical value than a smaller sample.

Tests therefore use statistically reasonable tolerances rather than invalid monotonic assumptions.

---

# Portfolio analytics

Tests validate:

- empty portfolio value
- long-position contribution
- short-position contribution
- total portfolio value
- aggregated Delta
- aggregated Gamma
- aggregated Vega
- aggregated Theta
- aggregated Rho

Each unit result is multiplied by the signed position quantity before aggregation.

This explicitly protects the distinction:

```text
Call / Put != Long / Short
```

---

# CSV portfolio loader

CSV tests use temporary files to validate both successful loading and malformed input.

Covered cases include:

- valid portfolio
- missing file
- empty file
- invalid header
- unsupported instrument type
- unsupported option type
- missing fields
- malformed floating-point values
- malformed integer values
- strict numeric conversion
- extra columns
- invalid domain values delegated to domain objects

The parser performs structural validation while domain classes remain responsible for financial invariants.

---

# Pricing comparison

Tests validate:

- Black-Scholes result
- Monte Carlo result
- absolute pricing error
- relative pricing error
- undefined relative error when the Black-Scholes reference is zero
- non-negative Black-Scholes timing
- non-negative Monte Carlo timing

Relative error is represented using:

```cpp
std::optional<double>
```

because division by a zero analytical reference is not meaningful.

---

# Portfolio report builder

Tests validate:

- one result per portfolio position
- instrument IDs
- signed quantities
- Black-Scholes unit prices
- Monte Carlo unit prices
- position values
- portfolio totals
- aggregated Greeks
- timing aggregation
- deterministic Monte Carlo seeds per position

The builder uses deterministic seed offsets for reproducible report generation.

---

# Report writers

Writer tests use:

```cpp
std::ostringstream
```

instead of the real terminal.

This keeps formatting tests independent from `std::cout`.

Tests check important semantic content such as:

- section headers
- pricing values
- quantities
- optional relative-error formatting
- totals
- Greeks
- timing fields

They deliberately avoid asserting every character of a complete formatted report when doing so would make the tests unnecessarily brittle.

---

# Pricing benchmark

Benchmark tests validate the structure and correctness of benchmark output.

Covered behavior includes:

- invalid iteration count rejection
- invalid path count rejection
- Black-Scholes result
- Monte Carlo result
- iteration counts
- non-negative timings
- average timing calculations
- Monte Carlo throughput
- optional Monte Carlo / Black-Scholes runtime ratio

The tests validate benchmark calculations.

They do **not** enforce hardware-dependent speed expectations.

---

# Parallel Monte Carlo

Parallel pricing tests cover:

- zero-path rejection
- zero-worker rejection
- one-worker equivalence with sequential Monte Carlo
- deterministic results for identical configuration
- zero-volatility deterministic pricing
- proximity to Black-Scholes
- worker count greater than path count

---

## Why the one-worker equivalence test matters

With:

```text
same path count
same seed
one parallel worker
```

the sequential and one-worker parallel implementations consume the same random stream in the same order.

Therefore exact equivalence provides a strong deterministic correctness test for the parallel implementation.

---

# Concurrency correctness

The parallel implementation avoids a shared payoff accumulator.

Instead:

```text
worker 0 -> partialSums[0]
worker 1 -> partialSums[1]
worker 2 -> partialSums[2]
...
```

Each worker owns its RNG and local payoff accumulator.

After all `std::jthread` workers have completed, the partial results are reduced.

The design avoids:

```text
data races
hot-loop mutex contention
shared RNG state
```

Unit tests protect the externally visible behavior of this design.

---

# Monte Carlo scaling benchmark

Scaling benchmark tests validate:

- zero-iteration rejection
- zero-path rejection
- sequential baseline creation
- worker configuration generation
- inclusion of the one-worker case
- one-worker price equivalence with sequential Monte Carlo
- positive throughput values
- structurally valid timing information

Tests intentionally do **not** assert:

```text
2 workers must beat 1 worker
8 workers must beat 4 workers
parallel speedup must exceed 1
```

Those properties depend on the machine and runtime environment.

Actual scaling is measured through the dedicated CLI benchmark.

---

# What is intentionally not unit-tested

Several tempting assertions would be incorrect or too environment-dependent.

QuantForge intentionally avoids tests such as:

```text
Monte Carlo with more paths must always be closer to Black-Scholes
```

```text
parallel Monte Carlo must always be faster than sequential
```

```text
16 workers must be faster than 8 workers
```

```text
throughput must exceed a fixed number of paths per second
```

These are performance or statistical observations, not deterministic correctness invariants.

---

# Debug and Release

Correctness has been validated under both Visual Studio configurations:

```text
Debug x64
Release x64
```

The CMake build and complete CTest suite have also been validated in optimized Release builds on:

```text
Windows / MSVC
Linux / GCC
```

Performance claims are based only on optimized benchmark runs.

---

# Continuous Integration

GitHub Actions automatically executes the build and test suite for:

```text
Pull Requests -> main
Pushes -> main
```

Two independent jobs are used:

```text
Linux / GCC
Windows / MSVC
```

Each job performs:

```text
checkout
    |
    v
CMake configure
    |
    v
build
    |
    v
CTest
    |
    v
141 tests
```

A Pull Request is therefore automatically checked against both supported toolchains.

---

# Why benchmarks are excluded from CI pass/fail rules

GitHub-hosted runners are not stable performance-testing environments.

Runner performance may vary because of:

- host hardware
- virtualization
- current system load
- CPU allocation
- scheduling behavior

Therefore CI validates:

```text
correctness
portability
build integrity
```

while dedicated benchmark runs validate:

```text
latency
throughput
scaling
```

Performance measurements are documented separately in:

```text
docs/PERFORMANCE.md
```

---

# Current validation status

At the current v1.0 release-candidate stage:

```text
GoogleTest tests: 141

Windows / MSVC:
141 / 141 passed

Linux / GCC:
141 / 141 passed

GitHub Actions:
Linux / GCC     PASS
Windows / MSVC  PASS
```

This gives QuantForge a reproducible correctness baseline before future pricing and performance extensions are introduced.
