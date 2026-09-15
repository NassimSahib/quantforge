#include "pch.h"

#include "AsianOption.h"

namespace quantforge::instruments {
	class AsianOptionTest : public ::testing::Test {
	protected:
		std::vector<double> monitoringPrices = { 100.20, 108.90, 110.0 };
		AsianOption ao{ "AAPL_ASIA_001",
			1.0,
			100,
			OptionType::Call,
			3 };
	};

	TEST_F(AsianOptionTest, ConstructWithValidValues) {
		EXPECT_EQ(ao.id(), "AAPL_ASIA_001");
		EXPECT_DOUBLE_EQ(ao.maturity(), 1.0);
		EXPECT_DOUBLE_EQ(ao.strike(), 100.0);
		EXPECT_EQ(ao.optionType(), OptionType::Call);
		EXPECT_EQ(ao.monitoringObservations(), 3);
	}

	TEST_F(AsianOptionTest, CorrectPayoff) {
		EXPECT_NEAR(ao.payoff(monitoringPrices), 106.36 - ao.strike(), 0.01);
	}
<<<<<<< HEAD

	TEST_F(AsianOptionTest, ReturnsCorrectInstrumentType) {
		EXPECT_EQ(ao.instrumentType(), InstrumentType::AsianOption);
	}
=======
>>>>>>> 27459080dc3c10ca19bb1b392232e622b04fddc7
}