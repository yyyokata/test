load("//magicmind:magicmind.bzl", "mm_cc_library", "mm_cc_binary", "mm_cc_test")

package(
    default_visibility = [
        "//magicmind:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

mm_cc_library(
    name = "api_test_util",
    hdrs = [
        "org_test_cc/api_test_util.h",
    ],
    srcs = ["org_test_cc/api_test_util.cc"],
    deps = [
	    "//magicmind/cc:cc",
        "//magicmind/runtime:toolkit_deps",
        "//magicmind/runtime/kernel:kernel_test_util",
        "//magicmind/runtime/core:tensor_util",
        "@com_google_googletest//:gtest",
    ],
)

mm_cc_test(
    name = "api_test",
    srcs = glob(["org_test_cc/*_test.cc"]),
    deps = [
	      "api_test_util",
    ],
)

mm_cc_test(
    name = "offline_conv_test",
    srcs = [
        "demo/offline_demo.cc",
    ],
    deps = [
	      "//magicmind/cc:cc",
    ],
    data = [
        ":conv_model",
    ],
)

mm_cc_test(
    name = "half_conv_test",
    srcs = ["demo/half_conv.cc"],
    deps = ["//magicmind/cc:cc"],
)

mm_cc_binary(
    name = "conv",
    srcs = [
        "demo/gen_model.cc",
    ],
    deps = [
	      "//magicmind/cc:cc",
    ],
)

genrule(
    name = "conv_model",
    outs = [
        "conv.graph",
        "conv.data",
    ],
    srcs = ["//magicmind/test:conv"],

    cmd = "$(location conv) $(OUTS)",
)
