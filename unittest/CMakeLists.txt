add_executable(test_kakoune
  test_ncurses.cpp
  main.cpp
)
target_link_libraries(test_kakoune
  kakoune
  CONAN_PKG::gtest
)
add_test(NAME test_kakoune COMMAND test_kakoune)
