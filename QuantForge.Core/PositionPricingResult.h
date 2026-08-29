#pragma once

#include "PricingComparisonResult.h"

#include <cstdint>
#include <string>

namespace quantforge::portfolio {

	struct PositionPricingResult {
		std::string instrumentId{};
		std::int64_t quantity{};

		quantforge::pricing::PricingComparisonResult pricing{};

		double blackScholesPositionValue{};
		double monteCarloPositionValue{};
	};

}