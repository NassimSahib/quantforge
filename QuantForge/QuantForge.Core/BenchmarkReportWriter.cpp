#include "pch.h"

#include "BenchmarkReportWriter.h"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace quantforge::reporting {

	void BenchmarkReportWriter::write(
		const quantforge::benchmarking::PricingBenchmarkResult& result,
		std::ostream& output
	) const {

		std::ostringstream buffer;

		buffer << std::fixed << std::setprecision(2);

		buffer << "========================================\n";
		buffer << "       QUANTFORGE PRICING BENCHMARK\n";
		buffer << "========================================\n\n";

		buffer << "Black-Scholes\n";
		buffer << "-------------\n";

		buffer << "Price: "
			<< result.blackScholesPrice
			<< "\n";

		buffer << "Iterations: "
			<< result.blackScholes.iterations
			<< "\n";

		buffer << "Total time: "
			<< result.blackScholes.totalTime.count()
			<< " ns\n";

		buffer << "Average time: "
			<< result.blackScholes.averageNanoseconds
			<< " ns / price\n\n";


		buffer << "Monte-Carlo\n";
		buffer << "-----------\n";

		buffer << "Price: "
			<< result.monteCarloPrice
			<< "\n";

		buffer << "Iterations: "
			<< result.monteCarlo.iterations
			<< "\n";

		buffer << "Paths per price: "
			<< result.monteCarloPaths
			<< "\n";

		buffer << "Total time: "
			<< result.monteCarlo.totalTime.count()
			<< " ns\n";

		buffer << "Average time: "
			<< result.monteCarlo.averageNanoseconds
			<< " ns / price\n";

		buffer << "Throughput: ";

		if (result.monteCarloPathsPerSecond.has_value()) {
			buffer
				<< result.monteCarloPathsPerSecond.value()
				<< " paths / second\n";
		}
		else {
			buffer << "N/A\n";
		}

		buffer << "\nPerformance\n";
		buffer << "-----------\n";

		buffer << "Monte-Carlo / Black-Scholes ratio: ";

		if (result.monteCarloToBlackScholesRatio.has_value()) {
			buffer
				<< result.monteCarloToBlackScholesRatio.value()
				<< "x\n";
		}
		else {
			buffer << "N/A\n";
		}

		buffer << "========================================\n";

		output << buffer.str();
	}

}