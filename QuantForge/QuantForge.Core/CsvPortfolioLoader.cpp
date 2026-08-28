#include "pch.h"

#include "CsvPortfolioLoader.h"
#include "EuropeanOption.h"
#include "Position.h"
#include "OptionType.h"
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <string>

namespace quantforge::io {
	quantforge::portfolio::Portfolio CsvPortfolioLoader::load(
		const std::filesystem::path& path) const {

		quantforge::portfolio::Portfolio portfolio;
		std::ifstream file(path);

		if (!file.is_open()) {
			throw std::runtime_error("Cannot open portfolio CSV file");
		}

		std::string header;
		if (!std::getline(file,header)) {
			throw std::runtime_error("Portfolio CSV file is empty !");
		}

		const std::string expectedHeader = "id,type,option_type,strike,maturity,quantity";

		if (header != expectedHeader) {
			throw std::invalid_argument("Invalid CSV portfolio header");
		}

		std::string line;

		while (std::getline(file, line)) {
			std::stringstream ss(line);

			std::string id;
			std::string instrumentTypeText;
			std::string optionTypeText;
			std::string strikeText;
			std::string maturityText;
			std::string quantityText;

			if ((!std::getline(ss, id, ',')                 || id.empty())  ||
				(!std::getline(ss, instrumentTypeText, ',') || instrumentTypeText.empty()) ||
				(!std::getline(ss, optionTypeText, ',')     || optionTypeText.empty()) ||
				(!std::getline(ss, strikeText, ',')         || strikeText.empty()) ||
				(!std::getline(ss, maturityText, ',')       || maturityText.empty()) ||
				(!std::getline(ss, quantityText, ',')       || quantityText.empty()))
			{
				throw std::invalid_argument("A field is invalid");
			}

			std::size_t pos = 0;

			const double strike = std::stod(strikeText,&pos);
			if (pos != strikeText.size()) {
				throw std::invalid_argument("Invalid strike");
			}
			const double maturity = std::stod(maturityText,&pos);
			if (pos != maturityText.size()) {
				throw std::invalid_argument("Invalid maturity");
			}
			const std::int64_t quantity = std::stoll(quantityText,&pos);
			if (pos != quantityText.size()) {
				throw std::invalid_argument("Invalid quantity");
			}

			if (!ss.eof()) {
				throw std::invalid_argument("Too many fields in CSV row");
			}

			if (instrumentTypeText != "EUROPEAN_OPTION") {
				throw std::invalid_argument("This instrument isn't supported yet");
			}

			quantforge::instruments::OptionType optionType;
			if (optionTypeText == "CALL") {
				optionType = quantforge::instruments::OptionType::Call;
			}
			else if (optionTypeText == "PUT") {
				optionType = quantforge::instruments::OptionType::Put;
			}
			else {
				throw std::invalid_argument("This Option type isn't supported yet");
			}

			std::shared_ptr<const quantforge::instruments::EuropeanOption> euOption =
				std::make_shared<quantforge::instruments::EuropeanOption>(
					id,
					maturity,
					strike,
					optionType
				);
			quantforge::portfolio::Position position{
				euOption,
				quantity
			};

			portfolio.addPosition(position);
		}

		return portfolio;

	}
}