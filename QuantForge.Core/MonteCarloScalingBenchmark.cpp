#include "pch.h"

#include "MonteCarloScalingBenchmark.h"

#include "MonteCarloPricer.h"
#include "ParallelMonteCarloPricer.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>
#include <vector>

namespace quantforge::benchmarking {

	MonteCarloScalingBenchmarkResult
		MonteCarloScalingBenchmark::run(
			const quantforge::instruments::EuropeanOption& option,
			const quantforge::market::MarketData& marketData,
			std::size_t iterations,
			std::size_t paths,
			std::uint64_t seed
		) const {

		if (iterations == 0) {
			throw std::invalid_argument(
				"Iteration count must be strictly positive"
			);
		}

		if (paths == 0) {
			throw std::invalid_argument(
				"Path count must be strictly positive"
			);
		}


		// ----------------------------------------------------
		// Hardware information
		// ----------------------------------------------------

		const unsigned int hardwareHint =
			std::thread::hardware_concurrency();

		const std::size_t hardwareWorkers =
			hardwareHint == 0
			? 1
			: static_cast<std::size_t>(hardwareHint);


		// ----------------------------------------------------
		// Sequential baseline
		// ----------------------------------------------------

		quantforge::pricing::MonteCarloPricer sequentialPricer{
			paths,
			seed
		};

		// Warm-up outside timing.
		sequentialPricer.price(
			option,
			marketData
		);

		double sequentialAccumulator = 0.0;

		const auto sequentialStart =
			std::chrono::steady_clock::now();

		for (std::size_t i = 0;
			i < iterations;
			++i) {

			sequentialAccumulator +=
				sequentialPricer.price(
					option,
					marketData
				);
		}

		const auto sequentialEnd =
			std::chrono::steady_clock::now();


		const auto sequentialTotalTime =
			std::chrono::duration_cast<std::chrono::nanoseconds>(
				sequentialEnd - sequentialStart
			);


		const double sequentialAverageNanoseconds =
			static_cast<double>(
				sequentialTotalTime.count()
				)
			/ static_cast<double>(iterations);


		const double sequentialPrice =
			sequentialAccumulator
			/ static_cast<double>(iterations);


		const double sequentialSeconds =
			std::chrono::duration<double>(
				sequentialTotalTime
			).count();


		double sequentialPathsPerSecond = 0.0;

		if (sequentialSeconds > 0.0) {

			const double totalPaths =
				static_cast<double>(paths)
				* static_cast<double>(iterations);

			sequentialPathsPerSecond =
				totalPaths / sequentialSeconds;
		}


		// ----------------------------------------------------
		// Determine worker counts
		// ----------------------------------------------------

		std::vector<std::size_t> workerCounts;

		const std::size_t maximumWorkers =
			std::min(
				hardwareWorkers,
				paths
			);


		std::size_t workers = 1;

		while (workers <= maximumWorkers) {

			workerCounts.push_back(workers);

			if (workers > maximumWorkers / 2) {
				break;
			}

			workers *= 2;
		}


		// If hardware concurrency isn't a power of 2:
		//
		// e.g. 12 threads
		// results become:
		// 1, 2, 4, 8, 12

		if (
			workerCounts.empty()
			|| workerCounts.back() != maximumWorkers
			) {
			workerCounts.push_back(
				maximumWorkers
			);
		}


		// ----------------------------------------------------
		// Parallel benchmarks
		// ----------------------------------------------------

		std::vector<MonteCarloScalingEntry>
			parallelResults;

		parallelResults.reserve(
			workerCounts.size()
		);


		for (const std::size_t workerCount :
		workerCounts) {

			quantforge::pricing::ParallelMonteCarloPricer
				parallelPricer{
					paths,
					seed,
					workerCount
			};


			// Warm-up.
			parallelPricer.price(
				option,
				marketData
			);


			double accumulator = 0.0;


			const auto start =
				std::chrono::steady_clock::now();


			for (std::size_t i = 0;
				i < iterations;
				++i) {

				accumulator +=
					parallelPricer.price(
						option,
						marketData
					);
			}


			const auto end =
				std::chrono::steady_clock::now();


			const auto totalTime =
				std::chrono::duration_cast<
				std::chrono::nanoseconds
				>(
					end - start
				);


			const double averageNanoseconds =
				static_cast<double>(
					totalTime.count()
					)
				/ static_cast<double>(
					iterations
					);


			const double averagePrice =
				accumulator
				/ static_cast<double>(
					iterations
					);


			const double seconds =
				std::chrono::duration<double>(
					totalTime
				).count();


			double pathsPerSecond = 0.0;

			if (seconds > 0.0) {

				const double totalPaths =
					static_cast<double>(paths)
					* static_cast<double>(
						iterations
						);

				pathsPerSecond =
					totalPaths / seconds;
			}


			double speedup = 0.0;

			if (averageNanoseconds > 0.0) {

				speedup =
					sequentialAverageNanoseconds
					/ averageNanoseconds;
			}


			parallelResults.push_back(
				MonteCarloScalingEntry{
					workerCount,
					averagePrice,
					totalTime,
					averageNanoseconds,
					pathsPerSecond,
					speedup
				}
			);
		}


		return MonteCarloScalingBenchmarkResult{

			iterations,
			paths,

			hardwareWorkers,

			sequentialPrice,

			BenchmarkStats{
				iterations,
				sequentialTotalTime,
				sequentialAverageNanoseconds
			},

			sequentialPathsPerSecond,

			parallelResults
		};
	}

}