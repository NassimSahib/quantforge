#include "pch.h"

#include <cmath>
#include <stdexcept>
#include <algorithm>
#include "BlackScholesPricer.h"


namespace quantforge::pricing
{
	double BlackScholesPricer::price(const quantforge::instruments::EuropeanOption& option,
		                             const quantforge::market::MarketData& marketData) const {

		const double vol = marketData.volatility();


		if (vol == 0.0)
		{
			const double spot = marketData.spot();
			const double maturity = option.maturity();
			const double strike = option.strike();
			const double riskFreeRate = marketData.riskFreeRate();
			const double dividendYield = marketData.dividendYield();


			const double discountDividend = std::exp(-dividendYield * maturity);
			const double discountRate = std::exp(-riskFreeRate * maturity);
			const double callPrice = std::max(spot * discountDividend - strike * discountRate, 0.0);
			const double putPrice = std::max(strike * discountRate - spot * discountDividend, 0.0);

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
		
		BlackScholesContext context(option, marketData);
		return price(context);
		
	}

	double BlackScholesPricer::price(const BlackScholesContext& context) const{
		const double callPrice = context.spot() * context.discountDividend() * context.nd1()
			                     - context.strike() * context.discountRate() * context.nd2();

		const double putPrice = context.strike() * context.discountRate() *( 1.0 - context.nd2())
			                     - context.spot() * context.discountDividend() * (1.0 - context.nd1());

		switch (context.optionType()) {

		case quantforge::instruments::OptionType::Call:
			return callPrice;

		case quantforge::instruments::OptionType::Put:
			return putPrice;

		default:
			throw std::logic_error("Option type must be Call or Put");
		}
		
	}
};