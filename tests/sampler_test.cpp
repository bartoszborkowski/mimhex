/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#define INSERT_ASSERTIONS

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

bool Valid(uint position)
{
    uint row = position >> 4;
    uint column = position & (Hex::kBoardSizeAligned - 1);

    return (row > 1) && (row < Hex::kBoardSize + 2) &&
        (column > 1) && (column < Hex::kBoardSize + 2);
}

int main(int argc, char *argv[])
/*
 * A test program that checks whether the sampler works correctly.
 */
{
    Hex::Sampler sampler;
    uint loops = 1000000;
    uint pos;
    Hex::Player player(Hex::Player::First());
    std::map<HexPatterns::Hash, uint> choices, occurences;
    std::vector<HexPatterns::Hash> xors, aux, possible;

    if (argc == 2)
        try {
            loops = boost::lexical_cast<uint>(argv[1]);
        } catch (boost::bad_lexical_cast &) {
            std::cerr << "Error: " << argv[1] << " is not an integer" << std::endl;
            exit(1);
        }

    rep(ii, TEMPLATES_AMOUNT) {
        aux = HexPatterns::templates[ii].GetAllHashes();
        xors.insert(xors.begin(), aux.begin(), aux.end());
    }

    rep(ii, xors.size()) {
        choices.insert(pair<HexPatterns::Hash, uint>(xors[ii], 0));
        occurences.insert(pair<HexPatterns::Hash, uint>(xors[ii], 0));
    }

    rep(ii, loops) {
        player = Hex::Player::First();
        sampler = Hex::Sampler();

        rep(jj, Hex::kBoardSize * Hex::kBoardSize) {
            possible.clear();

            pos = sampler.RandomMove();
            sampler.GetPlayableHashes(possible);

            ASSERT(Valid(pos));

           ++choices[sampler.GetHash(pos)];
            rep(kk, possible.size())
                ++occurences[possible[kk]];

            sampler.Play(Hex::Move(player, Hex::Location(pos)));
            player = player.Opponent();
        }
    }

    for(std::map<HexPatterns::Hash, uint>::iterator it = choices.begin();
        it != choices.end(); ++it)
        if (it->second)
            std::cout << it->first << "\t\t"
                        << static_cast<double>(it->second) / occurences[it->first]
                        << "\t\t"
                        << Hex::PatternData::GetStrength(it->first)
                        << std::endl;

    return 0;
}
