#include "pch.h"

#include "ParallelMonteCarloPricer.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <stdexcept>
#include <thread>
#include <vector>

namespace quantforge::pricing {

	ParallelMonteCarloPricer::ParallelMonteCarloPricer(
		std::size_t paths,
		std::uint64_t seed,
		std::size_t workers
	) :
		paths_(paths),
		seed_(seed),
		workers_(workers)
	{
		if (paths_ == 0) {
			throw std::invalid_argument(
				"Path count must be strictly positive"
			);
		}

		if (workers_ == 0) {
			throw std::invalid_argument(
				"Worker count must be strictly positive"
			);
		}
	}


	std::size_t ParallelMonteCarloPricer::workerCount() const {
		return workers_;
	}


	double ParallelMonteCarloPricer::price(
		const quantforge::instruments::EuropeanOption& option,
		const quantforge::market::MarketData& marketData
	) const {

		const double spot = marketData.spot();
		const double riskFreeRate = marketData.riskFreeRate();
		const double dividendYield = marketData.dividendYield();
		const double volatility = marketData.volatility();
		const double maturity = option.maturity();

		const double drift =
			(
				riskFreeRate
				- dividendYield
				- 0.5 * volatility * volatility
				)
			* maturity;

		const double diffusionScale =
			volatility * std::sqrt(maturity);

		const double discount =
			std::exp(-riskFreeRate * maturity);


		// Ne jamais créer plus de threads que de paths.
		const std::size_t actualWorkers =
			std::min(workers_, paths_);


		const std::size_t basePaths =
			paths_ / actualWorkers;

		const std::size_t remainder =
			paths_ % actualWorkers;


		std::vector<double> partialSums(
			actualWorkers,
			0.0
		);


		{
			std::vector<std::jthread> threads;

			threads.reserve(actualWorkers);


			for (
				std::size_t workerIndex = 0;
				workerIndex < actualWorkers;
				++workerIndex
				) {

				const std::size_t workerPaths =
					basePaths
					+ (workerIndex < remainder ? 1 : 0);


				threads.emplace_back(
					[&option,
					spot,
					drift,
					diffusionScale,
					workerPaths,
					workerIndex,
					this,
					&partialSums]()
					{

						const std::uint64_t workerSeed =
							seed_
							+ static_cast<std::uint64_t>(
								workerIndex
								);

						std::mt19937_64 generator{
							workerSeed
						};

						std::normal_distribution<double> normal{
							0.0,
							1.0
						};


						double localPayoffSum = 0.0;


						for (
							std::size_t i = 0;
							i < workerPaths;
							++i
							) {

							const double z =
								normal(generator);

							const double terminalSpot =
								spot
								* std::exp(
									drift
									+ diffusionScale * z
								);

							localPayoffSum +=
								option.payoff(
									terminalSpot
								);
						}


						partialSums[workerIndex] =
							localPayoffSum;
					}
				);
			}

		} // tous les std::jthread sont détruits → join automatique


		const double payoffSum =
			std::accumulate(
				partialSums.begin(),
				partialSums.end(),
				0.0
			);


		const double averagePayoff =
			payoffSum
			/ static_cast<double>(paths_);


		return discount * averagePayoff;
	}

}