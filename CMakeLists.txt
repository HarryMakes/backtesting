cmake_minimum_required(VERSION 3.10.0)

include(GNUInstallDirs)

project(backtesting VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent) # can be used with FetchContent_Declare if package (lib) is not installed system-wide

find_package(CURL REQUIRED)
find_package(spdlog REQUIRED)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)


set(PROJECT_LIBS
        CURL::libcurl
        nlohmann_json::nlohmann_json
        spdlog::spdlog
)

include_directories(src)
add_subdirectory(src)

include(CTest)
add_subdirectory(test)
