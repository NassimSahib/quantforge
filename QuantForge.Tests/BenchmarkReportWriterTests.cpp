#include "pch.h"

#include "BenchmarkReportWriter.h"

#include <chrono>
#include <sstream>
#include <string>

namespace quantforge::reporting {

	TEST(
		BenchmarkReportWriterTest,
		WritesBenchmarkStatistics
	) {
		const quantforge::benchmarking::PricingBenchmarkResult result{
			24.06,
			24.10,

			quantforge::benchmarking::BenchmarkStats{
				1000000,
				std::chrono::nanoseconds{ 100000000 },
				100.0
			},

			quantforge::benchmarking::BenchmarkStats{
				10,
				std::chrono::nanoseconds{ 200000000 },
				20000000.0
			},

			100000,
			5000000.0,
			200000.0
		};


		BenchmarkReportWriter writer;
		std::ostringstream output;

		writer.write(
			result,
			output
		);

		const std::string text =
			output.str();

		EXPECT_NE(
			text.find("QUANTFORGE PRICING BENCHMARK"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Iterations: 1000000"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("Paths per price: 100000"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("5000000.00 paths / second"),
			std::string::npos
		);

		EXPECT_NE(
			text.find("200000.00x"),
			std::string::npos
		);
	}

}