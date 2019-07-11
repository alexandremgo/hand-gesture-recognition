cmake_minimum_required(VERSION 2.8)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_REQUIRED_FLAGS -std=c++17 -lstdc++fs)
set(CMAKE_CXX_STANDARD 17)

project(lib_koala_project)

find_package( Boost REQUIRED )

include_directories(
    ${Boost_INCLUDE_DIR}
)


add_subdirectory(src)
add_subdirectory(examples)
