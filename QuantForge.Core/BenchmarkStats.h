#pragma once

#include <chrono>
#include <cstddef>

namespace quantforge::benchmarking {

	struct BenchmarkStats {
		std::size_t iterations{};
		std::chrono::nanoseconds totalTime{};
		double averageNanoseconds{};
	};
}