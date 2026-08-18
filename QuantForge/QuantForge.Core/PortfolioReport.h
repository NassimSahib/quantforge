#pragma once

#include "PositionPricingResult.h"
#include "Greeks.h"

#include <chrono>
#include <vector>

namespace quantforge::portfolio {

	struct PortfolioReport {
		std::vector<PositionPricingResult> positions{};

		double totalBlackScholesValue{};
		double totalMonteCarloValue{};

		quantforge::pricing::Greeks portfolioGreeks{};

		std::chrono::nanoseconds totalBlackScholesTime{};
		std::chrono::nanoseconds totalMonteCarloTime{};
	};

}