cmake_minimum_required(VERSION 3.24)
project(lab6_clion)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost 1.57.0)

if(Boost_FOUND)

    include_directories(${Boost_INCLUDE_DIRS})

endif()

add_executable(lab6_clion main.cpp DirectedGraph.cpp DirectedGraph.h)

if(Boost_FOUND)

    target_link_libraries(lab6_clion ${Boost_LIBRARIES})

endif()
