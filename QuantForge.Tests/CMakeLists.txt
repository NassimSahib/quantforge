include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

if(MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

add_executable(
    QuantForge.Tests

    BenchmarkReportWriterTests.cpp
    BlackScholesContextTests.cpp
    BlackScholesGreeksTests.cpp
    BlackScholesPricerTests.cpp
    ConsoleReportWriterTests.cpp
    CsvPortfolioLoaderTests.cpp
    EuropeanOptionTests.cpp
    MarketDataTests.cpp
    MonteCarloPricerTests.cpp
    MonteCarloScalingBenchmarkTests.cpp
    MonteCarloScalingReportWriterTests.cpp
    ParallelMonteCarloPricerTests.cpp
    PortfolioAnalyticsTests.cpp
    PortfolioReportBuilderTests.cpp
    PortfolioTests.cpp
    PositionTests.cpp
    PricingBenchmarkTests.cpp
    PricingComparisonTests.cpp
)

target_link_libraries(
    QuantForge.Tests
    PRIVATE
        QuantForge.Core
        GTest::gtest_main
)

target_compile_features(
    QuantForge.Tests
    PRIVATE
        cxx_std_20
)

include(GoogleTest)

gtest_discover_tests(
    QuantForge.Tests
)