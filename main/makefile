CXX = g++
<<<<<<< HEAD
OPTIONS = -Wall -W -fomit-frame-pointer -O3
INCLUDE_DIR = -I ../lib -I ../implementation -I ../patterns -I ../../../Downloads/boost_1_38_0
=======
OPTIONS = -Wall -W -fomit-frame-pointer -O3 -g
INCLUDE_DIR = -I ../lib -I ../implementation -I ../../../Downloads/boost_1_38_0
>>>>>>> 0f6b689f4501bab17d2305125c32e1928216c5c9
DEST_DIR = .

all: main

benchmark:
	$(CXX) $(OPTIONS) $(INCLUDE_DIR) -o $(DEST_DIR)/hex_benchmark main_benchmark.cpp

main:
	$(CXX) $(OPTIONS) $(INCLUDE_DIR) -o $(DEST_DIR)/hex main.cpp

clean:
	rm -f hex hex_benchmark
