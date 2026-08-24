#include "CliApplication.h"

#include "CsvPortfolioLoader.h"
#include "MarketData.h"
#include "PortfolioReportBuilder.h"
#include "ConsoleReportWriter.h"

#include "PricingBenchmark.h"
#include "BenchmarkReportWriter.h"
#include "EuropeanOption.h"
#include "OptionType.h"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <ostream>
#include <stdexcept>
#include <string>

namespace quantforge::cli {

	namespace {

		double parseDouble(const std::string& text) {

			std::size_t pos = 0;

			const double value =
				std::stod(text, &pos);

			if (pos != text.size()) {
				throw std::invalid_argument(
					"Invalid floating-point argument: " + text
				);
			}

			return value;
		}


		std::size_t parseCount(const std::string& text) {

			if (text.empty() || text.front() == '-') {
				throw std::invalid_argument(
					"Invalid positive count: " + text
				);
			}

			std::size_t pos = 0;

			const unsigned long long value =
				std::stoull(text, &pos);

			if (pos != text.size()) {
				throw std::invalid_argument(
					"Invalid positive count: " + text
				);
			}

			return static_cast<std::size_t>(value);
		}


		std::uint64_t parseSeed(const std::string& text) {

			if (text.empty() || text.front() == '-') {
				throw std::invalid_argument(
					"Invalid seed: " + text
				);
			}

			std::size_t pos = 0;

			const unsigned long long value =
				std::stoull(text, &pos);

			if (pos != text.size()) {
				throw std::invalid_argument(
					"Invalid seed: " + text
				);
			}

			return static_cast<std::uint64_t>(value);
		}


		void printUsage(std::ostream& output) {

			output
				<< "Usage:\n\n"

				<< "Portfolio pricing:\n"
				<< "QuantForge.CLI "
				<< "<portfolio.csv> "
				<< "<spot> "
				<< "<riskFreeRate> "
				<< "<dividendYield> "
				<< "<volatility> "
				<< "<paths> "
				<< "<seed>\n\n"

				<< "Benchmark:\n"
				<< "QuantForge.CLI benchmark "
				<< "<bsIterations> "
				<< "<mcIterations> "
				<< "<mcPaths> "
				<< "<seed>\n";
		}

	}


	int CliApplication::run(
		int argc,
		const char* const argv[],
		std::ostream& output,
		std::ostream& error
	) const {

		try {

			// ------------------------------------------------
			// BENCHMARK MODE
			// ------------------------------------------------

			if (
				argc >= 2
				&& std::string{ argv[1] } == "benchmark"
				) {

				if (argc != 6) {
					printUsage(error);
					return 1;
				}

				const std::size_t blackScholesIterations =
					parseCount(argv[2]);

				const std::size_t monteCarloIterations =
					parseCount(argv[3]);

				const std::size_t monteCarloPaths =
					parseCount(argv[4]);

				const std::uint64_t seed =
					parseSeed(argv[5]);


				// Fixed benchmark scenario.
				//
				// Keeping the financial inputs constant makes
				// performance comparisons between versions meaningful.

				const quantforge::instruments::EuropeanOption option{
					"BENCH_CALL",
					1.0,
					100.0,
					quantforge::instruments::OptionType::Call
				};

				const quantforge::market::MarketData marketData{
					120.0,
					0.05,
					0.02,
					0.20
				};


				quantforge::benchmarking::PricingBenchmark benchmark;

				const quantforge::benchmarking::PricingBenchmarkResult result =
					benchmark.run(
						option,
						marketData,
						blackScholesIterations,
						monteCarloIterations,
						monteCarloPaths,
						seed
					);


				output
					<< "Scenario: European Call\n"
					<< "S=120, K=100, T=1, r=0.05, q=0.02, sigma=0.20\n\n";


				quantforge::reporting::BenchmarkReportWriter writer;

				writer.write(
					result,
					output
				);

				return 0;
			}


			// ------------------------------------------------
			// PORTFOLIO MODE
			// ------------------------------------------------

			if (argc != 8) {
				printUsage(error);
				return 1;
			}


			const std::filesystem::path portfolioPath{
				argv[1]
			};

			const double spot =
				parseDouble(argv[2]);

			const double riskFreeRate =
				parseDouble(argv[3]);

			const double dividendYield =
				parseDouble(argv[4]);

			const double volatility =
				parseDouble(argv[5]);

			const std::size_t paths =
				parseCount(argv[6]);

			const std::uint64_t seed =
				parseSeed(argv[7]);


			const quantforge::market::MarketData marketData{
				spot,
				riskFreeRate,
				dividendYield,
				volatility
			};


			quantforge::io::CsvPortfolioLoader loader;

			const quantforge::portfolio::Portfolio portfolio =
				loader.load(portfolioPath);


			quantforge::portfolio::PortfolioReportBuilder builder;

			const quantforge::portfolio::PortfolioReport report =
				builder.build(
					portfolio,
					marketData,
					paths,
					seed
				);


			quantforge::reporting::ConsoleReportWriter writer;

			writer.write(
				report,
				output
			);

			return 0;
		}
		catch (const std::exception& exception) {

			error
				<< "QuantForge error: "
				<< exception.what()
				<< '\n';

			return 1;
		}
	}

}