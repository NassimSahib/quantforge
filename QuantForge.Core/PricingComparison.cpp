#include "pch.h"

#include "PricingComparison.h"
#include "BlackScholesPricer.h"
#include "MonteCarloPricer.h"

#include <cmath>
#include <chrono>

namespace quantforge::pricing {
	PricingComparisonResult PricingComparison::compare(
		const quantforge::instruments::EuropeanOption& option,
		const quantforge::market::MarketData& marketData,
		std::size_t paths,
		std::uint64_t seed
	) const {
		
		BlackScholesPricer bsp;
		MonteCarloPricer mcp{ paths, seed };

		const std::chrono::steady_clock::time_point startBsc = std::chrono::steady_clock::now();
		const double blackScholesPrice = bsp.price(option, marketData);
		const std::chrono::steady_clock::time_point endBsc = std::chrono::steady_clock::now();

		const std::chrono::steady_clock::time_point startMc = std::chrono::steady_clock::now();
		const double monteCarloPrice = mcp.price(option, marketData);
		const std::chrono::steady_clock::time_point endMc = std::chrono::steady_clock::now();

		const double absoluteError = std::abs(monteCarloPrice - blackScholesPrice);
		std::optional<double> relativeError;
		if (blackScholesPrice != 0) {
			relativeError = absoluteError
				/ std::abs(blackScholesPrice);
		}
		else {
			relativeError = std::nullopt;
		}

		const std::chrono::nanoseconds durationBsc = std::chrono::duration_cast<std::chrono::nanoseconds>(endBsc - startBsc);
		const std::chrono::nanoseconds durationMc = std::chrono::duration_cast<std::chrono::nanoseconds>(endMc - startMc);

		return PricingComparisonResult{ 
			blackScholesPrice,
			monteCarloPrice,
			absoluteError,
			relativeError,
			durationBsc,
			durationMc };
	}
}