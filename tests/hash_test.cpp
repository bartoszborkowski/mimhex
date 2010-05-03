/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "random.cpp"
#include "board_dim.cpp"
#include "board_player.cpp"
#include "board_location.cpp"
#include "board_move.cpp"
#include "board.cpp"
#include "params.cpp"
#include "switches.cpp"
#include "time_manager.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "pattern_data.cpp"

using namespace HexPatterns;

Hash CalculateHash(Hash h[][FIELD_STATES],
    uint f0, uint f1, uint f2, uint f3, uint f4, uint f5)
{
    return h[0][f0] ^ h[1][f1] ^ h[2][f2] ^ h[3][f3] ^ h[4][f4] ^ h[5][f5];
}

int main()
/*
 * A test program that checks whether the hashes are well distributed.
 */
{
    std::vector<Hash> aux, xors;

    rep(ii, TEMPLATES_AMOUNT) {
        aux = Template::templates[ii].GetAllHashes();
        xors.insert(xors.begin(), aux.begin(), aux.end());
    }

    rep(ii, xors.size())
        xors[ii] &= PATT_MASK;

    sort(xors.begin(), xors.end());

    rep(ii, xors.size() - 1)
        if (xors[ii] == xors[ii + 1]) {
            std::cerr << "Collision at " << ii << std::endl;
            exit(1);
        }

    return 0;
}
