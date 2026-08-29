#include "pch.h"

#include "ConsoleReportWriter.h"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace quantforge::reporting {

	void ConsoleReportWriter::write(
		const quantforge::portfolio::PortfolioReport& report,
		std::ostream& output
	) const {

		std::ostringstream buffer;

		buffer << std::fixed << std::setprecision(4);

		buffer << "========================================\n";
		buffer << "          QUANTFORGE PORTFOLIO REPORT\n";
		buffer << "========================================\n\n";

		buffer << "Positions: " << report.positions.size() << "\n\n";

		for (const auto& position : report.positions) {

			buffer << "----------------------------------------\n";

			buffer << "Instrument: "
				<< position.instrumentId
				<< "\n";

			buffer << "Quantity: "
				<< position.quantity
				<< "\n";

			buffer << "Black-Scholes unit price: "
				<< position.pricing.blackScholesPrice
				<< "\n";

			buffer << "Monte-Carlo unit price: "
				<< position.pricing.monteCarloPrice
				<< "\n";

			buffer << "Absolute pricing error: "
				<< position.pricing.absoluteError
				<< "\n";

			buffer << "Relative pricing error: ";

			if (position.pricing.relativeError.has_value()) {
				buffer
					<< position.pricing.relativeError.value() * 100.0
					<< "%\n";
			}
			else {
				buffer << "N/A\n";
			}

			buffer << "Black-Scholes position value: "
				<< position.blackScholesPositionValue
				<< "\n";

			buffer << "Monte-Carlo position value: "
				<< position.monteCarloPositionValue
				<< "\n";

			buffer << "Black-Scholes time: "
				<< position.pricing.blackScholesTime.count()
				<< " ns\n";

			buffer << "Monte-Carlo time: "
				<< position.pricing.monteCarloTime.count()
				<< " ns\n";
		}

		buffer << "\n========================================\n";
		buffer << "PORTFOLIO SUMMARY\n";
		buffer << "========================================\n";

		buffer << "Total Black-Scholes value: "
			<< report.totalBlackScholesValue
			<< "\n";

		buffer << "Total Monte-Carlo value: "
			<< report.totalMonteCarloValue
			<< "\n";

		buffer << "\nGreeks\n";
		buffer << "Delta: "
			<< report.portfolioGreeks.delta
			<< "\n";

		buffer << "Gamma: "
			<< report.portfolioGreeks.gamma
			<< "\n";

		buffer << "Vega: "
			<< report.portfolioGreeks.vega
			<< "\n";

		buffer << "Theta: "
			<< report.portfolioGreeks.theta
			<< "\n";

		buffer << "Rho: "
			<< report.portfolioGreeks.rho
			<< "\n";

		buffer << "\nTiming\n";

		buffer << "Total Black-Scholes time: "
			<< report.totalBlackScholesTime.count()
			<< " ns\n";

		buffer << "Total Monte-Carlo time: "
			<< report.totalMonteCarloTime.count()
			<< " ns\n";

		buffer << "========================================\n";

		output << buffer.str();
	}

}