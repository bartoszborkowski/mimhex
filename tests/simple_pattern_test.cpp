/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <iostream>

#include <inttypes.h>
#include <time.h>

#include <boost/random/linear_congruential.hpp>

#include "board.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "pattern_data.cpp"

using namespace HexPatterns;

uint Compare(uint p1, uint f1, uint p2, uint f2)
{
    if (patterns[p1].HashChange(f1, 0) && patterns[p2].HashChange(f2, 0))
        rep (ii, FIELD_STATES)
            if (patterns[p1].HashChange(f1, ii) != patterns[p2].HashChange(f2, ii))
                return 1;

    return 0;
}

int main()
/*
 * A simple test for patterns created on the board.
 */
{
    boost::rand48 random_generator;
    const uint size = Hex::kBoardSize * Hex::kBoardSize;
    uint board[size];

    random_generator.seed(static_cast<int32_t>(time(0)));

    rep(ii, Hex::kBoardSize)
        rep(jj, Hex::kBoardSize)
            board[ii * Hex::kBoardSize + jj] = NORMALISED_POSITION(ii, jj);

    rep(ii, Hex::kFieldsAlignedAmount * Hex::kFieldsAlignedAmount * 100) {
        uint f1 = board[random_generator() % size];
        uint f2 = board[random_generator() % size];

        if (Compare(f1, f1 - 16, f2, f2 - 16) ||
            Compare(f1, f1 - 15, f2, f2 - 15) ||
            Compare(f1, f1 - 1, f2, f2 - 1) ||
            Compare(f1, f1 + 1, f2, f2 + 1) ||
            Compare(f1, f1 + 15, f2, f2 + 15) ||
            Compare(f1, f1 + 16, f2, f2 + 16))
            return 1;
    }

    return 0;
}
