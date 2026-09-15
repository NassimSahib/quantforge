#include "pch.h"

#include "AsianMonteCarloPricer.h"

#include <stdexcept>
#include <cmath>
#include <random>

namespace quantforge::pricing {
	AsianMonteCarloPricer::AsianMonteCarloPricer(std::uint64_t seed,
		std::size_t paths) :
		seed_(seed),
		paths_(paths)
	{
		if (paths_ == 0) {
			throw std::invalid_argument("Paths count must be strictly positive");
		}
	}

	double AsianMonteCarloPricer::price(const quantforge::instruments::AsianOption& option,
		                                const quantforge::market::MarketData& data) const {
		double spot = data.spot();
		double riskFreeRate = data.riskFreeRate();
		double dividendYield = data.dividendYield();
		double volatility = data.volatility();
		double maturity = option.maturity();
		std::uint32_t monitoringObservations = option.monitoringObservations();

		const double dt = maturity / monitoringObservations;

		const double drift = (riskFreeRate - dividendYield
			                  - 0.5 * volatility * volatility) * dt;

		const double diffusionScale = volatility * std::sqrt(dt);

		const double discount = std::exp(-riskFreeRate * maturity);

		double payoffSum = 0.0;

		std::mt19937_64 generator(seed_);
		std::normal_distribution<double> normal(0.0, 1.0);

		for (std::size_t i = 0; i < paths_; i++) {
			double currentPrice = spot;

			std::vector<double> prices;
			prices.reserve(monitoringObservations);

			for (std::size_t j = 0; j < monitoringObservations; j++) {
				const double z = normal(generator);

				currentPrice *= std::exp(drift + diffusionScale * z);

				prices.push_back(currentPrice);
			}

			payoffSum += option.payoff(prices);	

		}

		const double averagePayoff = payoffSum / static_cast<double>(paths_);

		return averagePayoff * discount;

		
	}

}