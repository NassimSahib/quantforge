add_executable(
    QuantForge.CLI
    CliApplication.cpp
    QuantForge.CLI.cpp
)

target_link_libraries(
    QuantForge.CLI
    PRIVATE
        QuantForge.Core
)

target_compile_features(
    QuantForge.CLI
    PRIVATE
        cxx_std_20
)