cc_binary(
  name = "program",
  srcs = ["globlist.h", "main.cc", "globals.cc"],
  deps = [":globals"],
  copts = ["-std=c++14"],
)

cc_library(
  name = "globals",
  hdrs = ["globals.h"],
  srcs = ["globals.cc"],
  includes = ["globlist.h"],
  copts = ["-std=c++14"],
)

cc_test(
  name = "globals_test",
  srcs = ["globals_test.cc"],
  deps = [
    ":globals",
    "@googletest//:gtest",
    "@googletest//:gtest_main",
  ],
  copts = ["-std=c++14"],
)
