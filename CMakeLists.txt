CMAKE_MINIMUM_REQUIRED (VERSION 2.6)
SET (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
ENABLE_TESTING ()

PROJECT (MIMHEX)

ADD_DEFINITIONS ("-ansi")
ADD_DEFINITIONS ("-W -Wall -Wextra")
ADD_DEFINITIONS ("-O3 -fomit-frame-pointer")

ADD_SUBDIRECTORY (main)
ADD_SUBDIRECTORY (patterns)
ADD_SUBDIRECTORY (tests)
ADD_SUBDIRECTORY (benchmarks)
