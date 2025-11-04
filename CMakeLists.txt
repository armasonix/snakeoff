cmake_minimum_required(VERSION 3.20)
project(SnakeOff CXX)

# build options
option(SNAKEOFF_WERROR "Treat project warnings as errors" OFF)
option(SNAKEOFF_UNITY  "Enable Unity/jumbo builds for faster compilation" OFF)
option(SNAKEOFF_LTO    "Enable link-time optimization in Release" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# SFML
find_package(SFML CONFIG REQUIRED COMPONENTS system window graphics audio)

# build sources from src/**
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${PROJ_ROOT}/src/*.cpp"
)
if (NOT SOURCES)
  message(FATAL_ERROR "No .cpp files found in ${PROJ_ROOT}/src")
endif()

add_executable(SnakeOff ${SOURCES})

# includes
target_include_directories(SnakeOff PRIVATE
  ${PROJ_ROOT}/include
)

# vs files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
source_group(TREE ${PROJ_ROOT} FILES ${SOURCES})

# warnings
if (MSVC)
  target_compile_options(SnakeOff PRIVATE
    /W4
    /permissive-
    /Zc:preprocessor
    /EHsc
    /external:W0
    /external:anglebrackets
  )
  if (SNAKEOFF_WERROR)
    target_compile_options(SnakeOff PRIVATE /WX)
  endif()
else()
  target_compile_options(SnakeOff PRIVATE -Wall -Wextra -Wpedantic)
  if (SNAKEOFF_WERROR)
    target_compile_options(SnakeOff PRIVATE -Werror)
  endif()
endif()

# windows/crt
target_compile_definitions(SnakeOff PRIVATE
  NOMINMAX
  _CRT_SECURE_NO_WARNINGS
)

# SFML linking
target_link_libraries(SnakeOff PRIVATE
  sfml-system sfml-window sfml-graphics sfml-audio
)

# resources
set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(DATA_DIR   "${CMAKE_SOURCE_DIR}/data")

add_custom_command(TARGET SnakeOff POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:SnakeOff>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${DATA_DIR}"   "$<TARGET_FILE_DIR:SnakeOff>/data"
)

# DLL SFML
get_filename_component(_sfml_prefix "${SFML_DIR}/../../.." ABSOLUTE)
set(_sfml_bin "${_sfml_prefix}/bin")
add_custom_command(TARGET SnakeOff POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copy SFML DLLs from: ${_sfml_bin}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${_sfml_bin}" "$<TARGET_FILE_DIR:SnakeOff>"
)

# dedbug
set_property(TARGET SnakeOff PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:SnakeOff>")
set_property(TARGET SnakeOff PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:SnakeOff>;%PATH%")

if (SNAKEOFF_UNITY)
  set_target_properties(SnakeOff PROPERTIES UNITY_BUILD ON)
endif()

if (SNAKEOFF_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT _ipo_supported OUTPUT _ipo_err)
  if(_ipo_supported)
    set_property(TARGET SnakeOff PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
  else()
    message(WARNING "IPO/LTO not supported: ${_ipo_err}")
  endif()
endif()