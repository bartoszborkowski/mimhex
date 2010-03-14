INCLUDE_DIRECTORIES (../lib ../patterns ../implementation)

ADD_EXECUTABLE (hash_test hash_test.cpp)
ADD_EXECUTABLE (simple_pattern_test simple_pattern_test.cpp)
ADD_EXECUTABLE (sampler_test sampler_test.cpp)

ADD_TEST (hash_test hash_test)
ADD_TEST (pattern_test simple_pattern_test)
ADD_TEST (sampler_test sampler_test)
