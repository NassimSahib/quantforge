#include "pch.h"

#include "MonteCarloScalingBenchmark.h"

#include <stdexcept>

namespace quantforge::benchmarking {

	class MonteCarloScalingBenchmarkTest :
		public ::testing::Test {

	protected:

		quantforge::instruments::EuropeanOption option{
			"CALL_001",
			1.0,
			100.0,
			quantforge::instruments::OptionType::Call
		};

		quantforge::market::MarketData marketData{
			120.0,
			0.05,
			0.02,
			0.20
		};
	};


	TEST_F(
		MonteCarloScalingBenchmarkTest,
		RejectsZeroIterations
	) {
		MonteCarloScalingBenchmark benchmark;

		EXPECT_THROW(
			benchmark.run(
				option,
				marketData,
				0,
				1000,
				42
			),
			std::invalid_argument
		);
	}


	TEST_F(
		MonteCarloScalingBenchmarkTest,
		RejectsZeroPaths
	) {
		MonteCarloScalingBenchmark benchmark;

		EXPECT_THROW(
			benchmark.run(
				option,
				marketData,
				1,
				0,
				42
			),
			std::invalid_argument
		);
	}


	TEST_F(
		MonteCarloScalingBenchmarkTest,
		ReturnsSequentialBaseline
	) {
		MonteCarloScalingBenchmark benchmark;

		const auto result =
			benchmark.run(
				option,
				marketData,
				2,
				1000,
				42
			);

		EXPECT_GT(
			result.sequentialPrice,
			0.0
		);

		EXPECT_GT(
			result.sequentialPathsPerSecond,
			0.0
		);

		EXPECT_EQ(
			result.sequential.iterations,
			2u
		);
	}


	TEST_F(
		MonteCarloScalingBenchmarkTest,
		IncludesOneWorkerConfiguration
	) {
		MonteCarloScalingBenchmark benchmark;

		const auto result =
			benchmark.run(
				option,
				marketData,
				1,
				1000,
				42
			);

		ASSERT_FALSE(
			result.parallelResults.empty()
		);

		EXPECT_EQ(
			result.parallelResults.front().workers,
			1u
		);
	}


	TEST_F(
		MonteCarloScalingBenchmarkTest,
		OneWorkerProducesSequentialPrice
	) {
		MonteCarloScalingBenchmark benchmark;

		const auto result =
			benchmark.run(
				option,
				marketData,
				1,
				10000,
				42
			);

		ASSERT_FALSE(
			result.parallelResults.empty()
		);

		EXPECT_DOUBLE_EQ(
			result.parallelResults.front().price,
			result.sequentialPrice
		);
	}


	TEST_F(
		MonteCarloScalingBenchmarkTest,
		ReturnsPositiveParallelThroughput
	) {
		MonteCarloScalingBenchmark benchmark;

		const auto result =
			benchmark.run(
				option,
				marketData,
				1,
				10000,
				42
			);

		for (const auto& entry :
			result.parallelResults) {

			EXPECT_GT(
				entry.pathsPerSecond,
				0.0
			);

			EXPECT_GE(
				entry.speedupVsSequential,
				0.0
			);
		}
	}

}