#include "pch.h"

#include "MonteCarloScalingReportWriter.h"

#include <iomanip>
#include <ostream>
#include <sstream>

namespace quantforge::reporting {

	void MonteCarloScalingReportWriter::write(
		const quantforge::benchmarking::
		MonteCarloScalingBenchmarkResult& result,
		std::ostream& output
	) const {

		std::ostringstream buffer;

		buffer
			<< std::fixed
			<< std::setprecision(2);


		buffer
			<< "========================================\n"
			<< "   QUANTFORGE MONTE-CARLO SCALING\n"
			<< "========================================\n\n";


		buffer
			<< "Hardware concurrency: "
			<< result.hardwareConcurrency
			<< "\n";

		buffer
			<< "Iterations: "
			<< result.iterations
			<< "\n";

		buffer
			<< "Paths per price: "
			<< result.pathsPerPrice
			<< "\n\n";


		buffer
			<< "Sequential baseline\n"
			<< "-------------------\n";

		buffer
			<< "Price: "
			<< result.sequentialPrice
			<< "\n";

		buffer
			<< "Average time: "
			<< result.sequential.averageNanoseconds
			<< " ns / price\n";

		buffer
			<< "Throughput: "
			<< result.sequentialPathsPerSecond
			<< " paths / second\n\n";


		buffer
			<< "Parallel scaling\n"
			<< "--------------------------------------------------------------\n";

		buffer
			<< std::left
			<< std::setw(10)
			<< "Workers"

			<< std::setw(18)
			<< "Time (ns)"

			<< std::setw(22)
			<< "Paths/sec"

			<< std::setw(12)
			<< "Speedup"

			<< "Price\n";


		buffer
			<< "--------------------------------------------------------------\n";


		for (const auto& entry :
			result.parallelResults) {

			buffer
				<< std::left
				<< std::setw(10)
				<< entry.workers

				<< std::setw(18)
				<< entry.averageNanoseconds

				<< std::setw(22)
				<< entry.pathsPerSecond

				<< std::setw(12)
				<< entry.speedupVsSequential

				<< entry.price
				<< '\n';
		}


		buffer
			<< "--------------------------------------------------------------\n";

		output << buffer.str();
	}

}