cmake_minimum_required (VERSION 3.9)

project (boerboel_mktdata
         VERSION 1.0.0
         DESCRIPTION "Boerboel Market Data Interview Question"
         HOMEPAGE_URL "https://www.boerboeltrading.com"
         LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 17)

# Control symbol visibility
#
set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Boost
#
set(Boost_USE_MULTITHREADED   ON)
set(Boost_USE_STATIC_LIBS     ON)
set(Boost_USE_STATIC_RUNTIME  OFF)
find_package (Boost 1.75.0 REQUIRED)

# Implement the solution as a library
#
add_library (mktdata_lib STATIC
    mktdata.cpp
)

set_target_properties (mktdata_lib
                       PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories (mktdata_lib
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (mktdata_lib
                       PRIVATE Boost::boost)

# Test driver, linked against the library
#
add_executable (mktdata_test
    mktdata_test.cpp
)

target_include_directories (mktdata_test
                            PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries (mktdata_test
                       PRIVATE mktdata_lib)
