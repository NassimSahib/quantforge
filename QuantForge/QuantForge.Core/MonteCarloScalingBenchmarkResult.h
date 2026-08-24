#pragma once

#include "BenchmarkStats.h"
#include "MonteCarloScalingEntry.h"

#include <cstddef>
#include <vector>

namespace quantforge::benchmarking {

	struct MonteCarloScalingBenchmarkResult {

		std::size_t iterations{};
		std::size_t pathsPerPrice{};

		std::size_t hardwareConcurrency{};

		double sequentialPrice{};
		BenchmarkStats sequential{};
		double sequentialPathsPerSecond{};

		std::vector<MonteCarloScalingEntry> parallelResults{};
	};

}