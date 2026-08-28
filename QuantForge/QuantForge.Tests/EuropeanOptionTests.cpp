#include "pch.h"
#include "EuropeanOption.h"
#include <limits>
#include <stdexcept>

namespace quantforge::instruments
{
	TEST(EuropeanOptionTest, ConstructsWithValidValues) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Call);
		EXPECT_EQ(eo.id(), "OPT_AAPL_001");
		EXPECT_DOUBLE_EQ(eo.maturity(), 1.5);
		EXPECT_DOUBLE_EQ(eo.strike(), 100.0);
		EXPECT_EQ(eo.optionType(), OptionType::Call);
		EXPECT_EQ(eo.instrumentType(), InstrumentType::EuropeanOption);
	}

	TEST(EuropeanOptionTest, CalculatesInTheMoneyCallPayoff) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Call);
		EXPECT_DOUBLE_EQ(eo.payoff(130.0), 30.0);
	}

	TEST(EuropeanOptionTest, CalculatesAtTheMoneyCallPayoff) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Call);
		EXPECT_DOUBLE_EQ(eo.payoff(100.0), 0.0);
	}

	TEST(EuropeanOptionTest, CalculatesOutOfTheMoneyCallPayoff) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Call);
		EXPECT_DOUBLE_EQ(eo.payoff(80.0), 0.0);
	}

	TEST(EuropeanOptionTest, CalculatesInTheMoneyPutPayoff) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Put);
		EXPECT_DOUBLE_EQ(eo.payoff(80.0), 20.0);
	}

	TEST(EuropeanOptionTest, CalculatesAtTheMoneyPutPayoff) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Put);
		EXPECT_DOUBLE_EQ(eo.payoff(100.0), 0.0);
	}

	TEST(EuropeanOptionTest, CalculatesOutOfTheMoneyPutPayoff) {
		EuropeanOption eo("OPT_AAPL_001", 1.5, 100.0, OptionType::Put);
		EXPECT_DOUBLE_EQ(eo.payoff(130.0), 0.0);
	}

	TEST(EuropeanOptionTest, AcceptsTerminalSpotEqualsZero) {
		EuropeanOption eoCall("OPT_AAPL_001", 1.5, 100.0, OptionType::Call);
		EuropeanOption eoPut("OPT_AAPL_001", 1.5, 100.0, OptionType::Put);
		EXPECT_DOUBLE_EQ(eoCall.payoff(0), 0.0);
		EXPECT_DOUBLE_EQ(eoPut.payoff(0), 100.0);
	}

	TEST(EuropeanOptionTest, RejectsEmptyId) {
		EXPECT_THROW(EuropeanOption eoCall("", 1.5, 100.0, OptionType::Call),
		             std::invalid_argument);
	}

	// Maturity Tests

	TEST(EuropeanOptionTest, RejectsZeroMaturity) {
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", 0.0, 100.0, OptionType::Call),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsNegativeMaturity) {
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", -1.0, 100.0, OptionType::Call),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsInfiniteMaturity) {
		const double infinity = std::numeric_limits<double>::infinity();
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", infinity, 100.0, OptionType::Call),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsNaNMaturity) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", nan, 100.0, OptionType::Call),
			std::invalid_argument);
	}

	// Strike Tests

	TEST(EuropeanOptionTest, RejectsZeroStrike) {
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", 1.0, 0.0, OptionType::Call),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsNegativeStrike) {
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", 1.0, -100.0, OptionType::Call),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsInfiniteStrike) {
		const double infinity = std::numeric_limits<double>::infinity();
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", 1.0, infinity, OptionType::Call),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsNaNStrike) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EXPECT_THROW(EuropeanOption eoCall("OPT_AAPL_001", 1.0, nan, OptionType::Call),
			std::invalid_argument);
	}

	// Terminal Spot Tests

	TEST(EuropeanOptionTest, RejectsNegativeTerminalSpot) {
		EuropeanOption eoCall("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		EXPECT_THROW(eoCall.payoff(-100.0),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsInfiniteTerminalSpot) {
		const double infinity = std::numeric_limits<double>::infinity();
		EuropeanOption eoCall("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		EXPECT_THROW(eoCall.payoff(infinity),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, RejectsNaNTerminalSpot) {
		const double nan = std::numeric_limits<double>::quiet_NaN();
		EuropeanOption eoCall("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		EXPECT_THROW(eoCall.payoff(nan),
			std::invalid_argument);
	}

	TEST(EuropeanOptionTest, SupportsPolymorphicPayoffThroughInstrument) {
		EuropeanOption eo("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		Instrument* base = &eo;
		EXPECT_DOUBLE_EQ(base->payoff(130.0), 30.0);
		EXPECT_EQ(base->instrumentType(), InstrumentType::EuropeanOption);
	}
}

