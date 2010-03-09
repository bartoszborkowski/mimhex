/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <inttypes.h>
#include <iostream>
#include <time.h>

#include <boost/random/linear_congruential.hpp>
#include <boost/lexical_cast.hpp>

#include "board.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "macro_definitions.h"

int main(int argc, char *argv[])
{
    uint states = FIELD_STATES;
    uint fields = 9;
    boost::rand48 random_generator;

    if (argc == 2)
        try {
            fields = boost::lexical_cast<uint>(argv[1]);
        } catch (boost::bad_lexical_cast &) {
            std::cerr << "Error: " << argv[1] << " is not a number" << std::endl;
            exit(1);
        }

    random_generator.seed(static_cast<int32_t>(time(0)));

    rep(ii, fields) {
        rep(jj, states)
            std::cout << random_generator() << " ";
        std::cout << std::endl;
    }

    return 0;
}
