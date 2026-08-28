#include "pch.h"

#include "PricingComparison.h"
#include "BlackScholesPricer.h"
#include "MonteCarloPricer.h"
#include "EuropeanOption.h"
#include "MarketData.h"

#include <cmath>

namespace quantforge::pricing {

	class PricingComparisonTest : public ::testing::Test {
	protected:
		quantforge::instruments::EuropeanOption callOption{
			"OPT_AAPL_001",
			1.0,
			100.0,
			quantforge::instruments::OptionType::Call
		};

		quantforge::instruments::EuropeanOption putOption{
			"OPT_AAPL_002",
			1.0,
			100.0,
			quantforge::instruments::OptionType::Put
		};

		quantforge::market::MarketData zeroVolMarketData{
			120.0,
			0.05,
			0.02,
			0.0
		};

		quantforge::market::MarketData basicMarketData{
			120.0,
			0.05,
			0.02,
			0.20
		};
	};


	TEST_F(PricingComparisonTest, ReturnsBlackScholesPrice) {
		PricingComparison pc;
		BlackScholesPricer bsp;

		const PricingComparisonResult result =
			pc.compare(callOption, basicMarketData, 1000, 42);

		const double expectedPrice =
			bsp.price(callOption, basicMarketData);

		EXPECT_DOUBLE_EQ(
			result.blackScholesPrice,
			expectedPrice
		);
	}


	TEST_F(PricingComparisonTest, ReturnsMonteCarloPrice) {
		PricingComparison pc;
		MonteCarloPricer mcp{ 1000, 42 };

		const PricingComparisonResult result =
			pc.compare(callOption, basicMarketData, 1000, 42);

		const double expectedPrice =
			mcp.price(callOption, basicMarketData);

		EXPECT_DOUBLE_EQ(
			result.monteCarloPrice,
			expectedPrice
		);
	}


	TEST_F(PricingComparisonTest, ComputesAbsoluteError) {
		PricingComparison pc;

		const PricingComparisonResult result =
			pc.compare(callOption, basicMarketData, 1000, 42);

		const double expectedError =
			std::abs(
				result.monteCarloPrice
				- result.blackScholesPrice
			);

		EXPECT_DOUBLE_EQ(
			result.absoluteError,
			expectedError
		);
	}


	TEST_F(PricingComparisonTest, ComputesRelativeError) {
		PricingComparison pc;

		const PricingComparisonResult result =
			pc.compare(callOption, basicMarketData, 1000, 42);

		ASSERT_TRUE(result.relativeError.has_value());

		const double expectedError =
			result.absoluteError
			/ std::abs(result.blackScholesPrice);

		EXPECT_DOUBLE_EQ(
			result.relativeError.value(),
			expectedError
		);
	}


	TEST_F(PricingComparisonTest, ExecutionTimesAreNonNegative) {
		PricingComparison pc;

		const PricingComparisonResult result =
			pc.compare(callOption, basicMarketData, 1000, 42);

		EXPECT_GE(
			result.blackScholesTime.count(),
			0
		);

		EXPECT_GE(
			result.monteCarloTime.count(),
			0
		);
	}

	TEST_F(PricingComparisonTest, ZeroBlackScholesPriceHasNoRelativeError) {
		PricingComparison pc;

		const quantforge::market::MarketData zeroPriceMarketData{
			50.0,
			0.05,
			0.02,
			0.0
		};

		const PricingComparisonResult result =
			pc.compare(
				callOption,
				zeroPriceMarketData,
				1000,
				42
			);

		EXPECT_DOUBLE_EQ(
			result.blackScholesPrice,
			0.0
		);

		EXPECT_FALSE(
			result.relativeError.has_value()
		);
	}

}