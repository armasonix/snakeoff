cmake_minimum_required(VERSION 3.20)
project(SnakeOff CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# root
set(PROJ_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# SFML
find_package(SFML CONFIG REQUIRED COMPONENTS system window graphics audio)

# collect src/**
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${PROJ_ROOT}/src/*.cpp"
)

if (NOT SOURCES)
  message(FATAL_ERROR "not found any .cpp in ${PROJ_ROOT}/src. Check project structure.")
endif()

add_executable(SnakeOff ${SOURCES} "include/systems/HighScores.h" "src/systems/HighScores.cpp" "include/states/HighScoresState.h" "src/states/HighScoresState.cpp" "include/world/ProcGen.h" "src/world/ProcGen.cpp" "src/states/PauseState.cpp" "include/states/DifficultyState.h" "src/states/DifficultyState.cpp" "include/states/SettingsState.h" "src/states/SettingsState.cpp" "src/world/Grid.cpp" "include/states/VictoryState.h" "src/states/VictoryState.cpp" "include/states/PlayContext.h" "include/render/GridRenderer.h" "src/render/GridRenderer.cpp" "include/render/SnakeRenderer.h" "src/render/SnakeRenderer.cpp" "include/render/PortalsRenderer.h" "src/render/PortalsRenderer.cpp" "include/render/UIRenderer.h" "src/render/UIRenderer.cpp" "include/render/GridBatch.h" "src/render/GridBatch.cpp" "include/core/Perf.h" "include/render/ItemsRenderer.h" "src/render/ItemsRenderer.cpp" "include/render/DebugOverlay.h" "src/render/DebugOverlay.cpp" "include/render/BackgroundRenderer.h" "include/render/GateRenderer.h" "include/render/RendererRegistry.h" "include/render/SpriteRefs.h" "include/gfx/ChromAbParams.h" "include/render/ScopedView.h" "include/gfx/ShaderParams.h" "include/render/SpriteScaler.h" "include/gfx/ShaderInit.h" "include/render/ExplosionsRenderer.h" "src/render/ExplosionsRenderer.cpp" "include/gfx/Composite.h")

set(ALL_INC_DIRS
    ${PROJ_ROOT}/include
    ${PROJ_ROOT}/src
)

file(GLOB_RECURSE _INC_DIRS LIST_DIRECTORIES true "${PROJ_ROOT}/include/*")
foreach(_p IN LISTS _INC_DIRS)
  if (IS_DIRECTORY "${_p}")
    list(APPEND ALL_INC_DIRS "${_p}")
  endif()
endforeach()

file(GLOB_RECURSE _SRC_DIRS LIST_DIRECTORIES true "${PROJ_ROOT}/src/*")
foreach(_p IN LISTS _SRC_DIRS)
  if (IS_DIRECTORY "${_p}")
    list(APPEND ALL_INC_DIRS "${_p}")
  endif()
endforeach()

list(REMOVE_DUPLICATES ALL_INC_DIRS)
target_include_directories(SnakeOff PRIVATE ${PROJ_ROOT}/include)

# link SFML
target_link_libraries(SnakeOff PRIVATE sfml-system sfml-window sfml-graphics sfml-audio)

set(ASSETS_DIR "${CMAKE_SOURCE_DIR}/assets")
set(DATA_DIR   "${CMAKE_SOURCE_DIR}/data")

add_custom_command(TARGET SnakeOff POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${ASSETS_DIR}" "$<TARGET_FILE_DIR:SnakeOff>/assets"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${DATA_DIR}"   "$<TARGET_FILE_DIR:SnakeOff>/data"
)

get_filename_component(_sfml_prefix "${SFML_DIR}/../../.." ABSOLUTE)
set(_sfml_bin "${_sfml_prefix}/bin")

add_custom_command(TARGET SnakeOff POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copy SFML DLLs from: ${_sfml_bin}"
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${_sfml_bin}" "$<TARGET_FILE_DIR:SnakeOff>"
)

set_property(TARGET SnakeOff PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:SnakeOff>")
set_property(TARGET SnakeOff PROPERTY VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:SnakeOff>;%PATH%")

if (MSVC)
  target_compile_options(SnakeOff PRIVATE /W4 /permissive- /Zc:preprocessor)
else()
  target_compile_options(SnakeOff PRIVATE -Wall -Wextra -Wpedantic)
endif()