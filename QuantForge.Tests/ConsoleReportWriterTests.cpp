#include "pch.h"

#include "ConsoleReportWriter.h"

#include <chrono>
#include <sstream>
#include <string>

namespace quantforge::reporting {

	class ConsoleReportWriterTest : public ::testing::Test {
	protected:

		quantforge::portfolio::PortfolioReport createReport() {

			quantforge::portfolio::PortfolioReport report;

			quantforge::pricing::PricingComparisonResult pricing{
				24.1234,
				24.2234,
				0.1000,
				0.004145,
				std::chrono::nanoseconds{ 150 },
				std::chrono::nanoseconds{ 50000 }
			};

			report.positions.push_back(
				quantforge::portfolio::PositionPricingResult{
					"CALL_001",
					2,
					pricing,
					48.2468,
					48.4468
				}
			);

			report.totalBlackScholesValue = 48.2468;
			report.totalMonteCarloValue = 48.4468;

			report.portfolioGreeks.delta = 1.25;
			report.portfolioGreeks.gamma = 0.02;
			report.portfolioGreeks.vega = 35.0;
			report.portfolioGreeks.theta = -4.5;
			report.portfolioGreeks.rho = 20.0;

			report.totalBlackScholesTime =
				std::chrono::nanoseconds{ 150 };

			report.totalMonteCarloTime =
				std::chrono::nanoseconds{ 50000 };

			return report;
		}
	};


	TEST_F(
		ConsoleReportWriterTest,
		WritesReportHeader
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const auto report = createReport();

		writer.write(report, output);

		EXPECT_NE(
			output.str().find("QUANTFORGE PORTFOLIO REPORT"),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		WritesPositionInformation
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const auto report = createReport();

		writer.write(report, output);

		const std::string text = output.str();

		EXPECT_NE(
			text.find("CALL_001"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Quantity: 2"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("24.1234"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("24.2234"),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		WritesPortfolioValues
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const auto report = createReport();

		writer.write(report, output);

		const std::string text = output.str();

		EXPECT_NE(
			text.find("Total Black-Scholes value: 48.2468"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Total Monte-Carlo value: 48.4468"),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		WritesPortfolioGreeks
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const auto report = createReport();

		writer.write(report, output);

		const std::string text = output.str();

		EXPECT_NE(
			text.find("Delta: 1.2500"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Gamma: 0.0200"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Vega: 35.0000"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Theta: -4.5000"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Rho: 20.0000"),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		WritesRelativeErrorAsPercentage
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const auto report = createReport();

		writer.write(report, output);

		EXPECT_NE(
			output.str().find(
				"Relative pricing error: 0.4145%"
			),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		WritesNAWhenRelativeErrorIsUnavailable
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		auto report = createReport();

		report.positions[0].pricing.relativeError =
			std::nullopt;

		writer.write(report, output);

		EXPECT_NE(
			output.str().find(
				"Relative pricing error: N/A"
			),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		WritesExecutionTimes
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const auto report = createReport();

		writer.write(report, output);

		const std::string text = output.str();

		EXPECT_NE(
			text.find("150 ns"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("50000 ns"),
			std::string::npos
		);
	}


	TEST_F(
		ConsoleReportWriterTest,
		HandlesEmptyPortfolioReport
	) {
		ConsoleReportWriter writer;
		std::ostringstream output;

		const quantforge::portfolio::PortfolioReport report;

		writer.write(report, output);

		const std::string text = output.str();

		EXPECT_NE(
			text.find("Positions: 0"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("PORTFOLIO SUMMARY"),
			std::string::npos
		);
	}

}