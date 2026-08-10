#pragma once
#include "OptionType.h"
#include "EuropeanOption.h"
#include "MarketData.h"

namespace quantforge::pricing {
	class BlackScholesContext {
	private:
		quantforge::instruments::OptionType optionType_;
		double spot_;
		double strike_;
		double maturity_;
		double riskFreeRate_;
		double dividendYield_;
		double volatility_;
		double sqrtT_;
		double volSqrtT_;
		double discountRate_;
		double discountDividend_;
		double d1_;
		double d2_;
		double nd1_;
		double nd2_;
		double normalPdfD1_;
	public:
		BlackScholesContext(const quantforge::instruments::EuropeanOption&,
			const quantforge::market::MarketData&);

		quantforge::instruments::OptionType optionType() const;

		double spot() const;
		double strike() const;
		double maturity() const;
		double riskFreeRate() const;
		double dividendYield() const;
		double volatility() const;

		double sqrtT() const;
		double volSqrtT() const;
		double discountRate() const;
		double discountDividend() const;

		double d1() const;
		double d2() const;
		double nd1() const;
		double nd2() const;
		double normalPdfD1() const;
	};
}