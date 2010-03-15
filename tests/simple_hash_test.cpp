#include <algorithm>
#include <iostream>
/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <vector>

#include "board.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "macro_definitions.h"

using namespace HexPatterns;

int main()
/*
 * A simple test program that checks whether the hashes are well distributed.
 */
{
    uint h[6][FIELD_STATES];
    std::vector<uint> xors;

    rep(i, FIELD_STATES) {
        h[0][i] = templates[0].GetHash(-16, i);
        h[1][i] = templates[0].GetHash(-15, i);
        h[2][i] = templates[0].GetHash(-1, i);
        h[3][i] = templates[0].GetHash(1, i);
        h[4][i] = templates[0].GetHash(15, i);
        h[5][i] = templates[0].GetHash(16, i);
    }

    rep(f0, FIELD_STATES)
    rep(f1, FIELD_STATES)
    rep(f2, FIELD_STATES)
    rep(f3, FIELD_STATES)
    rep(f4, FIELD_STATES)
    rep(f5, FIELD_STATES)
        xors.push_back
            (h[0][f0] ^ h[1][f1] ^ h[2][f2] ^ h[3][f3] ^ h[4][f4] ^ h[5][f5]);

    sort(xors.begin(), xors.end());

    rep(i, xors.size() - 1)
        if (xors[i] == xors[i + 1])
            return 1;

    return 0;
}
