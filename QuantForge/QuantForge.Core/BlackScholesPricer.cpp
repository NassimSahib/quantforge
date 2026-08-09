#include "pch.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "BlackScholesPricer.h"

namespace
{
	double normalCdf(double x) {
		return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
	}
}

namespace quantforge::pricing
{
	double BlackScholesPricer::price(const quantforge::instruments::EuropeanOption& option,
		                             const quantforge::market::MarketData& marketData) const {
		const double spot = marketData.spot();
		const double maturity = option.maturity();
		const double strike = option.strike();
		const double riskFreeRate = marketData.riskFreeRate();
		const double dividendYield = marketData.dividendYield();
		const double vol = marketData.volatility();

		const double discountDividend = std::exp(-dividendYield * maturity);
		const double discountRate = std::exp(-riskFreeRate * maturity);

		if (vol == 0)
		{
			const double callPrice = std::max(spot * discountDividend - strike * discountRate,0.0);
			const double putPrice = std::max(strike * discountRate - spot * discountDividend,0.0);

			switch (option.optionType())
			{
			case quantforge::instruments::OptionType::Call:
				return callPrice;
			case quantforge::instruments::OptionType::Put:
				return putPrice;
			default:
				throw std::logic_error("Option Type must be Call or Put");
			}

		}

		const double logMoneyness = std::log(spot / strike);
		const double volSqrtT = vol * std::sqrt(maturity);
		const double numerator = logMoneyness + (riskFreeRate - dividendYield + 0.5 * vol * vol) * maturity;
		const double d1 = numerator / volSqrtT;
		const double d2 = d1 - volSqrtT;

		const double nd1 = normalCdf(d1);
		const double nd2 = normalCdf(d2);
		
		const double callPrice = spot * discountDividend * nd1 - strike * discountRate * nd2;
		const double negativeNd1 = normalCdf(-d1);
		const double negativeNd2 = normalCdf(-d2);
		const double putPrice = strike * discountRate * negativeNd2 - spot * discountDividend * negativeNd1;

		using quantforge::instruments::OptionType;

		switch (option.optionType()) {

		case OptionType::Call:
			return callPrice;

		case OptionType::Put:
			return putPrice;
		
		default:
			throw std::logic_error("Option Type must be Call or Put");
		}
	}
}