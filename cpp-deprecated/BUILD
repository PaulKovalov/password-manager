load("@rules_cc//cc:defs.bzl", "cc_binary")


cc_library(
    name = "hasher",
    srcs = ["hasher.cpp"],
    hdrs = ["hasher.hpp"],
    deps = [
        "@lcrypto//:lib",
    ],
)

cc_library(
    name = "aes",
    srcs = ["aes.cpp"],
    hdrs = ["aes.hpp"],
    deps = [
        ":hasher",
        "@lcrypto//:lib",
    ]
)

cc_library(
    name = "env",
    srcs = ["env.cpp"],
    hdrs = ["env.hpp"],
)

cc_binary(
    name = "password-manager",
    srcs = ["main.cpp"],
    deps = [
        ":aes",
        ":env",
        ":hasher"
    ],
)

