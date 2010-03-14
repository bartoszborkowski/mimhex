/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include "board.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "pattern_data.cpp"

int main(int argc, char *argv[])
/*
 * A test program that checks whether the sampler works correctly.
 */
{
    Hex::Sampler sampler;
    uint loops = 1000000;
    uint pos;
    Hex::Player player(Hex::Player::First());

    if (argc == 2)
        try {
            loops = boost::lexical_cast<uint>(argv[1]);
        } catch (boost::bad_lexical_cast &) {
            std::cerr << "Error: " << argv[1] << " is not an integer" << std::endl;
            exit(1);
        }

    rep(ii, loops) {
        player = Hex::Player::First();
        rep(jj, Hex::kBoardSize * Hex::kBoardSize) {
            pos = sampler.RandomMove();
            sampler.Play(Hex::Move(player, Hex::Location(pos)));
            player = player.Opponent();
        }
    }

    return 0;
}
