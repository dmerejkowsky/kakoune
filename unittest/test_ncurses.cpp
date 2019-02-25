#include <iostream>
#include <ncurses.h>
#include <gtest/gtest.h>


TEST(Ncurses, Version) {
    auto const actual = std::string{curses_version()};
    EXPECT_FALSE(actual.empty());
}
