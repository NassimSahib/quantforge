#include "pch.h"

#include "PortfolioReportBuilder.h"
#include "PortfolioAnalytics.h"
#include "BlackScholesPricer.h"
#include "MonteCarloPricer.h"
#include "EuropeanOption.h"
#include "Position.h"
#include "Portfolio.h"
#include "MarketData.h"

#include <memory>

namespace quantforge::portfolio {

	class PortfolioReportBuilderTest : public ::testing::Test {
	protected:

		std::shared_ptr<const quantforge::instruments::EuropeanOption>
			callOption =
			std::make_shared<const quantforge::instruments::EuropeanOption>(
				"CALL_001",
				1.0,
				100.0,
				quantforge::instruments::OptionType::Call
			);

		std::shared_ptr<const quantforge::instruments::EuropeanOption>
			putOption =
			std::make_shared<const quantforge::instruments::EuropeanOption>(
				"PUT_001",
				1.0,
				100.0,
				quantforge::instruments::OptionType::Put
			);

		quantforge::market::MarketData marketData{
			120.0,
			0.05,
			0.02,
			0.20
		};

		Portfolio portfolio;

		void SetUp() override {

			portfolio.addPosition(
				Position{
					callOption,
					2
				}
			);

			portfolio.addPosition(
				Position{
					putOption,
					-3
				}
			);
		}
	};


	TEST_F(
		PortfolioReportBuilderTest,
		BuildsOneResultPerPosition
	) {
		PortfolioReportBuilder builder;

		const PortfolioReport report =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		ASSERT_EQ(
			report.positions.size(),
			2u
		);

		EXPECT_EQ(
			report.positions[0].instrumentId,
			"CALL_001"
		);

		EXPECT_EQ(
			report.positions[1].instrumentId,
			"PUT_001"
		);

		EXPECT_EQ(
			report.positions[0].quantity,
			2
		);

		EXPECT_EQ(
			report.positions[1].quantity,
			-3
		);
	}


	TEST_F(
		PortfolioReportBuilderTest,
		ComputesBlackScholesPositionValues
	) {
		PortfolioReportBuilder builder;
		quantforge::pricing::BlackScholesPricer bsp;

		const PortfolioReport report =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		const double callPrice =
			bsp.price(
				*callOption,
				marketData
			);

		const double putPrice =
			bsp.price(
				*putOption,
				marketData
			);

		EXPECT_DOUBLE_EQ(
			report.positions[0].blackScholesPositionValue,
			callPrice * 2.0
		);

		EXPECT_DOUBLE_EQ(
			report.positions[1].blackScholesPositionValue,
			putPrice * -3.0
		);
	}


	TEST_F(
		PortfolioReportBuilderTest,
		ComputesMonteCarloPositionValues
	) {
		constexpr std::size_t paths = 1000;
		constexpr std::uint64_t seed = 42;

		PortfolioReportBuilder builder;

		const PortfolioReport report =
			builder.build(
				portfolio,
				marketData,
				paths,
				seed
			);

		quantforge::pricing::MonteCarloPricer callPricer{
			paths,
			42
		};

		quantforge::pricing::MonteCarloPricer putPricer{
			paths,
			43
		};

		const double expectedCallPrice =
			callPricer.price(
				*callOption,
				marketData
			);

		const double expectedPutPrice =
			putPricer.price(
				*putOption,
				marketData
			);

		EXPECT_DOUBLE_EQ(
			report.positions[0].monteCarloPositionValue,
			expectedCallPrice * 2.0
		);

		EXPECT_DOUBLE_EQ(
			report.positions[1].monteCarloPositionValue,
			expectedPutPrice * -3.0
		);
	}


	TEST_F(
		PortfolioReportBuilderTest,
		AggregatesPortfolioValues
	) {
		PortfolioReportBuilder builder;

		const PortfolioReport report =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		const double expectedBlackScholesValue =
			report.positions[0].blackScholesPositionValue
			+ report.positions[1].blackScholesPositionValue;

		const double expectedMonteCarloValue =
			report.positions[0].monteCarloPositionValue
			+ report.positions[1].monteCarloPositionValue;

		EXPECT_DOUBLE_EQ(
			report.totalBlackScholesValue,
			expectedBlackScholesValue
		);

		EXPECT_DOUBLE_EQ(
			report.totalMonteCarloValue,
			expectedMonteCarloValue
		);
	}


	TEST_F(
		PortfolioReportBuilderTest,
		ReturnsPortfolioGreeks
	) {
		PortfolioReportBuilder builder;
		PortfolioAnalytics analytics;

		const PortfolioReport report =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		const quantforge::pricing::Greeks expected =
			analytics.portfolioGreeks(
				portfolio,
				marketData
			);

		EXPECT_DOUBLE_EQ(
			report.portfolioGreeks.delta,
			expected.delta
		);

		EXPECT_DOUBLE_EQ(
			report.portfolioGreeks.gamma,
			expected.gamma
		);

		EXPECT_DOUBLE_EQ(
			report.portfolioGreeks.vega,
			expected.vega
		);

		EXPECT_DOUBLE_EQ(
			report.portfolioGreeks.theta,
			expected.theta
		);

		EXPECT_DOUBLE_EQ(
			report.portfolioGreeks.rho,
			expected.rho
		);
	}


	TEST_F(
		PortfolioReportBuilderTest,
		AggregatesExecutionTimes
	) {
		PortfolioReportBuilder builder;

		const PortfolioReport report =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		std::chrono::nanoseconds expectedBlackScholesTime{};
		std::chrono::nanoseconds expectedMonteCarloTime{};

		for (const PositionPricingResult& position :
			report.positions) {

			expectedBlackScholesTime +=
				position.pricing.blackScholesTime;

			expectedMonteCarloTime +=
				position.pricing.monteCarloTime;
		}

		EXPECT_EQ(
			report.totalBlackScholesTime,
			expectedBlackScholesTime
		);

		EXPECT_EQ(
			report.totalMonteCarloTime,
			expectedMonteCarloTime
		);
	}


	TEST_F(
		PortfolioReportBuilderTest,
		SameSeedProducesReproduciblePricing
	) {
		PortfolioReportBuilder builder;

		const PortfolioReport report1 =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		const PortfolioReport report2 =
			builder.build(
				portfolio,
				marketData,
				1000,
				42
			);

		ASSERT_EQ(
			report1.positions.size(),
			report2.positions.size()
		);

		for (std::size_t i = 0;
			i < report1.positions.size();
			++i) {

			EXPECT_DOUBLE_EQ(
				report1.positions[i].pricing.monteCarloPrice,
				report2.positions[i].pricing.monteCarloPrice
			);
		}

		EXPECT_DOUBLE_EQ(
			report1.totalMonteCarloValue,
			report2.totalMonteCarloValue
		);
	}

}