cmake_minimum_required(VERSION 2.8)
project(DC)

if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99")

	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -lc++")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
	endif()

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -Wno-nested-anon-types")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I '/opt/local/include/eigen2'")
endif()

#SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

include_directories(${DC_SOURCE_DIR}/src/math)
include_directories(${DC_SOURCE_DIR}/src/vol)
include_directories(${DC_SOURCE_DIR}/src/util)
include_directories(${DC_SOURCE_DIR}/src)

add_executable(dc
	src/main.cpp
	src/math/Math.cpp
	src/math/Solver.cpp
	src/vol/Contouring.cpp
)

add_executable(test
	src/test.cpp
	src/math/Math.cpp
	src/math/Solver.cpp
	src/vol/Contouring.cpp
)

add_test(core test)
