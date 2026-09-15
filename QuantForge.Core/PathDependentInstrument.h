#pragma once

#include "Instrument.h"

#include <vector>

namespace quantforge::instruments {
	class PathDependentInstrument : public Instrument {
	protected:
		PathDependentInstrument(std::string id, double maturity) :
			Instrument(std::move(id), maturity) 
		{
		};
	public:
		virtual ~PathDependentInstrument() = default;
		virtual double payoff(const std::vector<double>& prices) const = 0;

		virtual std::uint32_t monitoringObservations() const = 0;
	};
}