# QuantForge Performance

All results below were measured on the development machine in **Release x64**. They are machine- and workload-specific.

## Fixed financial scenario

```text
European Call
S = 120
K = 100
T = 1
r = 0.05
q = 0.02
sigma = 0.20
seed = 42
```

## Pricing benchmark

Command:

```text
QuantForge.CLI benchmark 1000000 10 100000 42
```

Observed:

| Metric | Result |
|---|---:|
| Black-Scholes price | 24.06 |
| Black-Scholes average | **57.54 ns / price** |
| Monte Carlo price | 24.15 |
| Monte Carlo average | **2.601 ms / price** |
| Monte Carlo throughput | **38.44 M paths/s** |
| MC / BS ratio | **45,206x** |

This quantifies the computational cost of simulation relative to an available closed-form solution.

## Parallel scaling benchmark

Command:

```text
QuantForge.CLI scaling 5 1000000 42
```

`std::thread::hardware_concurrency()` returned `16`.

| Mode | Avg. time / price | Throughput | Speedup |
|---|---:|---:|---:|
| Sequential | 45.675 ms | 21.89 M paths/s | 1.00x |
| 1 worker | 54.602 ms | 18.31 M paths/s | 0.84x |
| 2 workers | 30.636 ms | 32.64 M paths/s | 1.49x |
| 4 workers | 17.025 ms | 58.74 M paths/s | 2.68x |
| **8 workers** | **9.870 ms** | **101.32 M paths/s** | **4.63x** |
| 16 workers | 11.131 ms | 89.84 M paths/s | 4.10x |

Best observed configuration:

```text
8 workers
~9.87 ms / 1,000,000-path price
~101.32 M paths/s
4.63x speedup
```

## Interpretation

### Why one worker is slower

The parallel implementation still pays for:
- thread creation,
- scheduling,
- join,
- destruction.

So one `std::jthread` can be slower than direct sequential execution.

### Why 16 workers are slower than 8

`hardware_concurrency()` is a hint, not the optimal worker count.

Possible factors:
- logical vs physical cores,
- scheduling overhead,
- execution-resource contention,
- cache behavior,
- thread lifecycle overhead.

### Why single-thread throughput differs across benchmark runs

The earlier pricing benchmark used 100,000 paths per price.

The scaling benchmark used 1,000,000 paths per price.

They were also separate executions. Therefore the correct multithreading comparison is the sequential baseline measured inside the same scaling run.

## Benchmark methodology

- `std::chrono::steady_clock`
- warm-up before timed loops
- result accumulation to keep work observable to the optimizer
- Release x64 only for performance claims
- no machine-dependent speed assertions in unit tests

## Future performance work

Potential future experiments:
- persistent worker pool,
- antithetic variates,
- control variates,
- quasi-Monte Carlo,
- SIMD,
- cache-line-aware partial results,
- stronger independent RNG stream construction.
