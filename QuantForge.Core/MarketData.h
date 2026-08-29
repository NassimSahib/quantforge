#pragma once

namespace quantforge::market {
	class MarketData {
	private:
		double spot_;
		double riskFreeRate_;
		double dividendYield_;
		double volatility_;
	public:
		MarketData(double spot, double riskFreeRate, double dividendYield, double volatility);
		double spot() const;
		double riskFreeRate() const;
		double dividendYield() const;
		double volatility() const;
	};
}