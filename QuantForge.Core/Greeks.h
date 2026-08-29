#pragma once

namespace quantforge::pricing {
	struct Greeks {
		double delta;
		double gamma;
		double vega;
		double theta;
		double rho;
	};
}
