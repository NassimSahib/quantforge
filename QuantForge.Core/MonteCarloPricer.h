#pragma once

#include <cstddef>
#include <cstdint>

#include "EuropeanOption.h"
#include "MarketData.h"

namespace quantforge::pricing {
	class MonteCarloPricer {
	private:
		std::size_t paths_;
		std::uint64_t seed_;
	public:
		MonteCarloPricer(
			std::size_t paths,
			std::uint64_t seed);

		double price(
			const quantforge::instruments::EuropeanOption& option,
			const quantforge::market::MarketData& marketData) const;
	};
}