INCLUDE_DIRECTORIES (../lib ../patterns ../implementation)

ADD_EXECUTABLE (simple_hash_test simple_hash_test.cpp)
ADD_EXECUTABLE (simple_pattern_test simple_pattern_test.cpp)

ADD_TEST (hash_test simple_hash_test)
ADD_TEST (pattern_test simple_pattern_test)
