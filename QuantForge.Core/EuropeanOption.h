#pragma once
#include "Instrument.h"
#include "OptionType.h"

namespace quantforge::instruments {

	class EuropeanOption final: public Instrument {
	private:
		double strike_;
		OptionType optionType_;
	public:
		EuropeanOption(std::string id, double maturity, double strike, OptionType optionType);

		double strike() const;
		OptionType optionType() const;

		double payoff(double terminalSpot) const override;
		InstrumentType instrumentType() const override;
	};
}

