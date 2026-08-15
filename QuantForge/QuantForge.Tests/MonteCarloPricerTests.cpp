#include "pch.h"

#include "MonteCarloPricer.h"
#include "BlackScholesPricer.h"
#include <cmath>
#include <algorithm>

namespace quantforge::pricing {

	class MonteCarloPricerTest : public ::testing::Test {
	protected:
		quantforge::instruments::EuropeanOption option{
			"OPT_AAPL_001",
			1.0,
			100,
			quantforge::instruments::OptionType::Call
		};

		quantforge::instruments::EuropeanOption putOption{
			"OPT_AAPL_001",
			1.0,
			100,
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
	TEST_F(MonteCarloPricerTest, RejectsZeroPaths) {
		EXPECT_THROW(MonteCarloPricer pricer(0, 100),
			std::invalid_argument);
	}

	TEST_F(MonteCarloPricerTest, ZeroVolatilityMatchesDeterministicPrice) {
		MonteCarloPricer mcp{
			100,
			42
		};
		const double expected_price = std::max(
			zeroVolMarketData.spot() * std::exp(-zeroVolMarketData.dividendYield() * option.maturity())
			- option.strike() * std::exp(-zeroVolMarketData.riskFreeRate() * option.maturity()),
			0.0);
		EXPECT_NEAR(mcp.price(option, zeroVolMarketData), expected_price, 0.001);
	}

	TEST_F(MonteCarloPricerTest, SameSeedProducesSamePrice) {
		MonteCarloPricer mcp1{ 1000, 42 };
		MonteCarloPricer mcp2{ 1000, 42 };
		const double price1 = mcp1.price(option, basicMarketData);
		const double price2 = mcp2.price(option, basicMarketData);
		EXPECT_DOUBLE_EQ(price1, price2);
	}

	TEST_F(MonteCarloPricerTest, MonteCarloCallPriceIsCloseToBlackScholes) {
		BlackScholesPricer bsp;
		MonteCarloPricer mc{ 100000, 42 };
		EXPECT_NEAR(mc.price(option, basicMarketData),
			        bsp.price(option, basicMarketData),
			        0.2);
	}

	TEST_F(MonteCarloPricerTest, MonteCarloPutPriceIsCloseToBlackScholes) {
		BlackScholesPricer bsp;
		MonteCarloPricer mc{ 100000, 42 };
		EXPECT_NEAR(mc.price(putOption, basicMarketData),
			bsp.price(putOption, basicMarketData),
			0.2);
	}
}