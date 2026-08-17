#pragma once

#include "PricingComparisonResult.h"
#include "EuropeanOption.h"
#include "MarketData.h"

#include <cstddef>
#include <cstdint>

namespace quantforge::pricing {

	class PricingComparison {
	public:
		PricingComparisonResult compare(
			const quantforge::instruments::EuropeanOption& option,
			const quantforge::market::MarketData& marketData,
			std::size_t paths,
			std::uint64_t seed
		) const;
	};
}
