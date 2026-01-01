cmake_minimum_required(VERSION 3.10.0)

project(backtesting VERSION 0.1.0 LANGUAGES C CXX)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

include(FetchContent) # can be used with FetchContent_Declare if package (lib) is not installed system-wide

find_package(CURL REQUIRED)

set(SPDLOG_FMT_EXTERNAL OFF CACHE BOOL "Toggle using external fmt library" FORCE)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/nlohmann_json)

set(PROJECT_LIBS
        CURL::libcurl
        spdlog::spdlog
        nlohmann_json::nlohmann_json
)

include_directories(src)
add_subdirectory(src)

include(CTest)
add_subdirectory(test)
