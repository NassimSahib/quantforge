#include "pch.h"

#include "BlackScholesPricer.h"
#include <cmath>

namespace quantforge::pricing
{
	TEST(BlackScholesPricerTest, PricesAtTheMoneyCall) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption option("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		MarketData marketData(100.0, 0.05, 0.0, 0.20);
		BlackScholesPricer bsp;
		EXPECT_NEAR(bsp.price(option, marketData), 10.4506, 0.0001);
	}
	TEST(BlackScholesPricerTest, PricesAtTheMoneyPut) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption option("OPT_AAPL_001", 1.0, 100.0, OptionType::Put);
		MarketData marketData(100.0, 0.05, 0.0, 0.20);
		BlackScholesPricer bsp;
		EXPECT_NEAR(bsp.price(option, marketData), 5.5735, 0.0001);
	}
	TEST(BlackScholesPricerTest, SatisfiesPutCallParity) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption call("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		EuropeanOption put("OPT_AAPL_001", 1.0, 100.0, OptionType::Put);
		MarketData marketData(100.0, 0.05, 0.02, 0.20);
		BlackScholesPricer bsp;
		const double callPrice = bsp.price(call, marketData);
		const double putPrice = bsp.price(put, marketData);
		const double discountDividend = std::exp(-marketData.dividendYield() * call.maturity());
		const double discountRate = std::exp(-marketData.riskFreeRate() * call.maturity());
		const double parityTerm = marketData.spot() * discountDividend - call.strike() * discountRate;

		EXPECT_NEAR(callPrice - putPrice, parityTerm, 0.0001);
	}

	TEST(BlackScholesPricerTest, PricesZeroVolatilityInTheMoneyCall) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption call("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		MarketData marketData(120.0, 0.0, 0.0, 0.0);
		BlackScholesPricer bsp;
		EXPECT_NEAR(bsp.price(call, marketData), 20.0,0.001);
	}

	TEST(BlackScholesPricerTest, PricesZeroVolatilityOutOfTheMoneyCall) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption call("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		MarketData marketData(90.0, 0.0, 0.0, 0.0);
		BlackScholesPricer bsp;
		EXPECT_NEAR(bsp.price(call, marketData), 0.0, 0.001);
	}
	
	TEST(BlackScholesPricerTest, PricesZeroVolatilityInTheMoneyPut) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption put("OPT_AAPL_001", 1.0, 100.0, OptionType::Put);
		MarketData marketData(90.0, 0.0, 0.0, 0.0);
		BlackScholesPricer bsp;
		EXPECT_NEAR(bsp.price(put, marketData), 10.0, 0.001);
	}
	TEST(BlackScholesPricerTest, PricesZeroVolatilityOutOfTheMoneyPut) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption put("OPT_AAPL_001", 1.0, 100.0, OptionType::Put);
		MarketData marketData(120.0, 0.0, 0.0, 0.0);
		BlackScholesPricer bsp;
		EXPECT_NEAR(bsp.price(put, marketData), 0.0, 0.001);
	}

	TEST(BlackScholesPricerTest, PricesZeroVolatilityWithDiscounting) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption call("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		MarketData marketData(120.0, 0.02, 0.05, 0.0);
		BlackScholesPricer bsp;
		const double estimatedPrice = std::max(marketData.spot() * std::exp(-marketData.dividendYield() * call.maturity())
			- call.strike() * std::exp(-marketData.riskFreeRate() * call.maturity()), 0.0);
		EXPECT_NEAR(bsp.price(call, marketData), estimatedPrice, 0.001);
	}

	TEST(BlackScholesPricerTest, VolatilitySpikeIncreaseInTheMoneyCallPrice) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption call("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		MarketData marketData1(120.0, 0.02, 0.05, 0.10);
		MarketData marketData2(120.0, 0.02, 0.05, 0.20);
		BlackScholesPricer bsp;
		EXPECT_TRUE(bsp.price(call, marketData2) > bsp.price(call, marketData1));
	}

	TEST(BlackScholesPricerTest, PricesNegativeRiskFreeRateInTheMoneyCall) {
		using namespace quantforge::instruments;
		using namespace quantforge::market;
		EuropeanOption call("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		MarketData marketData(120.0, -0.02, 0.05, 0.10);
		BlackScholesPricer bsp;
		const double expected = 12.83395226;
		EXPECT_NEAR(bsp.price(call, marketData), expected, 0.001);

		

	}
}
