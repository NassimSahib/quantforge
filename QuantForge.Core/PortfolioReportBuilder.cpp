#include "pch.h"

#include "PortfolioReportBuilder.h"

#include "PricingComparison.h"
#include "PortfolioAnalytics.h"
#include "EuropeanOption.h"
#include "Instrument.h"

#include <stdexcept>

namespace quantforge::portfolio {

	PortfolioReport PortfolioReportBuilder::build(
		const Portfolio& portfolio,
		const quantforge::market::MarketData& marketData,
		std::size_t paths,
		std::uint64_t seed
	) const {

		PortfolioReport report;

		report.positions.reserve(portfolio.size());

		quantforge::pricing::PricingComparison pricingComparison;

		std::size_t positionIndex = 0;

		for (const Position& position : portfolio.positions()) {

			const quantforge::instruments::Instrument& instrument =
				position.instrument();

			switch (instrument.instrumentType()) {

			case quantforge::instruments::InstrumentType::EuropeanOption:
			{
				const auto& option =
					static_cast<const quantforge::instruments::EuropeanOption&>(
						instrument
						);

				const std::uint64_t positionSeed =
					seed + static_cast<std::uint64_t>(positionIndex);

				const quantforge::pricing::PricingComparisonResult pricing =
					pricingComparison.compare(
						option,
						marketData,
						paths,
						positionSeed
					);

				const double quantity =
					static_cast<double>(position.quantity());

				const double blackScholesPositionValue =
					pricing.blackScholesPrice * quantity;

				const double monteCarloPositionValue =
					pricing.monteCarloPrice * quantity;

				report.positions.push_back(
					PositionPricingResult{
						instrument.id(),
						position.quantity(),
						pricing,
						blackScholesPositionValue,
						monteCarloPositionValue
					}
				);

				report.totalBlackScholesValue +=
					blackScholesPositionValue;

				report.totalMonteCarloValue +=
					monteCarloPositionValue;

				report.totalBlackScholesTime +=
					pricing.blackScholesTime;

				report.totalMonteCarloTime +=
					pricing.monteCarloTime;

				break;
			}

			default:
				throw std::logic_error(
					"Unsupported instrument type in portfolio report"
				);
			}

			++positionIndex;
		}

		PortfolioAnalytics analytics;

		report.portfolioGreeks =
			analytics.portfolioGreeks(
				portfolio,
				marketData
			);

		return report;
	}

}