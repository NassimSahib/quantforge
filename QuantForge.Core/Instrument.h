#pragma once
#include <string>
#include "InstrumentType.h"

namespace quantforge::instruments {
	class Instrument {
	private:
		std::string id_;
		double maturity_;
	public:
		virtual ~Instrument() = default;
		virtual double payoff(double terminalSpot) const = 0;
		virtual InstrumentType instrumentType() const = 0;

		const std::string& id() const;
		double maturity() const;
	protected:
		Instrument(std::string id, double maturity);
	};
}

