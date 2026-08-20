
#include "CliApplication.h"

#include "CsvPortfolioLoader.h"
#include "MarketData.h"
#include "PortfolioReportBuilder.h"
#include "ConsoleReportWriter.h"

#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <ostream>
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

		std::size_t parsePaths(const std::string& text) {
			std::size_t pos = 0;

			const unsigned long long value =
				std::stoull(text, &pos);

			if (pos != text.size()) {
				throw std::invalid_argument(
					"Invalid path count: " + text
				);
			}

			return static_cast<std::size_t>(value);
		}

		std::uint64_t parseSeed(const std::string& text) {
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
				<< "Usage:\n"
				<< "QuantForge.CLI "
				<< "<portfolio.csv> "
				<< "<spot> "
				<< "<riskFreeRate> "
				<< "<dividendYield> "
				<< "<volatility> "
				<< "<paths> "
				<< "<seed>\n";
		}

	}


	int CliApplication::run(
		int argc,
		const char* const argv[],
		std::ostream& output,
		std::ostream& error
	) const {

		if (argc != 8) {
			printUsage(error);
			return 1;
		}

		try {

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
				parsePaths(argv[6]);

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