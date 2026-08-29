add_library(QuantForge.Core STATIC)

target_sources(
    QuantForge.Core
    PRIVATE
        BenchmarkReportWriter.cpp
        BlackScholesContext.cpp
        BlackScholesGreeks.cpp
        BlackScholesPricer.cpp
        ConsoleReportWriter.cpp
        CsvPortfolioLoader.cpp
        EuropeanOption.cpp
        Instrument.cpp
        MarketData.cpp
        MonteCarloPricer.cpp
        MonteCarloScalingBenchmark.cpp
        MonteCarloScalingReportWriter.cpp
        ParallelMonteCarloPricer.cpp
        Portfolio.cpp
        PortfolioAnalytics.cpp
        PortfolioReportBuilder.cpp
        Position.cpp
        PricingBenchmark.cpp
        PricingComparison.cpp
)

target_include_directories(
    QuantForge.Core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(
    QuantForge.Core
    PUBLIC
        cxx_std_20
)

find_package(Threads REQUIRED)

target_link_libraries(
    QuantForge.Core
    PUBLIC
        Threads::Threads
)