cmake_minimum_required(VERSION 3.10)
project(renderdem VERSION 1.0 LANGUAGES CXX)

SET(WITH_PDAL ON CACHE BOOL "Build PDAL readers support")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
      "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
      FORCE)
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if (NOT WIN32 AND NOT APPLE)
    set(STDPPFS_LIBRARY stdc++fs)
endif()

find_package(OpenMP REQUIRED)

if (WITH_PDAL)
    find_package(PDAL)
    if (PDAL_FOUND)
        include_directories(${PDAL_INCLUDE_DIRS})
    else()
        message(WARNING "PDAL not found, LAS/LAZ support will not be built")
        set(WITH_PDAL OFF)
    endif()
endif()

set(SOURCES point_io.cpp)
set(HEADERS point_io.hpp utils.hpp)

if (WITH_PDAL)
    add_definitions(-DWITH_PDAL)
    set(PDAL_LIB ${PDAL_LIBRARIES})
endif()

add_executable(renderdem main.cpp ${SOURCES} ${HEADERS})
target_link_libraries(renderdem ${STDPPFS_LIBRARY} OpenMP::OpenMP_CXX ${PDAL_LIB})
