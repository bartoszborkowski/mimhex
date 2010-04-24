/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               19th March 2010                                *
 ********************************************************************************/

#include <iostream>
#include <time.h>

#include "board.cpp"
#include "board_location.cpp"
#include "board_move.cpp"
#include "board_player.cpp"
#include "board_dim.cpp"
#include "random.cpp"
#include "params.cpp"
#include "switches.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "pattern_data.cpp"

int main(int argc, char *argv[])
/*
 * A test program that checks the sampler's speed.
 */
{
    Hex::Sampler sampler;
    uint loops = 100000;
    uint pos;
    time_t begin, end;
    Hex::Player player(Hex::Player::First());

    if (argc == 2)
        try {
            loops = boost::lexical_cast<uint>(argv[1]);
        } catch (boost::bad_lexical_cast &) {
            std::cerr << "Error: " << argv[1] << " is not an integer" << std::endl;
            exit(1);
        }

    begin = time(0);

    rep(ii, loops) {
        player = Hex::Player::First();
        sampler = Hex::Sampler();

        rep(jj, Hex::Dim::actual_size * Hex::Dim::actual_size) {
            pos = sampler.RandomMove();
            sampler.Play(Hex::Move(player, Hex::Location(pos)));
            player = player.Opponent();
        }
    }

    end = time(0);

    std::cout << loops << " sampler loops lasted " << difftime(end, begin) << std::endl;

    return 0;
}
