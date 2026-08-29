#pragma once

#include "PortfolioReport.h"

#include <iosfwd>

namespace quantforge::reporting {

	class ConsoleReportWriter {
	public:
		void write(
			const quantforge::portfolio::PortfolioReport& report,
			std::ostream& output
		) const;
	};

}