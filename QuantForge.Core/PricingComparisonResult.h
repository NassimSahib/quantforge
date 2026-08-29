#pragma once

#include <chrono>
#include <optional>

namespace quantforge::pricing {
	struct PricingComparisonResult {
		double blackScholesPrice{};
		double monteCarloPrice{};

		double absoluteError{};
		std::optional<double> relativeError{};

		std::chrono::nanoseconds blackScholesTime{};
		std::chrono::nanoseconds monteCarloTime{};
	};
}
