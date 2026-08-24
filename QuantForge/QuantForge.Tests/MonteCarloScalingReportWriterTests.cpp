#include "pch.h"

#include "MonteCarloScalingReportWriter.h"

#include <chrono>
#include <sstream>
#include <string>

namespace quantforge::reporting {

	TEST(
		MonteCarloScalingReportWriterTest,
		WritesScalingResults
	) {

		quantforge::benchmarking::
			MonteCarloScalingBenchmarkResult result;

		result.iterations = 5;
		result.pathsPerPrice = 1000000;
		result.hardwareConcurrency = 8;

		result.sequentialPrice = 24.10;

		result.sequential =
			quantforge::benchmarking::BenchmarkStats{
				5,
				std::chrono::nanoseconds{
					100000000
				},
				20000000.0
		};

		result.sequentialPathsPerSecond =
			50000000.0;


		result.parallelResults.push_back(
			quantforge::benchmarking::
			MonteCarloScalingEntry{
				4,
				24.12,
				std::chrono::nanoseconds{
					30000000
				},
				6000000.0,
				166666666.0,
				3.33
			}
		);


		MonteCarloScalingReportWriter writer;

		std::ostringstream output;

		writer.write(
			result,
			output
		);


		const std::string text =
			output.str();


		EXPECT_NE(
			text.find(
				"QUANTFORGE MONTE-CARLO SCALING"
			),
			std::string::npos
		);

		EXPECT_NE(
			text.find(
				"Hardware concurrency: 8"
			),
			std::string::npos
		);

		EXPECT_NE(
			text.find("3.33"),
			std::string::npos
		);
	}

}