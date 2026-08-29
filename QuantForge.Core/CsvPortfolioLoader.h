#pragma once

#include <iostream>
#include <filesystem>

#include "Portfolio.h"

namespace quantforge::io {
	class CsvPortfolioLoader {
	public:
		quantforge::portfolio::Portfolio load(const std::filesystem::path& path) const;
	};
}
