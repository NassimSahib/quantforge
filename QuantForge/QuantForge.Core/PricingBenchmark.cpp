#include "pch.h"

#include "PricingBenchmark.h"

#include "BlackScholesPricer.h"
#include "MonteCarloPricer.h"

#include <chrono>
#include <optional>
#include <stdexcept>

namespace quantforge::benchmarking {

	PricingBenchmarkResult PricingBenchmark::run(
		const quantforge::instruments::EuropeanOption& option,
		const quantforge::market::MarketData& marketData,
		std::size_t blackScholesIterations,
		std::size_t monteCarloIterations,
		std::size_t monteCarloPaths,
		std::uint64_t seed
	) const {

		if (blackScholesIterations == 0) {
			throw std::invalid_argument(
				"Black-Scholes iteration count must be strictly positive"
			);
		}

		if (monteCarloIterations == 0) {
			throw std::invalid_argument(
				"Monte-Carlo iteration count must be strictly positive"
			);
		}

		quantforge::pricing::BlackScholesPricer bsp;

		quantforge::pricing::MonteCarloPricer mcp{
			monteCarloPaths,
			seed
		};


		// Warm-up.
		//
		// These calls are deliberately outside the timed sections.
		// The first execution can be noisier because of cold caches,
		// runtime initialization, etc.
		bsp.price(option, marketData);
		mcp.price(option, marketData);


		double blackScholesAccumulator = 0.0;

		const auto blackScholesStart =
			std::chrono::steady_clock::now();

		for (std::size_t i = 0;
			i < blackScholesIterations;
			++i) {

			blackScholesAccumulator +=
				bsp.price(option, marketData);
		}

		const auto blackScholesEnd =
			std::chrono::steady_clock::now();


		double monteCarloAccumulator = 0.0;

		const auto monteCarloStart =
			std::chrono::steady_clock::now();

		for (std::size_t i = 0;
			i < monteCarloIterations;
			++i) {

			monteCarloAccumulator +=
				mcp.price(option, marketData);
		}

		const auto monteCarloEnd =
			std::chrono::steady_clock::now();


		const auto blackScholesTotalTime =
			std::chrono::duration_cast<std::chrono::nanoseconds>(
				blackScholesEnd - blackScholesStart
			);

		const auto monteCarloTotalTime =
			std::chrono::duration_cast<std::chrono::nanoseconds>(
				monteCarloEnd - monteCarloStart
			);


		const double blackScholesAverageNanoseconds =
			static_cast<double>(blackScholesTotalTime.count())
			/ static_cast<double>(blackScholesIterations);

		const double monteCarloAverageNanoseconds =
			static_cast<double>(monteCarloTotalTime.count())
			/ static_cast<double>(monteCarloIterations);


		const double blackScholesPrice =
			blackScholesAccumulator
			/ static_cast<double>(blackScholesIterations);

		const double monteCarloPrice =
			monteCarloAccumulator
			/ static_cast<double>(monteCarloIterations);


		std::optional<double> pathsPerSecond;

		const double monteCarloSeconds =
			std::chrono::duration<double>(
				monteCarloTotalTime
			).count();

		if (monteCarloSeconds > 0.0) {

			const double totalPaths =
				static_cast<double>(monteCarloPaths)
				* static_cast<double>(monteCarloIterations);

			pathsPerSecond =
				totalPaths / monteCarloSeconds;
		}


		std::optional<double> performanceRatio;

		if (blackScholesAverageNanoseconds > 0.0) {
			performanceRatio =
				monteCarloAverageNanoseconds
				/ blackScholesAverageNanoseconds;
		}


		return PricingBenchmarkResult{
			blackScholesPrice,
			monteCarloPrice,

			BenchmarkStats{
				blackScholesIterations,
				blackScholesTotalTime,
				blackScholesAverageNanoseconds
			},

			BenchmarkStats{
				monteCarloIterations,
				monteCarloTotalTime,
				monteCarloAverageNanoseconds
			},

			monteCarloPaths,
			pathsPerSecond,
			performanceRatio
		};
	}

}