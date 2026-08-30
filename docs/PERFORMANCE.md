# QuantForge Performance

This document records performance measurements observed while developing QuantForge.

The results are **machine-, compiler-, operating-system-, and workload-dependent**. They are intended to document the behavior of the implementation and the impact of parallelization, not to provide universal performance guarantees.

---

## Benchmark environments

QuantForge was benchmarked in optimized Release builds on two environments running on the same development machine:

### Windows

```text
Operating system: Windows
Compiler: MSVC
Architecture: x64
Build system: CMake / Visual Studio
Configuration: Release
```

### Linux

```text
Operating system: Linux under WSL2
Compiler: GCC 15.2
Architecture: x86_64
Build system: CMake
Configuration: Release
```

The Linux results are WSL2 measurements and should not be interpreted as native-Linux hardware benchmarks.

---

## Fixed financial scenario

Unless otherwise stated, benchmarks use:

```text
Instrument: European Call

S = 120
K = 100
T = 1 year

r = 0.05
q = 0.02
sigma = 0.20

seed = 42
```

This fixed scenario makes performance measurements across code revisions easier to compare.

---

# Pricing benchmark

The pricing benchmark compares the cost of:

```text
Black-Scholes closed-form pricing
vs
Monte Carlo simulation
```

Command:

```text
QuantForge.CLI benchmark 1000000 10 100000 42
```

Parameters:

```text
Black-Scholes iterations = 1,000,000
Monte Carlo iterations   = 10
Monte Carlo paths/price  = 100,000
seed                     = 42
```

---

## Windows / MSVC

Observed:

| Metric | Result |
|---|---:|
| Black-Scholes price | 24.06 |
| Black-Scholes average | **57.54 ns / price** |
| Monte Carlo price | 24.15 |
| Monte Carlo average | **2.601 ms / price** |
| Monte Carlo throughput | **38.44 M paths/s** |
| MC / BS runtime ratio | **45,206x** |

The analytical Black-Scholes implementation is extremely cheap compared with a Monte Carlo price requiring 100,000 simulated terminal states.

The ratio does **not** imply that Monte Carlo is an inferior pricing technique. It quantifies the computational cost of simulation when a closed-form solution is available.

---

## Linux / GCC under WSL2

Using the same scenario and workload:

| Metric | Result |
|---|---:|
| Black-Scholes price | 24.06 |
| Black-Scholes average | **448.72 ns / price** |
| Monte Carlo price | 24.15 |
| Monte Carlo average | **8.079 ms / price** |
| Monte Carlo throughput | **12.38 M paths/s** |
| MC / BS runtime ratio | **18,005x** |

For this sequential benchmark, Windows/MSVC was substantially faster than the measured WSL2/GCC environment.

Approximate observed differences:

```text
Black-Scholes:
Windows ~7.8x faster

Sequential Monte Carlo:
Windows ~3.1x faster
```

These results should not be generalized to Windows vs Linux performance overall.

The measurements involve different compilers, runtime environments, scheduling behavior, and WSL2 virtualization.

---

# Parallel Monte Carlo scaling

The scaling benchmark evaluates the ability of `ParallelMonteCarloPricer` to distribute Monte Carlo paths across multiple workers.

Each option price uses:

```text
1,000,000 Monte Carlo paths
```

The implementation uses:

```text
std::jthread
worker-local RNG state
worker-local payoff accumulation
partial-sum reduction
```

There is no shared payoff accumulator in the simulation hot loop.

---

# Windows / MSVC scaling

Command used:

```text
QuantForge.CLI scaling 5 1000000 42
```

Configuration:

```text
Iterations: 5
Paths per price: 1,000,000
Hardware concurrency: 16
```

Observed:

| Workers | Avg. time / price | Throughput | Speedup vs sequential |
|---:|---:|---:|---:|
| Sequential | 45.675 ms | 21.89 M paths/s | 1.00x |
| 1 | 54.602 ms | 18.31 M paths/s | 0.84x |
| 2 | 30.636 ms | 32.64 M paths/s | 1.49x |
| 4 | 17.025 ms | 58.74 M paths/s | 2.68x |
| **8** | **9.870 ms** | **101.32 M paths/s** | **4.63x** |
| 16 | 11.131 ms | 89.84 M paths/s | 4.10x |

Best observed Windows configuration:

```text
8 workers
9.87 ms / price
101.32 M paths/s
4.63x speedup
```

Performance peaked at 8 workers.

Moving from 8 to 16 workers reduced throughput.

---

# Linux / GCC scaling under WSL2

A longer run was used to obtain a more stable measurement:

```text
QuantForge.CLI scaling 20 1000000 42
```

Configuration:

```text
Iterations: 20
Paths per price: 1,000,000
Hardware concurrency: 16
```

Observed:

| Workers | Avg. time / price | Throughput | Speedup vs sequential |
|---:|---:|---:|---:|
| Sequential | 106.765 ms | 9.37 M paths/s | 1.00x |
| 1 | 108.703 ms | 9.20 M paths/s | 0.98x |
| 2 | 61.665 ms | 16.22 M paths/s | 1.73x |
| 4 | 30.448 ms | 32.84 M paths/s | 3.51x |
| 8 | 13.942 ms | 71.73 M paths/s | 7.66x |
| **16** | **7.348 ms** | **136.09 M paths/s** | **14.53x** |

Best observed Linux/WSL2 configuration:

```text
16 workers
7.35 ms / price
136.09 M paths/s
14.53x speedup
```

The measured parallel efficiency relative to the sequential baseline was approximately:

```text
14.53 / 16 ~= 90.8%
```

at 16 workers.

This is not a claim of universal 90% scaling efficiency. It describes this particular benchmark run.

---

# Cross-platform scaling comparison

The most interesting result is that sequential and parallel performance tell different stories.

| Metric | Windows / MSVC | Linux / GCC under WSL2 |
|---|---:|---:|
| Sequential time | **45.68 ms** | 106.76 ms |
| Sequential throughput | **21.89 M paths/s** | 9.37 M paths/s |
| Best worker count | 8 | **16** |
| Best parallel time | 9.87 ms | **7.35 ms** |
| Best throughput | 101.32 M paths/s | **136.09 M paths/s** |
| Best speedup | 4.63x | **14.53x** |

The sequential Windows implementation was approximately:

```text
2.34x faster
```

than the WSL2/GCC sequential baseline.

However, the Linux/WSL2 run scaled much more effectively with additional workers.

At the best observed configurations:

```text
Windows:
8 workers
101.32 M paths/s

Linux / WSL2:
16 workers
136.09 M paths/s
```

The measured Linux/WSL2 throughput was therefore approximately:

```text
34% higher
```

than the best Windows throughput in these runs.

Again, this result describes the tested environments rather than a general Linux-vs-Windows performance conclusion.

---

# Why one worker can be slower

Parallel execution has fixed costs:

- thread creation,
- OS scheduling,
- thread startup,
- joining,
- destruction,
- partial-result reduction.

Therefore:

```text
ParallelMonteCarloPricer(workers = 1)
```

does not necessarily outperform:

```text
MonteCarloPricer
```

even though both eventually use one execution thread for the simulation work.

Observed:

### Windows

```text
Sequential: 45.68 ms
1 worker:   54.60 ms
```

### Linux / WSL2

```text
Sequential: 106.76 ms
1 worker:   108.70 ms
```

This demonstrates that concurrency is not free.

---

# Why more workers are not always faster

`std::thread::hardware_concurrency()` returns a hardware concurrency **hint**.

It does not identify the optimal worker count.

Performance can be affected by:

- physical vs logical cores,
- scheduling overhead,
- cache behavior,
- CPU frequency changes,
- execution-resource contention,
- thread lifecycle overhead,
- operating-system scheduling,
- compiler optimization behavior.

This is visible in the Windows results:

```text
8 workers  -> 4.63x
16 workers -> 4.10x
```

Adding workers after the measured sweet spot reduced performance.

In contrast, the WSL2/GCC run continued scaling through all 16 reported hardware threads.

---

# Why scaling speedup must use the same-run baseline

A previous Windows pricing benchmark measured:

```text
38.44 M paths/s
```

while the Windows scaling benchmark sequential baseline measured:

```text
21.89 M paths/s
```

These results should not be directly compared for speedup calculations.

The workloads differ:

```text
Pricing benchmark:
100,000 paths / price
10 Monte Carlo iterations

Scaling benchmark:
1,000,000 paths / price
multiple worker configurations
```

They were also executed at different times under potentially different system conditions.

Therefore QuantForge calculates scaling speedup using the sequential baseline collected in the **same scaling benchmark run**.

---

# Benchmark methodology

QuantForge benchmarks follow several rules.

## Release builds

Performance claims are based on optimized Release configurations.

Debug builds are not used for benchmark reporting.

---

## Monotonic clock

Timing uses:

```cpp
std::chrono::steady_clock
```

because elapsed-time measurement must not be affected by system-clock adjustments.

---

## Warm-up

A pricing call is executed before the timed loop.

This reduces noise related to first-use effects.

---

## Observable results

Pricing results are accumulated so optimized builds cannot trivially remove the benchmarked calculation as unused work.

---

## Multiple iterations

The scaling benchmark averages multiple full pricing runs.

The latest documented Linux scaling result uses:

```text
20 iterations
```

to reduce run-to-run noise.

---

## Same-run baseline

Parallel speedup is calculated relative to the sequential implementation measured during the same benchmark execution.

---

# Performance tests vs correctness tests

Performance is deliberately **not** encoded as a GoogleTest correctness requirement.

QuantForge does not assert conditions such as:

```text
8 workers must be faster than 4
parallel speedup must exceed 4x
throughput must exceed 100 M paths/s
```

Such expectations depend on:

- hardware,
- operating system,
- compiler,
- scheduler,
- current machine load.

The automated test suite checks correctness and structural benchmark outputs.

Dedicated benchmark modes measure performance separately.

---

# CI and performance

GitHub Actions automatically builds and tests QuantForge on:

```text
Windows / MSVC
Linux / GCC
```

However, GitHub-hosted runners are **not used for published performance measurements**.

Shared CI infrastructure does not provide a sufficiently stable environment for meaningful microbenchmark comparisons.

CI answers:

```text
Does it build?
Do the tests pass?
```

Dedicated benchmarks answer:

```text
How fast is this implementation on this environment?
How does it scale?
```

These concerns are intentionally separated.

---

# Current performance conclusions

The measurements currently support the following observations:

1. Black-Scholes closed-form pricing is orders of magnitude cheaper than a 100,000-path Monte Carlo price.

2. Parallel Monte Carlo substantially improves throughput for sufficiently large workloads.

3. Thread creation and scheduling overhead can make a one-worker parallel implementation slower than the dedicated sequential implementation.

4. `hardware_concurrency()` should not be treated as the optimal worker count.

5. The optimal worker count is platform- and environment-dependent.

6. Windows/MSVC produced substantially stronger sequential performance in the measured environments.

7. WSL2/GCC exhibited substantially stronger parallel scaling in the measured runs.

8. The best observed throughput to date was approximately:

```text
136.09 million Monte Carlo paths / second
```

using 16 workers under WSL2/GCC.

---

# Future performance work

Potential extensions include:

- persistent worker pool
- antithetic variates
- control variates
- confidence-interval reporting
- quasi-Monte Carlo
- SIMD/vectorization experiments
- cache-line-aware worker result storage
- improved independent RNG stream construction
- native Linux benchmarking outside WSL2
- compiler comparison between MSVC, GCC, and Clang

These are intentionally outside the initial V1 scope.
