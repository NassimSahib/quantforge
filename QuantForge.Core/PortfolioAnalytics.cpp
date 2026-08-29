#include "pch.h"

#include "PortfolioAnalytics.h"
#include "BlackScholesContext.h"
#include "BlackScholesGreeks.h"
#include "BlackScholesPricer.h"
#include "EuropeanOption.h"
#include <stdexcept>

namespace quantforge::portfolio {
	double PortfolioAnalytics::portfolioValue(
		const Portfolio& portfolio,
		const quantforge::market::MarketData& marketData)
		const {

		quantforge::pricing::BlackScholesPricer bsp;
		double total = 0.0;

		for (const Position& position : portfolio.positions()) {

			switch (position.instrument().instrumentType()) {

				case quantforge::instruments::InstrumentType::EuropeanOption:
				{
					const quantforge::instruments::EuropeanOption& euOption =
						static_cast<const quantforge::instruments::EuropeanOption&>(
							position.instrument());

					const double price = bsp.price(euOption, marketData);

					total += price * position.quantity();
					break;
				}
				default:
					throw std::logic_error("This instrument isn't supported yet");
			}
		}
		return total;

		}
	quantforge::pricing::Greeks PortfolioAnalytics::portfolioGreeks(
		const Portfolio& portfolio,
		const quantforge::market::MarketData& marketData)
		const {

		quantforge::pricing::Greeks totalGreeks{ 0.0, 0.0, 0.0, 0.0, 0.0 };
		quantforge::pricing::BlackScholesGreeks bsg;

		for (const Position& position : portfolio.positions()) {
			switch (position.instrument().instrumentType()) {

			case quantforge::instruments::InstrumentType::EuropeanOption:
			{
				const quantforge::instruments::EuropeanOption& euOption =
					static_cast<const quantforge::instruments::EuropeanOption&>(
						position.instrument());

				quantforge::pricing::BlackScholesContext context{
					euOption,
					marketData
				};

				const auto positionGreeks = bsg.calculate(context);
				totalGreeks.delta += positionGreeks.delta * position.quantity(); 
				totalGreeks.gamma += positionGreeks.gamma * position.quantity();
				totalGreeks.vega += positionGreeks.vega * position.quantity();
				totalGreeks.theta += positionGreeks.theta * position.quantity();
				totalGreeks.rho += positionGreeks.rho * position.quantity();

				break;
			}

			default:
				throw std::logic_error("This instrument isn't supported yet");

			}
			
		}
		return totalGreeks;

	}
}