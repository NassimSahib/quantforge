#pragma once

#include "MonteCarloScalingBenchmarkResult.h"
#include "EuropeanOption.h"
#include "MarketData.h"

#include <cstddef>
#include <cstdint>

namespace quantforge::benchmarking {

	class MonteCarloScalingBenchmark {
	public:

		MonteCarloScalingBenchmarkResult run(
			const quantforge::instruments::EuropeanOption& option,
			const quantforge::market::MarketData& marketData,
			std::size_t iterations,
			std::size_t paths,
			std::uint64_t seed
		) const;
	};

}