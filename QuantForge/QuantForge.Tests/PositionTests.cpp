#include "pch.h"
#include "EuropeanOption.h"
#include "Position.h"
#include <stdexcept>
#include <memory>

namespace quantforge::portfolio {
	// Test Quantity
	TEST(PositionTest, AcceptsPositiveQuantity) {
		using namespace quantforge::instruments;
		std::shared_ptr<EuropeanOption> instrument = std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		Position pos(instrument, 30);
		EXPECT_EQ(pos.quantity(), 30);

	}

	TEST(PositionTest, AcceptsNegativeQuantity) {
		using namespace quantforge::instruments;
		std::shared_ptr<EuropeanOption> instrument = std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		Position pos(instrument, -40);
		EXPECT_EQ(pos.quantity(), -40);
	}

	TEST(PositionTest, RejectsZeroQuantity) {
		using namespace quantforge::instruments;
		std::shared_ptr<EuropeanOption> instrument = std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		EXPECT_THROW(Position pos(instrument, 0),
			         std::invalid_argument);
	}

	// Test Referenced Object

	TEST(PositionTest, ExposesReferencedInstrument) {
		using namespace quantforge::instruments;
		std::shared_ptr<EuropeanOption> instrument = std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		Position pos(instrument, 30);
		EXPECT_EQ(pos.instrument().id(), "OPT_AAPL_001");

	}

	TEST(PositionTest, RejectsNullInstrument) {
		using namespace quantforge::instruments;
		std::shared_ptr<EuropeanOption> instrument =nullptr;
		EXPECT_THROW(Position pos(instrument, 30),
			         std::invalid_argument);
	}

	TEST(PositionTest, AllowsMultiplePositionsToShareInstrument) {
		using namespace quantforge::instruments;
		std::shared_ptr<EuropeanOption> instrument = std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call);
		Position pos1(instrument,10);
		Position pos2(instrument,25);
		EXPECT_EQ(pos1.instrument().id(), pos2.instrument().id());
		EXPECT_EQ(pos1.instrument().instrumentType(), pos2.instrument().instrumentType());
		EXPECT_EQ(&pos1.instrument(), &pos2.instrument());
	}

	TEST(PositionTest, KeepsInstrumentAliveAfterOriginalSharedPointerIsDestroyed) {
		using namespace quantforge::instruments;
		Position pos1(std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call), 10);
		EXPECT_EQ(pos1.instrument().id(), "OPT_AAPL_001");
	}

	TEST(PositionTest, SupportsPolymorphicInstrumentBehavior) {
		using namespace quantforge::instruments;
		Position pos1(std::make_shared<EuropeanOption>("OPT_AAPL_001", 1.0, 100.0, OptionType::Call), 10);
		EXPECT_DOUBLE_EQ(pos1.instrument().payoff(130.0), 30.0);
		EXPECT_EQ(pos1.instrument().instrumentType(), InstrumentType::EuropeanOption);
	}
}

