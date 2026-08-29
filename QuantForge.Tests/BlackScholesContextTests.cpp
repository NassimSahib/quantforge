#include "pch.h"
#include "BlackScholesContext.h"
#include "EuropeanOption.h"
#include "MarketData.h"
#include <cmath>
#include <stdexcept>

namespace quantforge::pricing {

	class BlackScholesContextTest : public ::testing::Test
	{
	protected:
		quantforge::instruments::EuropeanOption option{
			"OPT_AAPL_001",
			1.0,
			100.0,
			quantforge::instruments::OptionType::Call
		};

		quantforge::market::MarketData marketData{
			100.0,
			0.05,
			0.02,
			0.20
		};
	};
	TEST_F(BlackScholesContextTest, PreservesInputValues) {
		BlackScholesContext context(option, marketData);
		EXPECT_DOUBLE_EQ(context.spot(),100.0);
		EXPECT_DOUBLE_EQ(context.strike(), 100.0);
		EXPECT_DOUBLE_EQ(context.maturity(), 1.0);
		EXPECT_DOUBLE_EQ(context.riskFreeRate(), 0.05);
		EXPECT_DOUBLE_EQ(context.dividendYield(), 0.02);
		EXPECT_EQ(context.optionType(), quantforge::instruments::OptionType::Call);
		EXPECT_DOUBLE_EQ(context.volatility(), 0.20);
	}

	TEST_F(BlackScholesContextTest, ComputesTimeAndDiscountTerms) {
		BlackScholesContext context(option, marketData);
		EXPECT_DOUBLE_EQ(context.sqrtT(), std::sqrt(1.0));
		EXPECT_DOUBLE_EQ(context.volSqrtT(), 0.20 * std::sqrt(1.0));
		EXPECT_DOUBLE_EQ(context.discountDividend(), std::exp(-0.02 * 1.0));
		EXPECT_DOUBLE_EQ(context.discountRate(), std::exp(-0.05 * 1.0));
	}

	TEST_F(BlackScholesContextTest, ComputesD1AndD2) {
		BlackScholesContext context(option, marketData);
		EXPECT_NEAR(context.d1(), 0.25, 0.001);
		EXPECT_NEAR(context.d2(), 0.05, 0.001);
	}

	TEST_F(BlackScholesContextTest, ComputesNormalDistributionTerms) {
		BlackScholesContext context(option, marketData);
		EXPECT_NEAR(context.nd1(), 0.5987063257, 0.001);
		EXPECT_NEAR(context.nd2(), 0.5199388058, 0.001);
		EXPECT_NEAR(context.normalPdfD1(), 0.3866681168, 0.001);
	}

	TEST_F(BlackScholesContextTest, RejectsZeroVolatility) {
		quantforge::market::MarketData mdZeroVol(100.0, 0.05, 0.02, 0.0);
		EXPECT_THROW(BlackScholesContext context(option, mdZeroVol),
				     std::invalid_argument);
	}

}
