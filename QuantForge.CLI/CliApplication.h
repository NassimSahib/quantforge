#pragma once

#include <iosfwd>

namespace quantforge::cli {

	class CliApplication {
	public:
		int run(
			int argc,
			const char* const argv[],
			std::ostream& output,
			std::ostream& error
		) const;
	};

}