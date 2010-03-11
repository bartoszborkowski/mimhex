CXX = g++
OPTIONS = -Wall -W -fomit-frame-pointer -O3
INCLUDE_DIR = -I ../lib -I ../implementation -I ../../../Downloads/boost_1_38_0
DEST_DIR = .

all: main

benchmark:
	$(CXX) $(OPTIONS) $(INCLUDE_DIR) -o $(DEST_DIR)/hex_benchmark main_benchmark.cpp

main:
	$(CXX) $(OPTIONS) $(INCLUDE_DIR) -o $(DEST_DIR)/hex main.cpp

clean:
	rm -f hex hex_benchmark
