#include "pch.h"

#include "CsvPortfolioLoader.h"
#include "EuropeanOption.h"
#include "OptionType.h"
#include <fstream>

namespace quantforge::io {
	TEST(CsvPortfolioLoaderTest, LoadsValidPortfolio) {
		const std::filesystem::path path = "portfolio_valid_test.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,EUROPEAN_OPTION,CALL,100.0,1.0,1\n";
		file << "OPT_AAPL_002,EUROPEAN_OPTION,PUT,90.0,0.5,-5\n";
		file.close();

		CsvPortfolioLoader loader;
		quantforge::portfolio::Portfolio portfolio = loader.load(path);

		EXPECT_EQ(portfolio.size(), 2);
		EXPECT_EQ(portfolio.positions()[0].instrument().id(), "OPT_AAPL_001");
		EXPECT_EQ(portfolio.positions()[1].instrument().id(), "OPT_AAPL_002");

		EXPECT_EQ(portfolio.positions()[0].quantity(), 1);
		EXPECT_EQ(portfolio.positions()[1].quantity(), -5);

		EXPECT_EQ(
			portfolio.positions()[0].instrument().instrumentType(),
			quantforge::instruments::InstrumentType::EuropeanOption);
		EXPECT_EQ(
			portfolio.positions()[1].instrument().instrumentType(),
			quantforge::instruments::InstrumentType::EuropeanOption);

		const quantforge::instruments::EuropeanOption& call =
			static_cast<const quantforge::instruments::EuropeanOption&>(
				portfolio.positions()[0].instrument()
				);
		const quantforge::instruments::EuropeanOption& put =
			static_cast<const quantforge::instruments::EuropeanOption&>(
				portfolio.positions()[1].instrument()
				);

		EXPECT_DOUBLE_EQ(call.strike(), 100.0);
		EXPECT_DOUBLE_EQ(put.strike(), 90.0);

		EXPECT_DOUBLE_EQ(call.maturity(), 1.0);
		EXPECT_DOUBLE_EQ(put.maturity(), 0.5);

		EXPECT_EQ(call.optionType(), quantforge::instruments::OptionType::Call);
		EXPECT_EQ(put.optionType(), quantforge::instruments::OptionType::Put);

		std::filesystem::remove(path);
	}

	TEST(CsvPortfolioLoaderTest, RejectsMissingFile) {
		const std::filesystem::path path = "portfolio_file_that_does_not_exist.csv";

		std::filesystem::remove(path);

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::runtime_error);
	}

	TEST(CsvPortfolioLoaderTest, RejectsEmptyFile) {
		const std::filesystem::path path = "portfolio_empty.csv";

		std::ofstream file(path);
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::runtime_error);

		std::filesystem::remove(path);
	}

	TEST(CsvPortfolioLoaderTest, RejectsInvalidHeader) {
		const std::filesystem::path path = "portfolio_invalid_header.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantitY\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);

		std::filesystem::remove(path);
	}

	TEST(CsvPortfolioLoaderTest, RejectsUnsupportedInstrument) {
		const std::filesystem::path path = "portfolio_unsupported_instrument.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,AMERICAN_OPTION,CALL,100.0,1.0,1\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);
	   
		std::filesystem::remove(path);
	}

	TEST(CsvPortfolioLoaderTest, RejectsUnsupportedOptionType) {
		const std::filesystem::path path = "portfolio_unsupported_option_type.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,EUROPEAN_OPTION,LONG,100.0,1.0,1\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);

		std::filesystem::remove(path);
	}

	TEST(CsvPortfolioLoaderTest, RejectsMalformedRow) {
		const std::filesystem::path path = "portfolio_malformed_row.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,EUROPEAN_OPTION,CALL,100.0,1.0\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);

		std::filesystem::remove(path);
	}

	TEST(CsvPortfolioLoaderTest, RejectsInvalidStrike) {
		const std::filesystem::path path = "portfolio_invalid_strike.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,EUROPEAN_OPTION,CALL,100.0abc,1.0,1\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);

		std::filesystem::remove(path);
	}
	TEST(CsvPortfolioLoaderTest, RejectsInvalidQuantity) {
		const std::filesystem::path path = "portfolio_invalid_quantity.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,EUROPEAN_OPTION,CALL,100.0,1.0,1.5\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);

		std::filesystem::remove(path);
	}
	TEST(CsvPortfolioLoaderTest, RejectsInvalidEndOfRow) {
		const std::filesystem::path path = "portfolio_invalid_end_of_row.csv";

		std::ofstream file(path);
		file << "id,type,option_type,strike,maturity,quantity\n";
		file << "OPT_AAPL_001,EUROPEAN_OPTION,CALL,100.0,1.0,1.5,BUG\n";
		file.close();

		CsvPortfolioLoader loader;
		EXPECT_THROW(loader.load(path),
			std::invalid_argument);

		std::filesystem::remove(path);
	}
}
