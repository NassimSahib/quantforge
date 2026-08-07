#include "pch.h"
#include <stdexcept>
#include <utility>
#include "Position.h"

namespace quantforge::portfolio
{
	Position::Position(std::shared_ptr<const quantforge::instruments::Instrument> instrument,
		               std::int64_t quantity) :
		instrument_(std::move(instrument)),
		quantity_(quantity) {
		if (!instrument_)
			throw std::invalid_argument("Instrument pointer must not be null");
		if (quantity_ == 0)
			throw std::invalid_argument("Quantity must be non-zero");
		
	}
		const quantforge::instruments::Instrument& Position::instrument() const { return *instrument_; }
		std::int64_t Position::quantity() const { return quantity_; }
}