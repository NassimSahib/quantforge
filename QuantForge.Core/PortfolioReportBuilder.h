#pragma once

#include "PortfolioReport.h"
#include "Portfolio.h"
#include "MarketData.h"

#include <cstddef>
#include <cstdint>

namespace quantforge::portfolio {

	class PortfolioReportBuilder {
	public:
		PortfolioReport build(
			const Portfolio& portfolio,
			const quantforge::market::MarketData& marketData,
			std::size_t paths,
			std::uint64_t seed
		) const;
	};

}