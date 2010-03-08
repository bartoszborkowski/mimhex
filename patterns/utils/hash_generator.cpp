#include <inttypes.h>
#include <iostream>
#include <time.h>

#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>

#include "macro_definitions.h"

int main(int argc, char *argv[])
{
    uint states = FIELD_STATES;
    uint fields = 9;
    boost::rand48 generator;

    if (argc == 2)
        try {
            fields = boost::lexical_cast<uint>(argv[1]);
        } catch (boost::bad_lexical_cast &) {
            std::cerr << "Error: " << argv[1] << " is not a number" << std::endl;
            exit(1);
        }

    generator.seed(static_cast<int32_t>(time(0)));

    rep(ii, fields) {
        rep(jj, states)
            std::cout << generator() << " ";
        std::cout << std::endl;
    }

    return 0;
}
