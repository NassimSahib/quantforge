#include "pch.h"
#include "AsianOption.h"

#include <utility>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <string>
#include <numeric>


namespace quantforge::instruments {
	AsianOption::AsianOption(std::string id,
		double maturity,
		double strike,
		OptionType optionType,
		std::uint32_t monitoringObservations) :
		PathDependentInstrument(std::move(id), maturity),
		strike_(strike),
		optionType_(optionType),
		monitoringObservations_(monitoringObservations)
	{
		if (strike_ <= 0 || !std::isfinite(strike_)) {
			throw std::invalid_argument("Strike must be strictly positive and finite");
		}

		if (monitoringObservations_ == 0) {
			throw std::invalid_argument("Monitoring observations must be at least 1");
		}
	}

	double AsianOption::strike() const { return strike_;  }
	OptionType AsianOption::optionType() const { return optionType_; }

	InstrumentType AsianOption::instrumentType() const { return InstrumentType::AsianOption; }

	double AsianOption::payoff(const std::vector<double>& prices) const {
		if (prices.size() != monitoringObservations_) {
			throw std::invalid_argument("Number of prices must be equal to monitoring observations");
		}

		double sum = std::accumulate(prices.begin(), prices.end(), 0.0);
		double arithmeticAverage = sum / static_cast<double>(prices.size());

		switch (optionType_) {

		case OptionType::Call:
			return std::max(arithmeticAverage - strike_, 0.0);
		case OptionType::Put:
			return std::max(strike_ - arithmeticAverage, 0.0);
		default:
			throw std::invalid_argument("Option Type must be Call or Put");
		}
	}

	std::uint32_t AsianOption::monitoringObservations() const { return monitoringObservations_; }


}

