#include "pch.h"

#include "PortfolioAnalytics.h"
#include "BlackScholesContext.h"
#include "BlackScholesPricer.h"
#include "BlackScholesGreeks.h"
#include "EuropeanOption.h"

namespace quantforge::portfolio {
	class PortfolioAnalyticsTests : public ::testing::Test {
	protected:
		std::shared_ptr<const quantforge::instruments::EuropeanOption> callOpt =
			std::make_shared<quantforge::instruments::EuropeanOption>(
				"OPT_AAPL_001",
				1.0,
				100.0,
				quantforge::instruments::OptionType::Call
			);
		std::shared_ptr<const quantforge::instruments::EuropeanOption> putOpt =
			std::make_shared<quantforge::instruments::EuropeanOption>(
				"OPT_AAPL_001",
				1.0,
				100.0,
				quantforge::instruments::OptionType::Put
			);
		Position callPos{ callOpt, 10 };
		Position putPos{ putOpt, -20 };

		quantforge::market::MarketData marketData{
			100.0,
			0.05,
			0.02,
			0.20 };

		quantforge::pricing::BlackScholesContext contextCall{ *callOpt,
			                                              marketData };
		quantforge::pricing::BlackScholesContext contextPut{ *putOpt,
														  marketData };
	};
	TEST_F(PortfolioAnalyticsTests, EmptyPortfolioHasZeroValue) {
		Portfolio portfolio;
		PortfolioAnalytics portfAnalytics;
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioValue(portfolio, marketData),
			             0.0);
	}
	TEST_F(PortfolioAnalyticsTests, ValuesSingleLongPosition) {
		Portfolio portfolio;
		portfolio.addPosition(callPos);
		PortfolioAnalytics portfAnalytics;
		quantforge::pricing::BlackScholesPricer bsp;
		
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioValue(portfolio, marketData),
			             callPos.quantity() * bsp.price(contextCall)) ;
	}

	TEST_F(PortfolioAnalyticsTests, ValuesLongAndShortPositions) {
		Portfolio portfolio;
		portfolio.addPosition(callPos);
		portfolio.addPosition(putPos);
		PortfolioAnalytics portfAnalytics;
		quantforge::pricing::BlackScholesPricer bsp;

		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioValue(portfolio, marketData),
			callPos.quantity() * bsp.price(contextCall)
		    + putPos.quantity() * bsp.price(contextPut));
	}

	TEST_F(PortfolioAnalyticsTests, EmptyPortfolioHasZeroValueGreeks) {
		Portfolio portfolio;
		PortfolioAnalytics portfAnalytics;
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).delta,
			0.0);
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).gamma,
			0.0);
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).vega,
			0.0);
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).theta,
			0.0);
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).rho,
			0.0);
	}

	TEST_F(PortfolioAnalyticsTests, ValuesSingleLongPositionGreeks) {
		Portfolio portfolio;
		portfolio.addPosition(callPos);
		PortfolioAnalytics portfAnalytics;
		quantforge::pricing::BlackScholesGreeks bsg;

		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).delta,
			bsg.calculate(contextCall).delta * callPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).gamma,
			bsg.calculate(contextCall).gamma * callPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).vega,
			bsg.calculate(contextCall).vega * callPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).theta,
			bsg.calculate(contextCall).theta * callPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).rho,
			bsg.calculate(contextCall).rho * callPos.quantity());
	}

	TEST_F(PortfolioAnalyticsTests, ValuesLongAndShortPositionsGreeks) {
		Portfolio portfolio;
		portfolio.addPosition(callPos);
		portfolio.addPosition(putPos);
		PortfolioAnalytics portfAnalytics;
		quantforge::pricing::BlackScholesGreeks bsg;

		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).delta,
			bsg.calculate(contextCall).delta * callPos.quantity()
		    + bsg.calculate(contextPut).delta * putPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).gamma,
			bsg.calculate(contextCall).gamma * callPos.quantity()
			+ bsg.calculate(contextPut).gamma * putPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).vega,
			bsg.calculate(contextCall).vega * callPos.quantity()
		    + bsg.calculate(contextPut).vega * putPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).theta,
			bsg.calculate(contextCall).theta * callPos.quantity()
		    + bsg.calculate(contextPut).theta * putPos.quantity());
		EXPECT_DOUBLE_EQ(portfAnalytics.portfolioGreeks(portfolio, marketData).rho,
			bsg.calculate(contextCall).rho * callPos.quantity()
		    + bsg.calculate(contextPut).rho * putPos.quantity());
	}


}