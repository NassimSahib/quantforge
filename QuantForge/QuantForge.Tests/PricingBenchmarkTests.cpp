#include "pch.h"

#include "PricingBenchmark.h"

#include <stdexcept>

namespace quantforge::benchmarking {

	class PricingBenchmarkTest : public ::testing::Test {
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
		PricingBenchmarkTest,
		RejectsZeroBlackScholesIterations
	) {
		PricingBenchmark benchmark;

		EXPECT_THROW(
			benchmark.run(
				option,
				marketData,
				0,
				1,
				1000,
				42
			),
			std::invalid_argument
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		RejectsZeroMonteCarloIterations
	) {
		PricingBenchmark benchmark;

		EXPECT_THROW(
			benchmark.run(
				option,
				marketData,
				100,
				0,
				1000,
				42
			),
			std::invalid_argument
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		RejectsZeroMonteCarloPaths
	) {
		PricingBenchmark benchmark;

		EXPECT_THROW(
			benchmark.run(
				option,
				marketData,
				100,
				1,
				0,
				42
			),
			std::invalid_argument
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		ReturnsRequestedIterationCounts
	) {
		PricingBenchmark benchmark;

		const PricingBenchmarkResult result =
			benchmark.run(
				option,
				marketData,
				1000,
				2,
				1000,
				42
			);

		EXPECT_EQ(
			result.blackScholes.iterations,
			1000u
		);

		EXPECT_EQ(
			result.monteCarlo.iterations,
			2u
		);

		EXPECT_EQ(
			result.monteCarloPaths,
			1000u
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		ReturnsPositivePricingResults
	) {
		PricingBenchmark benchmark;

		const PricingBenchmarkResult result =
			benchmark.run(
				option,
				marketData,
				1000,
				2,
				1000,
				42
			);

		EXPECT_GT(
			result.blackScholesPrice,
			0.0
		);

		EXPECT_GT(
			result.monteCarloPrice,
			0.0
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		ReturnsNonNegativeExecutionTimes
	) {
		PricingBenchmark benchmark;

		const PricingBenchmarkResult result =
			benchmark.run(
				option,
				marketData,
				1000,
				2,
				1000,
				42
			);

		EXPECT_GE(
			result.blackScholes.totalTime.count(),
			0
		);

		EXPECT_GE(
			result.monteCarlo.totalTime.count(),
			0
		);

		EXPECT_GE(
			result.blackScholes.averageNanoseconds,
			0.0
		);

		EXPECT_GE(
			result.monteCarlo.averageNanoseconds,
			0.0
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		MonteCarloPriceRemainsCloseToBlackScholes
	) {
		PricingBenchmark benchmark;

		const PricingBenchmarkResult result =
			benchmark.run(
				option,
				marketData,
				1000,
				2,
				100000,
				42
			);

		EXPECT_NEAR(
			result.monteCarloPrice,
			result.blackScholesPrice,
			0.2
		);
	}


	TEST_F(
		PricingBenchmarkTest,
		ComputesMonteCarloThroughput
	) {
		PricingBenchmark benchmark;

		const PricingBenchmarkResult result =
			benchmark.run(
				option,
				marketData,
				1000,
				2,
				10000,
				42
			);

		ASSERT_TRUE(
			result.monteCarloPathsPerSecond.has_value()
		);

		EXPECT_GT(
			result.monteCarloPathsPerSecond.value(),
			0.0
		);
	}

}