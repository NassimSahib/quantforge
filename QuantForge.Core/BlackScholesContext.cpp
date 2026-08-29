#include "pch.h"

#include "BlackScholesContext.h"
#include <stdexcept>
#include <cmath>
#include <numbers>

namespace
{
	double normalCdf(double x) {
		return 0.5 * (1.0 + std::erf(x / std::sqrt(2.0)));
	}

	double normalPdf(double x) {
		return 1.0 / std::sqrt(2.0 * std::numbers::pi) * std::exp(-x * x / 2);
	}
}

namespace quantforge::pricing {
	BlackScholesContext::BlackScholesContext(const quantforge::instruments::EuropeanOption& option,
		const quantforge::market::MarketData& marketData)
		: optionType_(option.optionType()),
		spot_(marketData.spot()),
		strike_(option.strike()),
		maturity_(option.maturity()),
		riskFreeRate_(marketData.riskFreeRate()),
		dividendYield_(marketData.dividendYield()),
		volatility_(marketData.volatility())
	{
		if (volatility_ <= 0.0)
			throw std::invalid_argument("Volatility must be strictly positive");

		sqrtT_ = std::sqrt(maturity_);
		volSqrtT_ = volatility_ * sqrtT_;
		discountRate_ = std::exp(-riskFreeRate_ * maturity_);
		discountDividend_ = std::exp(-dividendYield_ * maturity_);

		const double logMoneyness = std::log(spot_ / strike_);
		const double numerator = logMoneyness + (riskFreeRate_ - dividendYield_ + 0.5 * volatility_ * volatility_) * maturity_;
		d1_ = numerator / volSqrtT_;
		d2_ = d1_ - volSqrtT_;

		nd1_ = normalCdf(d1_);
		nd2_ = normalCdf(d2_);

		normalPdfD1_ = normalPdf(d1_);
	}
	
	quantforge::instruments::OptionType BlackScholesContext::optionType() const { return optionType_; }

	double BlackScholesContext::spot() const { return spot_; }
	double BlackScholesContext::strike() const { return strike_; }
	double BlackScholesContext::maturity() const { return maturity_; }
	double BlackScholesContext::riskFreeRate() const { return riskFreeRate_; }
	double BlackScholesContext::dividendYield() const { return dividendYield_; }
	double BlackScholesContext::volatility() const { return volatility_; }

	double BlackScholesContext::sqrtT() const { return sqrtT_; }
	double BlackScholesContext::volSqrtT() const { return volSqrtT_; }
	double BlackScholesContext::discountRate() const { return discountRate_; }
	double BlackScholesContext::discountDividend() const { return discountDividend_; }

	double BlackScholesContext::d1() const { return d1_; }
	double BlackScholesContext::d2() const { return d2_; }
	double BlackScholesContext::nd1() const { return nd1_; }
	double BlackScholesContext::nd2() const { return nd2_; }
	double BlackScholesContext::normalPdfD1() const { return normalPdfD1_; }
}