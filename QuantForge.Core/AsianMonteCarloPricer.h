#pragma once

#include "AsianOption.h"
#include "MarketData.h"

#include <cstddef>
#include <cstdint>

namespace quantforge::pricing {
	class AsianMonteCarloPricer {
	private:
		std::uint64_t seed_;
		std::size_t paths_;
	public:
		AsianMonteCarloPricer(std::uint64_t seed,
			                  std::size_t paths);

		double price(const quantforge::instruments::AsianOption&  ao,
			         const quantforge::market::MarketData& md) const;
	};

}