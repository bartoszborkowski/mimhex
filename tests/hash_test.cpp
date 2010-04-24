/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#include <algorithm>
#include <iostream>
#include <vector>

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

using namespace HexPatterns;

int main()
/*
 * A test program that checks whether the hashes are well distributed.
 */
{
    std::vector<Hash> aux, xors;

    rep(ii, TEMPLATES_AMOUNT) {
        aux = templates[ii].GetAllHashes();
        xors.insert(xors.begin(), aux.begin(), aux.end());
    }

    sort(xors.begin(), xors.end());

    rep(ii, xors.size() - 1)
        if (xors[ii] == xors[ii + 1])
            return 1;

    return 0;
}
