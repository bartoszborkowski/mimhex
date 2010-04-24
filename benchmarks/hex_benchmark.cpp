#include "board.h"
#include "board.cpp"
#include "board_location.cpp"
#include "board_move.cpp"
#include "board_player.cpp"
#include "board_dim.cpp"
#include "random.cpp"
#include "params.cpp"
#include "switches.cpp"
#include "playout.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "pattern_data.cpp"

#include <iostream>
#include <cstdlib>

int playouts_number = 100000;

int main(int argc, char* argv[]) {

	if (argc > 1)
		playouts_number = atoi(argv[1]);
	std::cout << "Starting " << playouts_number << " playouts..." << std::endl;
	uint n = Hex::DoPlayouts(Hex::Board::Empty(), playouts_number);
	std::cout << "First player won: " << n << std::endl;
	std::cout << "Second player won: " << playouts_number - n << std::endl;
	return 0;
}
