#pragma once

#include "BenchmarkStats.h"

#include <cstddef>
#include <optional>

namespace quantforge::benchmarking {

	struct PricingBenchmarkResult {
		double blackScholesPrice{};
		double monteCarloPrice{};

		BenchmarkStats blackScholes{};
		BenchmarkStats monteCarlo{};

		std::size_t monteCarloPaths{};

		std::optional<double> monteCarloPathsPerSecond{};
		std::optional<double> monteCarloToBlackScholesRatio{};
	};

}