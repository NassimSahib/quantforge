#include "pch.h"

#include "BlackScholesGreeks.h"
#include <stdexcept>

namespace quantforge::pricing {
	Greeks BlackScholesGreeks::calculate(const BlackScholesContext& context) const {
		const double gamma = (context.discountDividend() * context.normalPdfD1())
			                 / (context.spot() * context.volatility() * context.sqrtT());

		const double vega = context.spot() * context.discountDividend() * context.normalPdfD1() * context.sqrtT();

		const double commonThetaTerm = -((context.spot() * context.discountDividend() * context.normalPdfD1() * context.volatility())
			                             / 2.0 * context.sqrtT());

		switch (context.optionType()) {

		case quantforge::instruments::OptionType::Call:
		{
			const double callDelta = context.discountDividend() * context.nd1();
			const double callTheta = commonThetaTerm
				- context.riskFreeRate() * context.strike() * context.discountRate() * context.nd2()
				+ context.dividendYield() * context.spot() * context.discountDividend() * context.nd1();
			const double callRho = context.strike() * context.discountRate() * context.nd2();
			return Greeks{callDelta, gamma, vega, callTheta, callRho};
		}
			
		case quantforge::instruments::OptionType::Put:
		{
			const double putDelta = context.discountDividend() * (context.nd1() - 1.0);
			const double putTheta = commonThetaTerm
				+ context.riskFreeRate() * context.strike() * context.discountRate() * ( 1.0 - context.nd2())
				- context.dividendYield() * context.spot() * context.discountDividend() * (1.0 - context.nd1());
			const double putRho = -context.strike() * context.discountRate() * (1.0 - context.nd2());
			return Greeks{putDelta, gamma, vega, putTheta, putRho};
		}
			
		default:
			throw std::logic_error("Option type must be Call or Put");

		}
	}
}