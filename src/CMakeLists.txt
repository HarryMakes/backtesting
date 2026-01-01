message(STATUS "Building and installing library.")

add_library(backtester
        STATIC common.cpp data_fetcher.cpp strategy.cpp backtester.cpp
               zzindic/zigzag.c
)

set_target_properties(backtester PROPERTIES
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER backtester.hpp
)

install(TARGETS backtester
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/backtester COMPONENT dev
)

message(STATUS "Building and installing main executable.")

add_executable(backtesting
        main.cpp
)
include_directories(../lib/spdlog/include)
include_directories(../lib/nlohmann_json/include)
target_link_libraries(
        backtesting
        PUBLIC backtester ${PROJECT_LIBS}
)

add_custom_command(TARGET backtesting POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:backtester>
    $<TARGET_FILE_DIR:backtesting>
    COMMENT "Copying dynamic libraries to executable directory"
)
