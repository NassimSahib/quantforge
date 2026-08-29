#include "pch.h"

#include "Portfolio.h"

namespace quantforge::portfolio {
	void Portfolio::addPosition(const Position& position) {
		positions_.push_back(position);
	}

	std::size_t Portfolio::size() const { return positions_.size(); }
	bool Portfolio::empty() const { return positions_.empty(); }
	const std::vector<Position>& Portfolio::positions() const { return positions_; }
}