#pragma once

#include <chrono>
#include <cstddef>

namespace quantforge::benchmarking {

	struct MonteCarloScalingEntry {
		std::size_t workers{};

		double price{};

		std::chrono::nanoseconds totalTime{};
		double averageNanoseconds{};

		double pathsPerSecond{};
		double speedupVsSequential{};
	};

}