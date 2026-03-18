load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

genrule(
    name = "gen_globlist",
    srcs = ["globlist_example.h"],
    outs = ["globlist.h"],
    cmd = "cp $< $@",
)

cc_binary(
    name = "program",
    srcs = [
        "globals.cc",
        "globlist_example.h",
        "main.cc",
    ],
    deps = [":globals"],
)

cc_library(
    name = "globals",
    hdrs = [
        "globals.cc",
        "globals.h",
        ":gen_globlist",
    ],
)

cc_test(
    name = "globals_test",
    srcs = [
        "globals.cc",
        "globals.h",
        "test/globals_test.cc",
        "test/globlist.h",
    ],
    copts = ["-I test"],
)
