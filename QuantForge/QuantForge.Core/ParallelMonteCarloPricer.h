#pragma once

#include "EuropeanOption.h"
#include "MarketData.h"

#include <cstddef>
#include <cstdint>

namespace quantforge::pricing {

	class ParallelMonteCarloPricer {
	private:
		std::size_t paths_;
		std::uint64_t seed_;
		std::size_t workers_;

	public:
		ParallelMonteCarloPricer(
			std::size_t paths,
			std::uint64_t seed,
			std::size_t workers
		);

		double price(
			const quantforge::instruments::EuropeanOption& option,
			const quantforge::market::MarketData& marketData
		) const;

		std::size_t workerCount() const;
	};

}