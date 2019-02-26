cmake_minimum_required(VERSION 3.0)
project(kakoune)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses REQUIRED)


enable_testing()

set(CMAKE_CXX_STANDARD 14)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
message(STATUS "Building with ${CMAKE_BUILD_TYPE} settings")

# TODO: read .version or use git describe
set(KAKOUNE_VERSION dev)

set(_version_cpp ${CMAKE_BINARY_DIR}/version.cpp)
file(WRITE ${_version_cpp}
  "namespace Kakoune { const char* version = \"${KAKOUNE_VERSION}\"; }"
)


add_library(kakoune
  src/alias_registry.cc
  src/assert.cc
  src/backtrace.cc
  src/buffer.cc
  src/buffer_manager.cc
  src/buffer_utils.cc
  src/changes.cc
  src/client.cc
  src/client_manager.cc
  src/color.cc
  src/command_manager.cc
  src/commands.cc
  src/completion.cc
  src/context.cc
  src/display_buffer.cc
  src/env_vars.cc
  src/event_manager.cc
  src/exception.cc
  src/face_registry.cc
  src/file.cc
  src/hash.cc
  src/hash_map.cc
  src/highlighter.cc
  src/highlighter_group.cc
  src/highlighters.cc
  src/hook_manager.cc
  src/input_handler.cc
  src/insert_completer.cc
  src/json_ui.cc
  src/keymap_manager.cc
  src/keys.cc
  src/line_modification.cc
  src/memory.cc
  src/ncurses_ui.cc
  src/normal.cc
  src/option_manager.cc
  src/option_types.cc
  src/parameters_parser.cc
  src/ranges.cc
  src/ranked_match.cc
  src/regex.cc
  src/regex_impl.cc
  src/register_manager.cc
  src/remote.cc
  src/scope.cc
  src/selection.cc
  src/selectors.cc
  src/shared_string.cc
  src/shell_manager.cc
  src/string.cc
  src/string_utils.cc
  src/unit_tests.cc
  src/window.cc
  src/word_db.cc

  ${_version_cpp}
)
target_compile_definitions(kakoune PUBLIC "KAKOUNE_RUNTIME_PATH=\"${CMAKE_SOURCE_DIR}/share/kak\"")
target_include_directories(kakoune PUBLIC ${CURSES_INCLUDE_DIRS} include)
target_link_libraries(kakoune ${CURSES_LIBRARIES})


add_executable(kak
  src/main.cc
)
target_link_libraries(kak kakoune)

add_subdirectory(unittest)
