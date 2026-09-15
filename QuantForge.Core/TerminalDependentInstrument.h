#pragma once

#include "Instrument.h"

#include <vector>

namespace quantforge::instruments {
	class TerminalDependentInstrument : public Instrument{
	protected:
		TerminalDependentInstrument(std::string id, double maturity) :
			Instrument(std::move(id), maturity)
		{
		};
	public:
		virtual ~TerminalDependentInstrument() = default;
		virtual double payoff(double terminalSpot) const = 0;
	};
}