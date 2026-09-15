#include "pch.h"

#include "BlackScholesPricer.h"
#include "AsianMonteCarloPricer.h"
#include "AsianOption.h"
#include "OptionType.h"

#include <stdexcept>

namespace quantforge::pricing {
	class AsianMonteCarloPricerTest : public ::testing::Test {
	protected:
		quantforge::instruments::AsianOption singleObsAsianCall{
			"AAPL_ASIAN_001",
			1.0,
			100,
			quantforge::instruments::OptionType::Call,
			1};

		quantforge::instruments::EuropeanOption singleObsEuCall{
			"AAPL_EU_001",
			1.0,
			100,
			quantforge::instruments::OptionType::Call};

		quantforge::market::MarketData data{
			100.0,
			0.05,
			0.02,
			0.0};

		std::uint64_t seed = 42;
		std::size_t paths = 100000;
		
	};

	TEST_F(AsianMonteCarloPricerTest, PriceZeroVolCallComparedBsc) {
		AsianMonteCarloPricer amcp{ seed, paths };
		BlackScholesPricer bsp;
		EXPECT_NEAR(amcp.price(singleObsAsianCall, data), bsp.price(singleObsEuCall, data), 0.02);
	}
}