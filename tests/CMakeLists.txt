INCLUDE_DIRECTORIES (../lib ../patterns ../implementation)

ADD_EXECUTABLE (simple_hash_test simple_hash_test.cpp)
ADD_EXECUTABLE (simple_pattern_test simple_pattern_test.cpp)

ADD_TEST (test1 simple_hash_test)
ADD_TEST (test2 simple_pattern_test)
