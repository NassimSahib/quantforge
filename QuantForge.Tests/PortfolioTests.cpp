#include "pch.h"

#include "Portfolio.h"
#include "OptionType.h"
#include "EuropeanOption.h"

namespace quantforge::portfolio {
	class PortfolioTests : public ::testing::Test {
	protected:
		std::shared_ptr<const quantforge::instruments::EuropeanOption> callOption = 
			std::make_shared< quantforge::instruments::EuropeanOption>(
				"OPT_AAPL_001", 
				1.0, 
				100.0, 
				quantforge::instruments::OptionType::Call);
		std::shared_ptr<const quantforge::instruments::EuropeanOption> putOption=
			std::make_shared<quantforge::instruments::EuropeanOption>(
				"OPT_AAPL_002",
				1.0,
				100.0,
				quantforge::instruments::OptionType::Put);
			
		Position pos1{ callOption, 5 };
		Position pos2{ putOption, 20};
	};

	TEST_F(PortfolioTests, NewPortfolioIsEmpty) {
		Portfolio portfolio;
		EXPECT_TRUE(portfolio.empty());
	}
	TEST_F(PortfolioTests, AddPositionIncreaseSize) {
		Portfolio portfolio;
		portfolio.addPosition(pos1);
		EXPECT_EQ(portfolio.size(), 1);
	}
	TEST_F(PortfolioTests, StoresAddedPositions) {
		Portfolio portfolio;
		portfolio.addPosition(pos1);
		portfolio.addPosition(pos2);
		EXPECT_EQ(portfolio.size(), 2);

		EXPECT_EQ(portfolio.positions()[0].quantity(), 5);
		EXPECT_EQ(portfolio.positions()[1].quantity(), 20);

		EXPECT_EQ(&portfolio.positions()[0].instrument(), callOption.get());
		EXPECT_EQ(&portfolio.positions()[1].instrument(), putOption.get());
	}
}