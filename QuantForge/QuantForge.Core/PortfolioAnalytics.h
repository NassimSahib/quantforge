#pragma once

#include "Portfolio.h"
#include "Greeks.h"
#include "MarketData.h"

namespace quantforge::portfolio {
	class PortfolioAnalytics {
	public:
		double portfolioValue(
			const Portfolio& portfolio,
			const quantforge::market::MarketData& marketData
		) const;

		quantforge::pricing::Greeks portfolioGreeks(
			const Portfolio& portfolio,
			const quantforge::market::MarketData& marketData
		) const;

	};
}