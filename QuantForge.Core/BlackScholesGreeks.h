#pragma once
#include "Greeks.h"
#include "BlackScholesContext.h"


namespace quantforge::pricing {

	class BlackScholesGreeks {
	public:
		Greeks calculate(const BlackScholesContext& context) const;
	};
}

