#pragma once
#include "MarketData.h"
#include "EuropeanOption.h"
#include "BlackScholesContext.h"

namespace quantforge::pricing
{
	class BlackScholesPricer {
	public:
		double price(const quantforge::instruments::EuropeanOption& option,
			         const quantforge::market::MarketData& marketData) 
			const;

		double price(const BlackScholesContext& context) const;
	};
}