message(STATUS "Building and installing library.")

add_library(backtester
        STATIC data_fetcher.cpp strategy.cpp
)

set_target_properties(backtester PROPERTIES
        VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER backtester.hpp
)

install(TARGETS backtester
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/backtester COMPONENT dev
)
