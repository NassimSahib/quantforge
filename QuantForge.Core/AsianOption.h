#pragma once

#include <string>
#include <vector>

#include "Instrument.h"
#include "PathDependentInstrument.h"
#include "OptionType.h"
#include <cstdint>

namespace quantforge::instruments {

	class AsianOption final : public PathDependentInstrument {
	private:
		double strike_;
		OptionType optionType_;
		std::uint32_t monitoringObservations_;
	public:
		AsianOption(std::string id,
			double maturity,
			double strike,
			OptionType optionType,
			std::uint32_t monitoringObservations);

		double strike() const;
		OptionType optionType() const;

		double payoff(const std::vector<double>& prices) const override;
		InstrumentType instrumentType() const override;
		std::uint32_t monitoringObservations() const override;
	};
}
