# QuantForge Testing Strategy

QuantForge uses GoogleTest.

## Coverage

The suite validates:
- domain invariants,
- financial formulas,
- edge cases,
- deterministic Monte Carlo behavior,
- portfolio aggregation,
- CSV parsing,
- report generation,
- benchmark plumbing,
- parallel pricing correctness.

## MarketData

Tests include:
- valid values,
- zero / negative rates where allowed,
- zero volatility,
- invalid spot,
- negative volatility,
- infinity,
- NaN.

## EuropeanOption

Tests include:
- Call payoff,
- Put payoff,
- strike validation,
- maturity,
- instrument type.

## Position / Portfolio

Tests include:
- null instrument rejection,
- non-zero quantity,
- long / short quantities,
- empty portfolio,
- insertion,
- order.

## Black-Scholes / Greeks

Tests validate:
- context values,
- Call / Put pricing,
- zero-volatility case,
- Delta,
- Gamma,
- Vega,
- Theta,
- Rho.

## Monte Carlo

Tests include:
- zero-path rejection,
- zero-volatility deterministic pricing,
- same-seed reproducibility,
- Call / Put proximity to Black-Scholes.

Not tested:

```text
more paths must always be closer to Black-Scholes
```

Monte Carlo convergence is statistical, not monotonic for a single realization.

## CSV

Temporary files test:
- valid load,
- missing file,
- empty file,
- invalid header,
- unsupported enums,
- missing fields,
- malformed numbers,
- strict numeric parsing,
- extra columns.

## Pricing comparison

Tests cover:
- Black-Scholes result,
- Monte Carlo result,
- absolute error,
- optional relative error,
- zero-reference-price behavior,
- non-negative timings.

## Reporting

Writer tests use `std::ostringstream`.

They validate important content rather than every character of a full report.

## Parallel Monte Carlo

Tests include:
- zero paths,
- zero workers,
- one-worker equality with sequential,
- reproducibility,
- zero-volatility behavior,
- proximity to Black-Scholes,
- more workers than paths.

## Scaling benchmark

Tests validate:
- invalid input rejection,
- sequential baseline creation,
- inclusion of one-worker configuration,
- one-worker price equality,
- positive throughput.

Tests intentionally do not assert that additional workers must improve speed.

## Build configurations

Correctness is expected in both:

```text
Debug x64
Release x64
```

Performance claims are made only from Release measurements.
