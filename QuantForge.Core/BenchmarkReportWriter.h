#pragma once

#include "PricingBenchmarkResult.h"

#include <iosfwd>

namespace quantforge::reporting {

	class BenchmarkReportWriter {
	public:
		void write(
			const quantforge::benchmarking::PricingBenchmarkResult& result,
			std::ostream& output
		) const;
	};

}
