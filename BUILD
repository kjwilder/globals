cc_binary(
  name = "program",
  srcs = ["globlist.h", "main.cc", "globals.cc"],
  deps = [":globals"],
)

cc_library(
  name = "globals",
  hdrs = ["globals.h", "globals.cc"],
  includes = ["globlist.h"],
)
