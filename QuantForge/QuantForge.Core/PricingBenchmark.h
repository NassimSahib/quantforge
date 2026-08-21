#pragma once

#include "PricingBenchmarkResult.h"
#include "EuropeanOption.h"
#include "MarketData.h"

#include <cstddef>
#include <cstdint>

namespace quantforge::benchmarking {

	class PricingBenchmark {
	public:
		PricingBenchmarkResult run(
			const quantforge::instruments::EuropeanOption& option,
			const quantforge::market::MarketData& marketData,
			std::size_t blackScholesIterations,
			std::size_t monteCarloIterations,
			std::size_t monteCarloPaths,
			std::uint64_t seed
		) const;
	};

}