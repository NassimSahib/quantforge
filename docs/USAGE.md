# QuantForge Usage

## Current environment

Validated:
- Windows x64
- Visual Studio / MSVC
- C++20

CMake and Linux/GCC support are planned before v1.0.

## Portfolio CSV

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

Fields:
- `id`: instrument identifier
- `type`: currently `EUROPEAN_OPTION`
- `option_type`: `CALL` or `PUT`
- `strike`: strictly positive
- `maturity`: years, strictly positive
- `quantity`: signed non-zero integer

## Portfolio pricing mode

```text
QuantForge.CLI <portfolio.csv> <spot> <riskFreeRate> <dividendYield> <volatility> <paths> <seed>
```

Example:

```text
QuantForge.CLI portfolio.csv 120 0.05 0.02 0.20 100000 42
```

The CSV describes the portfolio. Market data is supplied separately so the same portfolio can be revalued under different scenarios.

## Pricing benchmark mode

```text
QuantForge.CLI benchmark <bsIterations> <mcIterations> <mcPaths> <seed>
```

Example:

```text
QuantForge.CLI benchmark 1000000 10 100000 42
```

Run benchmark commands in **Release x64**.

## Scaling benchmark mode

```text
QuantForge.CLI scaling <iterations> <paths> <seed>
```

Example:

```text
QuantForge.CLI scaling 5 1000000 42
```

The benchmark tests sequential Monte Carlo and worker counts up to the hardware concurrency hint.

## Error handling

Rejected input includes:
- wrong argument count,
- malformed numbers,
- missing CSV,
- invalid CSV header,
- unsupported instrument / option type,
- invalid domain values,
- zero Monte Carlo path count.

CLI failures return non-zero and print `QuantForge error:` instead of allowing an uncaught exception to terminate the application.

## Reproducibility

Monte Carlo engines use explicit seeds.

Same implementation + same configuration + same seed gives deterministic results.

Changing the parallel worker count changes random-stream partitioning, so multi-worker prices do not need to exactly match the sequential realization.
