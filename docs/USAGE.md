# QuantForge Usage

QuantForge is a cross-platform C++20 application built with CMake.

Validated environments:

```text
Windows / MSVC
Linux / GCC
```

The complete test suite is also executed automatically on both platforms through GitHub Actions.

---

## Requirements

### Common

- CMake >= 3.24
- C++20-compatible compiler
- Git

GoogleTest is retrieved automatically by CMake through `FetchContent` when tests are enabled.

No manual GoogleTest installation is required for the CMake build.

---

## Clone the repository

```bash
git clone https://github.com/NassimSahib/quantforge.git
cd quantforge
```

---

# Build

## Linux / GCC

Configure an optimized Release build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Build all targets:

```bash
cmake --build build --parallel
```

Run the complete test suite:

```bash
ctest --test-dir build --output-on-failure
```

The CLI executable is generated at:

```text
build/QuantForge.CLI/QuantForge.CLI
```

---

## Windows / MSVC

From a Visual Studio Developer PowerShell:

```powershell
cmake -S . -B build
```

Build the Release configuration:

```powershell
cmake --build build --config Release --parallel
```

Run the complete test suite:

```powershell
ctest --test-dir build -C Release --output-on-failure
```

The CLI executable is generated at approximately:

```text
build\QuantForge.CLI\Release\QuantForge.CLI.exe
```

The repository also contains the native Visual Studio solution:

```text
QuantForge.slnx
```

---

## Building without tests

CTest exposes the standard CMake option:

```text
BUILD_TESTING
```

Tests are enabled by default.

To build only QuantForge Core and CLI:

### Linux

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build --parallel
```

### Windows

```powershell
cmake -S . -B build -DBUILD_TESTING=OFF
cmake --build build --config Release --parallel
```

When testing is disabled, GoogleTest is not required.

---

# Portfolio CSV

QuantForge loads a portfolio from a CSV file.

Required header:

```csv
id,type,option_type,strike,maturity,quantity
```

Example:

```csv
id,type,option_type,strike,maturity,quantity
CALL_001,EUROPEAN_OPTION,CALL,100,1.0,2
PUT_001,EUROPEAN_OPTION,PUT,110,0.5,-3
CALL_002,EUROPEAN_OPTION,CALL,130,2.0,5
```

The repository includes an example portfolio at:

```text
examples/portfolio.csv
```

---

## CSV fields

| Field | Description |
|---|---|
| `id` | Instrument identifier |
| `type` | Currently `EUROPEAN_OPTION` |
| `option_type` | `CALL` or `PUT` |
| `strike` | Strictly positive strike |
| `maturity` | Maturity in years, strictly positive |
| `quantity` | Signed non-zero integer position quantity |

Position direction is represented by the quantity:

```text
quantity > 0  -> long
quantity < 0  -> short
```

`CALL` / `PUT` describes the payoff type and is independent from the long / short position direction.

---

# CLI modes

QuantForge provides three execution modes:

```text
portfolio pricing
pricing benchmark
Monte Carlo scaling benchmark
```

---

# 1. Portfolio pricing

Syntax:

```text
QuantForge.CLI <portfolio.csv> <spot> <riskFreeRate> <dividendYield> <volatility> <paths> <seed>
```

Arguments:

| Argument | Meaning |
|---|---|
| `portfolio.csv` | Portfolio definition |
| `spot` | Current underlying spot price |
| `riskFreeRate` | Continuously compounded risk-free rate |
| `dividendYield` | Continuously compounded dividend yield |
| `volatility` | Annualized volatility |
| `paths` | Monte Carlo paths per option price |
| `seed` | Deterministic RNG seed |

Example market scenario:

```text
spot = 120
riskFreeRate = 0.05
dividendYield = 0.02
volatility = 0.20
paths = 100000
seed = 42
```

---

## Linux

```bash
./build/QuantForge.CLI/QuantForge.CLI \
    examples/portfolio.csv \
    120 0.05 0.02 0.20 100000 42
```

---

## Windows

```powershell
.\build\QuantForge.CLI\Release\QuantForge.CLI.exe `
    examples\portfolio.csv `
    120 0.05 0.02 0.20 100000 42
```

Or on one line:

```powershell
.\build\QuantForge.CLI\Release\QuantForge.CLI.exe examples\portfolio.csv 120 0.05 0.02 0.20 100000 42
```

---

## Portfolio report

The generated report contains, for each position:

- instrument ID
- signed quantity
- Black-Scholes unit price
- Monte Carlo unit price
- absolute pricing error
- relative pricing error
- Black-Scholes position value
- Monte Carlo position value
- pricing timings

The portfolio summary includes:

- total Black-Scholes value
- total Monte Carlo value
- Delta
- Gamma
- Vega
- Theta
- Rho
- total pricing timings

Example summary:

```text
Total Black-Scholes value: 101.3479
Total Monte-Carlo value: 101.4733
```

---

# 2. Pricing benchmark

The pricing benchmark compares the execution cost of:

```text
Black-Scholes closed-form pricing
vs
sequential Monte Carlo pricing
```

Syntax:

```text
QuantForge.CLI benchmark <bsIterations> <mcIterations> <mcPaths> <seed>
```

Example:

```text
QuantForge.CLI benchmark 1000000 10 100000 42
```

---

## Linux

```bash
./build/QuantForge.CLI/QuantForge.CLI benchmark 1000000 10 100000 42
```

## Windows

```powershell
.\build\QuantForge.CLI\Release\QuantForge.CLI.exe benchmark 1000000 10 100000 42
```

The benchmark reports:

- Black-Scholes price
- Monte Carlo price
- total execution time
- average execution time
- Monte Carlo throughput
- Monte Carlo / Black-Scholes runtime ratio

Performance benchmarks should be executed in optimized Release builds.

See:

```text
docs/PERFORMANCE.md
```

for methodology and recorded benchmark results.

---

# 3. Monte Carlo scaling benchmark

The scaling benchmark measures how the parallel Monte Carlo implementation behaves as the worker count increases.

Syntax:

```text
QuantForge.CLI scaling <iterations> <paths> <seed>
```

Example:

```text
QuantForge.CLI scaling 20 1000000 42
```

---

## Linux

```bash
./build/QuantForge.CLI/QuantForge.CLI scaling 20 1000000 42
```

## Windows

```powershell
.\build\QuantForge.CLI\Release\QuantForge.CLI.exe scaling 20 1000000 42
```

The benchmark first measures the sequential Monte Carlo baseline and then tests parallel worker counts up to the hardware concurrency hint.

Typical worker counts:

```text
1
2
4
8
16
```

depending on the detected hardware concurrency.

For each configuration it reports:

- worker count
- average time per price
- Monte Carlo paths per second
- speedup vs sequential baseline
- resulting option price

`std::thread::hardware_concurrency()` is treated as a hint, not as a guarantee of the optimal worker count.

---

# Reproducibility

Monte Carlo engines use explicit deterministic seeds.

For the same:

```text
implementation
inputs
path count
worker configuration
seed
```

the result is reproducible.

For sequential Monte Carlo:

```text
same seed + same path count
```

produces the same random sequence.

For parallel Monte Carlo, changing the worker count changes how random streams are partitioned across workers.

Therefore:

```text
1 worker
2 workers
4 workers
8 workers
```

do not need to produce identical Monte Carlo realizations.

They should remain statistically consistent with the same theoretical option value.

---

# Error handling

QuantForge rejects invalid input rather than silently accepting malformed data.

Examples include:

- wrong number of CLI arguments
- malformed floating-point input
- malformed integer input
- missing CSV file
- invalid CSV header
- missing CSV fields
- unsupported instrument type
- unsupported option type
- invalid strike
- invalid maturity
- invalid quantity
- invalid market-data values
- zero Monte Carlo path count
- zero worker count where applicable

CLI failures return a non-zero process exit code.

Errors are reported using messages such as:

```text
QuantForge error: ...
```

instead of terminating with an uncaught exception.

---

# Example end-to-end Linux session

```bash
git clone https://github.com/NassimSahib/quantforge.git
cd quantforge

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

ctest --test-dir build --output-on-failure

./build/QuantForge.CLI/QuantForge.CLI \
    examples/portfolio.csv \
    120 0.05 0.02 0.20 100000 42

./build/QuantForge.CLI/QuantForge.CLI \
    benchmark 1000000 10 100000 42

./build/QuantForge.CLI/QuantForge.CLI \
    scaling 20 1000000 42
```

---

# Example end-to-end Windows session

```powershell
git clone https://github.com/NassimSahib/quantforge.git
cd quantforge

cmake -S . -B build
cmake --build build --config Release --parallel

ctest --test-dir build -C Release --output-on-failure

.\build\QuantForge.CLI\Release\QuantForge.CLI.exe examples\portfolio.csv 120 0.05 0.02 0.20 100000 42

.\build\QuantForge.CLI\Release\QuantForge.CLI.exe benchmark 1000000 10 100000 42

.\build\QuantForge.CLI\Release\QuantForge.CLI.exe scaling 20 1000000 42
```

---

# Continuous Integration

Every Pull Request targeting `main` and every push to `main` is automatically built and tested through GitHub Actions on:

```text
Linux / GCC
Windows / MSVC
```

The CI workflow verifies:

```text
CMake configuration
build
complete GoogleTest / CTest suite
```

Performance benchmarks are intentionally not used as CI pass/fail requirements because shared CI runners are not stable benchmarking environments.
