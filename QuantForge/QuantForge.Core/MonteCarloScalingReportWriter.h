#pragma once

#include "MonteCarloScalingBenchmarkResult.h"

#include <iosfwd>

namespace quantforge::reporting {

	class MonteCarloScalingReportWriter {
	public:

		void write(
			const quantforge::benchmarking::
			MonteCarloScalingBenchmarkResult& result,
			std::ostream& output
		) const;
	};

}