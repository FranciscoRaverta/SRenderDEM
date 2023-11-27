cmake_minimum_required(VERSION 3.10)
project(renderdem VERSION 1.0 LANGUAGES CXX)

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
find_package(PDAL REQUIRED)

include_directories(${PDAL_INCLUDE_DIRS})

set(SOURCES point_io.cpp render.cpp)
set(HEADERS point_io.hpp utils.hpp render.hpp)

add_executable(renderdem main.cpp ${SOURCES} ${HEADERS})
target_link_libraries(renderdem ${STDPPFS_LIBRARY} OpenMP::OpenMP_CXX ${PDAL_LIBRARIES})
install(TARGETS renderdem RUNTIME DESTINATION bin)