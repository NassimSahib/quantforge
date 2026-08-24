#include "pch.h"

#include "ParallelMonteCarloPricer.h"
#include "MonteCarloPricer.h"
#include "BlackScholesPricer.h"

#include <algorithm>
#include <cmath>

namespace quantforge::pricing {

	class ParallelMonteCarloPricerTest :
		public ::testing::Test {

	protected:

		quantforge::instruments::EuropeanOption callOption{
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

		quantforge::market::MarketData zeroVolMarketData{
			120.0,
			0.05,
			0.02,
			0.0
		};
	};


	TEST_F(
		ParallelMonteCarloPricerTest,
		RejectsZeroPaths
	) {
		EXPECT_THROW(
			ParallelMonteCarloPricer(
				0,
				42,
				4
			),
			std::invalid_argument
		);
	}


	TEST_F(
		ParallelMonteCarloPricerTest,
		RejectsZeroWorkers
	) {
		EXPECT_THROW(
			ParallelMonteCarloPricer(
				1000,
				42,
				0
			),
			std::invalid_argument
		);
	}


	TEST_F(
		ParallelMonteCarloPricerTest,
		OneWorkerMatchesSequentialPricer
	) {
		constexpr std::size_t paths = 10000;
		constexpr std::uint64_t seed = 42;

		MonteCarloPricer sequential{
			paths,
			seed
		};

		ParallelMonteCarloPricer parallel{
			paths,
			seed,
			1
		};

		EXPECT_DOUBLE_EQ(
			parallel.price(
				callOption,
				marketData
			),
			sequential.price(
				callOption,
				marketData
			)
		);
	}


	TEST_F(
		ParallelMonteCarloPricerTest,
		SameConfigurationProducesSamePrice
	) {
		ParallelMonteCarloPricer pricer1{
			10000,
			42,
			4
		};

		ParallelMonteCarloPricer pricer2{
			10000,
			42,
			4
		};

		EXPECT_DOUBLE_EQ(
			pricer1.price(
				callOption,
				marketData
			),
			pricer2.price(
				callOption,
				marketData
			)
		);
	}


	TEST_F(
		ParallelMonteCarloPricerTest,
		ZeroVolatilityMatchesDeterministicPrice
	) {
		ParallelMonteCarloPricer pricer{
			10000,
			42,
			4
		};

		const double expected =
			std::max(
				zeroVolMarketData.spot()
				* std::exp(
					-zeroVolMarketData.dividendYield()
					* callOption.maturity()
				)
				- callOption.strike()
				* std::exp(
					-zeroVolMarketData.riskFreeRate()
					* callOption.maturity()
				),
				0.0
			);

		EXPECT_NEAR(
			pricer.price(
				callOption,
				zeroVolMarketData
			),
			expected,
			0.001
		);
	}


	TEST_F(
		ParallelMonteCarloPricerTest,
		PriceIsCloseToBlackScholes
	) {
		ParallelMonteCarloPricer parallel{
			100000,
			42,
			4
		};

		BlackScholesPricer bsp;

		EXPECT_NEAR(
			parallel.price(
				callOption,
				marketData
			),
			bsp.price(
				callOption,
				marketData
			),
			0.25
		);
	}


	TEST_F(
		ParallelMonteCarloPricerTest,
		HandlesMoreWorkersThanPaths
	) {
		ParallelMonteCarloPricer pricer{
			2,
			42,
			16
		};

		EXPECT_NO_THROW(
			pricer.price(
				callOption,
				marketData
			)
		);
	}

}