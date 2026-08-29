#include "pch.h"

#include "MonteCarloPricer.h"

#include <stdexcept>
#include <random>
#include <cmath>

namespace quantforge::pricing {
	MonteCarloPricer::MonteCarloPricer(
		std::size_t paths,
		std::uint64_t seed
	) : 
		paths_(paths),
		seed_(seed) 
	{
		if (paths_ == 0) {
			throw std::invalid_argument("Path count must be strictly positive");
		}

	}

	double MonteCarloPricer::price(
		const quantforge::instruments::EuropeanOption& option,
		const quantforge::market::MarketData& marketData) const {

		const double spot = marketData.spot();
		const double riskFreeRate = marketData.riskFreeRate();
		const double dividendYield = marketData.dividendYield();
		const double volatility = marketData.volatility();
		const double maturity = option.maturity();

		const double drift =
			(riskFreeRate - dividendYield 
				- 0.5 * volatility * volatility) * maturity;

		const double diffusionScale =
			volatility * std::sqrt(maturity);

		const double discount =
			std::exp(-riskFreeRate * maturity);

		double payoffSum = 0.0;

		std::mt19937_64 generator(seed_);
		std::normal_distribution<double> normal(0.0,1.0);

		for (std::size_t i = 0; i < paths_; i++) {

			const double z = normal(generator);
			
			const double terminalSpot =
				spot * std::exp(drift + diffusionScale * z);

			payoffSum += option.payoff(terminalSpot);
		}

		const double averagePayoff =
			payoffSum / static_cast<double>(paths_);

		return discount * averagePayoff;

	}
}