cmake_minimum_required(VERSION 3.0)
project(kakoune)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if(APPLE)
 # We only support ncurses built by brew, so no need to look
 # for the curses library, we know wher it is
 set(CURSES_INCLUDE_DIRS "/usr/local/opt/ncurses/include")
 set(CURSES_LIBRARIES "/usr/local/opt/ncurses/lib/libncurses.dylib")
else()
  set(CURSES_NEED_NCURSES TRUE)
#  set(CURSES_NEED_WIDE TRUE)
  find_package(Curses REQUIRED)
endif()

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
  src/alias_registry.hh
  src/array_view.hh
  src/assert.cc
  src/assert.hh
  src/backtrace.cc
  src/backtrace.hh
  src/buffer.cc
  src/buffer.hh
  src/buffer.inl.hh
  src/buffer_manager.cc
  src/buffer_manager.hh
  src/buffer_utils.cc
  src/buffer_utils.hh
  src/changes.cc
  src/changes.hh
  src/client.cc
  src/client.hh
  src/client_manager.cc
  src/client_manager.hh
  src/clock.hh
  src/color.cc
  src/color.hh
  src/command_manager.cc
  src/command_manager.hh
  src/commands.cc
  src/commands.hh
  src/completion.cc
  src/completion.hh
  src/constexpr_utils.hh
  src/context.cc
  src/context.hh
  src/coord.hh
  src/diff.hh
  src/display_buffer.cc
  src/display_buffer.hh
  src/enum.hh
  src/env_vars.cc
  src/env_vars.hh
  src/event_manager.cc
  src/event_manager.hh
  src/exception.cc
  src/exception.hh
  src/face.hh
  src/face_registry.cc
  src/face_registry.hh
  src/file.cc
  src/file.hh
  src/flags.hh
  src/hash.cc
  src/hash.hh
  src/hash_map.cc
  src/hash_map.hh
  src/highlighter.cc
  src/highlighter_group.cc
  src/highlighter_group.hh
  src/highlighter.hh
  src/highlighters.cc
  src/highlighters.hh
  src/hook_manager.cc
  src/hook_manager.hh
  src/input_handler.cc
  src/input_handler.hh
  src/insert_completer.cc
  src/insert_completer.hh
  src/json_ui.cc
  src/json_ui.hh
  src/keymap_manager.cc
  src/keymap_manager.hh
  src/keys.cc
  src/keys.hh
  src/line_modification.cc
  src/line_modification.hh
  src/memory.cc
  src/memory.hh
  src/meta.hh
  src/ncurses_ui.cc
  src/ncurses_ui.hh
  src/normal.cc
  src/normal.hh
  src/optional.hh
  src/option.hh
  src/option_manager.cc
  src/option_manager.hh
  src/option_types.cc
  src/option_types.hh
  src/parameters_parser.cc
  src/parameters_parser.hh
  src/range.hh
  src/ranges.cc
  src/ranges.hh
  src/ranked_match.cc
  src/ranked_match.hh
  src/ref_ptr.hh
  src/regex.cc
  src/regex.hh
  src/regex_impl.cc
  src/regex_impl.hh
  src/register_manager.cc
  src/register_manager.hh
  src/remote.cc
  src/remote.hh
  src/safe_ptr.hh
  src/script.cc
  src/script.hh
  src/scope.cc
  src/scope.hh
  src/selection.cc
  src/selection.hh
  src/selectors.cc
  src/selectors.hh
  src/shared_string.cc
  src/shared_string.hh
  src/shell_manager.cc
  src/shell_manager.hh
  src/string.cc
  src/string.hh
  src/string_utils.cc
  src/string_utils.hh
  src/unicode.hh
  src/units.hh
  src/unit_tests.cc
  src/unit_tests.hh
  src/user_interface.hh
  src/utf8.hh
  src/utf8_iterator.hh
  src/utils.hh
  src/value.hh
  src/vector.hh
  src/window.cc
  src/window.hh
  src/word_db.cc
  src/word_db.hh

  ${_version_cpp}
)

set(_kak_defs)
target_compile_definitions(kakoune PUBLIC "KAKOUNE_RUNTIME_PATH=\"${CMAKE_SOURCE_DIR}/share/kak\"")
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  list(APPEND _kak_defs KAK_DEBUG=1)
endif()

target_compile_definitions(kakoune PUBLIC ${_kak_defs} )
target_include_directories(kakoune PUBLIC ${CURSES_INCLUDE_DIRS})
target_link_libraries(kakoune ${CURSES_LIBRARIES} CONAN_PKG::chaiscript dl pthread)


add_executable(kak
  src/main.cc
)
target_link_libraries(kak kakoune)
