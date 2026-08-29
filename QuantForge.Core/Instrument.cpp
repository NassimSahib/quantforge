#include "pch.h"
#include "Instrument.h"
#include <stdexcept>
#include <cmath>
#include <utility>

namespace quantforge::instruments
{
	Instrument::Instrument(std::string id, double maturity) : id_(std::move(id)), maturity_(maturity) {
		if (id_.empty())
			throw std::invalid_argument("Id must be a non-empty string");
		if (maturity_ <= 0 || !std::isfinite(maturity_))
			throw std::invalid_argument("Maturity must be strictly positive and finite");
	}

	const std::string& Instrument::id() const { return id_; }
	double Instrument::maturity() const { return maturity_; }
}