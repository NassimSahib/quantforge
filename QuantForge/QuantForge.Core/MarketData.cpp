#include "pch.h"
#include "MarketData.h"
#include <stdexcept>
#include <cmath>

namespace quantforge::market
{
	MarketData::MarketData(double spot,
		double riskFreeRate,
		double dividendYield,
		double volatility)
		: spot_(spot),
		riskFreeRate_(riskFreeRate),
		dividendYield_(dividendYield),
		volatility_(volatility)
	{
		// Invariants
		if (!std::isfinite(spot_) || spot_ <= 0)
			throw std::invalid_argument("Spot must be finite and strictly positive");
		if (!std::isfinite(riskFreeRate_))
			throw std::invalid_argument("Risk free rate must be finite");
		if (!std::isfinite(dividendYield_))
			throw std::invalid_argument("Dividend Yield must be finite");
		if (!std::isfinite(volatility_) || volatility_ < 0)
			throw std::invalid_argument("Volatility must be finite and non-negative");
	}

	double MarketData::spot() const { return spot_;  }
	double MarketData::riskFreeRate() const { return riskFreeRate_; }
	double MarketData::dividendYield() const { return dividendYield_; }
	double MarketData::volatility() const { return volatility_; }

		  
}