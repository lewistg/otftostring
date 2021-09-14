cc_binary(
    name = "otf-reader",
    srcs = [
        "main.c",
        "table-directory.c",
        "table-directory.h",
        "cmap-table.h",
        "cmap-table.c",
    ],
    deps = [
        ":fread-endian",
        ":cff"
    ]
)

cc_binary(
    name = "cff-reader",
    srcs = [
        "cff-reader.c",
    ],
    deps = [
        ":cff",
    ]
)

cc_library(
    name = "fread-endian",
    srcs = [
        "fread-endian.c"
    ],
    hdrs = [
        "fread-endian.h"
    ]
)

cc_library(
    name = "cff",
    srcs = [
        "cff-table.h",
        "cff-table.c",
    ],
    deps = [
        ":fread-endian",
    ]
)
