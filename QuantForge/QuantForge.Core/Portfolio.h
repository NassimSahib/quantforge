#pragma once

#include <vector>
#include "Position.h"


namespace quantforge::portfolio {
	class Portfolio {
	private:
		std::vector<Position> positions_;
	public:
		void addPosition(const Position& position);
		std::size_t size() const;
		bool empty() const;
		const std::vector<Position>& positions() const;
	};
}
