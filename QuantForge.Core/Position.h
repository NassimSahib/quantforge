#pragma once
#include <cstdint>
#include <memory>
#include "Instrument.h"

namespace quantforge::portfolio
{
	class Position {
	private:
		std::shared_ptr<const quantforge::instruments::Instrument> instrument_;
		std::int64_t quantity_;
	public:
		Position(std::shared_ptr <const quantforge::instruments::Instrument> instrument,
			              std::int64_t quantity);

		const quantforge::instruments::Instrument& instrument() const;
		std::int64_t quantity() const;

	};
}
