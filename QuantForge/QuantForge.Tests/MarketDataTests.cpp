#include "pch.h"
#include "MarketData.h"
#include <stdexcept>
#include <limits>


namespace quantforge::market {

	TEST(MarketDataTest, ConstructWithValidValues) {
		MarketData md(100.0, 0.03, 0.01, 0.20);
		EXPECT_DOUBLE_EQ(md.spot(), 100.0);
		EXPECT_DOUBLE_EQ(md.riskFreeRate(), 0.03);
		EXPECT_DOUBLE_EQ(md.dividendYield(), 0.01);
		EXPECT_DOUBLE_EQ(md.volatility(), 0.20);
	}
	
	TEST(MarketDataTest, AcceptsRiskFreeRateEqualsZero) {
		MarketData md(100.0, 0, 0.01, 0.20);
		EXPECT_DOUBLE_EQ(md.riskFreeRate(),0);
	}
	TEST(MarketDataTest, AcceptsDividendYieldEqualsZero) {
		MarketData md(100.0, 0.01, 0, 0.20);
		EXPECT_DOUBLE_EQ(md.dividendYield(),0);
	}

	TEST(MarketDataTest, AcceptsVolatilityEqualsZero) {
		MarketData md(100.0, 0.01, 0.01, 0);
		EXPECT_DOUBLE_EQ(md.volatility(),0);
	}
	TEST(MarketDataTest, AcceptsNegativeRiskFreeRate) {
		MarketData md(100.0, -0.02, 0.01, 0.20);
		EXPECT_DOUBLE_EQ(md.riskFreeRate(), -0.02);
	}
	TEST(MarketDataTest, AcceptsNegativeDividendYield) {
		MarketData md(100.0, 0.02, -0.01, 0.10);
		EXPECT_DOUBLE_EQ(md.dividendYield(), -0.01);
	}


	TEST(MarketDataTest, RejectZeroSpot) {
		EXPECT_THROW(MarketData md(0, -0.02, -0.01, 0),
			std::invalid_argument);
	}
	TEST(MarketDataTest, RejectNegativeSpot) {
		EXPECT_THROW(MarketData md(-100.00, 0.02, 0.01, 0.12),
			std::invalid_argument);
	}
	TEST(MarketDataTest, RejectNegativeVolatility) {
		EXPECT_THROW(MarketData md(100, 0.01, 0.01, -0.23),
			std::invalid_argument);
	}


	TEST(MarketDataTest, RejectInfiniteSpot) {
		const double infinity = std::numeric_limits<double>::infinity();
		EXPECT_THROW(MarketData md(infinity, 0.01, 0.01, 0.23),
			std::invalid_argument);
	}
	TEST(MarketDataTest, RejectInfiniteRiskFreeRate) {
		const double infinity = std::numeric_limits<double>::infinity();
		EXPECT_THROW(MarketData md(100.0, infinity, 0.01, 0.23),
			std::invalid_argument);
	}
	TEST(MarketDataTest, RejectInfiniteDividendYield) {
		const double infinity = std::numeric_limits<double>::infinity();
		EXPECT_THROW(MarketData md(100.0, 0.01, infinity, 0.23),
			std::invalid_argument);
	}
	TEST(MarketDataTest, RejectInfiniteVolatility) {
		const double infinity = std::numeric_limits<double>::infinity();
		EXPECT_THROW(MarketData md(100.0, 0.01, 0.01, infinity),
			std::invalid_argument);
	}

	TEST(MarketDataTest, RejectNaNSpot) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_THROW(MarketData md(nan, 0.1, 0.1, 0.20),
			std::invalid_argument);
	}

	TEST(MarketDataTest, RejectNaNRiskFreeRate) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_THROW(MarketData md(100.0, nan, 0.1, 0.20),
			std::invalid_argument);
	}

	TEST(MarketDataTest, RejectNaNDividendYield) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_THROW(MarketData md(100.0, 0.1, nan, 0.20),
			std::invalid_argument);
	}
	TEST(MarketDataTest, RejectNaNVolatility) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_THROW(MarketData md(100.0, 0.1, 0.1, nan),
			std::invalid_argument);
	}

	
}