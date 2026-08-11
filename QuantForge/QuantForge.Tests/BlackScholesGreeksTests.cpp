#include "pch.h"

#include "BlackScholesGreeks.h"
#include "OptionType.h"

namespace quantforge::pricing {

	class BlackScholesGreeksTest : public ::testing::Test
	{
	protected:
		quantforge::instruments::EuropeanOption callOption{
			"OPT_AAPL_001",
			1.0,
			100.0,
			quantforge::instruments::OptionType::Call
	    };
		quantforge::instruments::EuropeanOption putOption{
			"OPT_AAPL_001",
			1.0,
			100.0,
			quantforge::instruments::OptionType::Put
	    };

	    quantforge::market::MarketData marketData{
			100.0,
			0.05,
			0.02,
			0.20
	    };
			
	};
	TEST_F(BlackScholesGreeksTest, ComputesDeltaCall) {
		BlackScholesContext context(callOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.delta, 0.5868511461,0.001);
	}
	TEST_F(BlackScholesGreeksTest, ComputesDeltaPut) {
		BlackScholesContext context(putOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.delta, -0.3933475272, 0.001);
	}

	TEST_F(BlackScholesGreeksTest, ComputesGammaCall) {
		BlackScholesContext context(callOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.gamma, 0.0189506, 0.001);
	}

	TEST_F(BlackScholesGreeksTest, ComputesGammaPut) {
		BlackScholesContext context(putOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.gamma, 0.0189506, 0.001);
	}

	TEST_F(BlackScholesGreeksTest, ComputesVegaCall) {
		BlackScholesContext context(callOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.vega, 37.9012, 0.001);
	}

	TEST_F(BlackScholesGreeksTest, ComputesVegaPut) {
		BlackScholesContext context(putOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.vega, 37.9012, 0.001);
	}

	TEST_F(BlackScholesGreeksTest, ComputesThetaCall) {
		BlackScholesContext context(callOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.theta, -5.0893, 0.001);
	}

	TEST_F(BlackScholesGreeksTest, ComputesThetaPut) {
		BlackScholesContext context(putOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.theta, -2.2936, 0.001);
	}
	TEST_F(BlackScholesGreeksTest, ComputesRhoCall) {
		BlackScholesContext context(callOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.rho, 49.4581, 0.001);
	}
	TEST_F(BlackScholesGreeksTest, ComputesRhoPut) {
		BlackScholesContext context(putOption, marketData);
		BlackScholesGreeks greeks;
		const Greeks result = greeks.calculate(context);
		EXPECT_NEAR(result.rho, -45.6648, 0.001);
	}
}