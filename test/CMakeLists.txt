message(STATUS "Building tests.")

include(FetchContent) # can be used with FetchContent_Declare if package (lib) is not installed system-wide

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests
        test_data_fetcher.cpp
        test_strategy.cpp
)
target_link_libraries(
        unit_tests
        PRIVATE backtester ${PROJECT_LIBS}
        PUBLIC Catch2::Catch2 Catch2::Catch2WithMain
)

add_test(NAME test COMMAND unit_tests)

