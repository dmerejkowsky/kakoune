#include <iostream>
#include <ncurses.h>
#include <gtest/gtest.h>


TEST(Ncurses, Version) {
    const auto actual = std::string{curses_version()};
    EXPECT_FALSE(actual.empty());
}
