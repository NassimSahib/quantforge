#include "pch.h"
#include "EuropeanOption.h"
#include <stdexcept>
#include <cmath>
#include <utility>
#include <algorithm>

namespace quantforge::instruments {
	EuropeanOption::EuropeanOption(std::string id,
		double maturity,
		double strike,
		OptionType optionType)
		: Instrument(std::move(id), maturity),
		  strike_(strike), 
		  optionType_(optionType)
	{
		if (strike_ <= 0 || !std::isfinite(strike_))
			throw std::invalid_argument("Strike must be strictly positive and finite");
	}

	double EuropeanOption::strike() const { return strike_; }
	OptionType EuropeanOption::optionType() const { return optionType_; }

	double EuropeanOption::payoff(double terminalSpot) const  {
		if (terminalSpot < 0 || !std::isfinite(terminalSpot))
			throw std::invalid_argument("Terminal Spot must be non-negative and finite");
		switch (optionType_) {

		case OptionType::Call:
			return std::max(0.0, terminalSpot - strike_);
		case OptionType::Put:
			return std::max(0.0, strike_ - terminalSpot);
		default:
			throw std::logic_error("Call/Put info must be present");
		}
	}
	InstrumentType EuropeanOption::instrumentType() const { return InstrumentType::EuropeanOption; }

}