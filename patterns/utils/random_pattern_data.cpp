/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include <inttypes.h>
#include <time.h>

#include <boost/random/uniform_01.hpp>
#include <boost/random/linear_congruential.hpp>

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

int main()
/*
 * A program that creates a pattern data file filled with random values.
 * Strengths are normalised, so their sum gives a value of 1.0.
 */
{
    boost::rand48 base_generator;
    boost::uniform_01<boost::rand48> random_generator(random_generator);
    std::vector<HexPatterns::Hash> xors, aux;
    std::vector<double> strengths;
    double sum = 0.0;
    ofstream ofs("random_pattern.data", std::ios_base::trunc);

    base_generator.seed(static_cast<int32_t>(time(0)));
    random_generator = boost::uniform_01<boost::rand48>(base_generator);

    rep(ii, TEMPLATES_AMOUNT) {
        aux = HexPatterns::Template::templates[ii].GetAllHashes();
        xors.insert(xors.begin(), aux.begin(), aux.end());
    }

    rep(ii, xors.size()) {
        strengths.push_back(random_generator());
        sum += strengths[ii];
    }

    rep(ii, xors.size())
        ofs << xors[ii] << " " << strengths[ii] / sum << std::endl;

    ofs.close();

    return 0;
}
