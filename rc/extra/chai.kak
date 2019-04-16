kak.hook("global", "bufcreate", ".*[.](js)x?", fun(x) {
  kak.set_option("buffer", "filetype", "chai");
})
