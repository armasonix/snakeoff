cmake_minimum_required(VERSION 3.20)
project(SnakeOff CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SFML
find_package(SFML CONFIG REQUIRED COMPONENTS system window graphics audio)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
)

add_executable(SnakeOff ${SOURCES})

target_include_directories(SnakeOff PRIVATE include src)

# SFML libs
target_link_libraries(SnakeOff PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

if (MSVC)
  target_compile_options(SnakeOff PRIVATE /W4 /permissive- /Zc:preprocessor)
else()
  target_compile_options(SnakeOff PRIVATE -Wall -Wextra -Wpedantic)
endif()