#define INSERT_ASSERTIONS

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <boost/lexical_cast.hpp>
#include "lib/conditional_assert.h"
#include "mm.h"
#include "lib/random.cpp"

#define BUFFER_SIZE 4096
#define UNKNOWN_HASH_GAMMA (0.0001)

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cerr << "Usage: ./gammas_calc iterations";
        return 1;
    }

    std::string iterations_str = argv[1];
    unsigned iterations = boost::lexical_cast<unsigned>(iterations_str);

    char buffer[BUFFER_SIZE];
    std::ifstream in("comp.txt", std::ifstream::in);

    MM::BtModel model;

    std::cerr << "Reading input file..." << std::endl;

    while (in.good()) {
        in.getline(buffer, BUFFER_SIZE);
        std::istringstream str(buffer);
        unsigned chosen;
        str >> chosen;
        if (!str.good())
            break;
        MM::Match& match = model.NewMatch();
        bool winnerSet = false;
        while (str.good()) {
            unsigned hash;
            str >> hash;
            MM::Team& team = match.NewTeam();
            team.SetFeatureLevel(MM::kPatternFeature, hash);
            if (hash == chosen  && !winnerSet) {
                match.SetWinnerLastTeam();
                winnerSet = true;
            }
        }
        ASSERT(winnerSet);
    }
    in.close();

    std::cerr << "Input file read. Calculating gammas..." << std::endl;

    model.PreprocessData();
    for (unsigned i = 0; i < iterations; ++i) {
        if ((i * 100) % iterations == 0)
            std::cerr << i * 100 / iterations << "%...";
        model.TrainFeature(MM::kPatternFeature);
    }

    std::cerr << std::endl << "Gammas calculated. Preparing output..." << std::endl;

    std::map<unsigned, unsigned> hash_numbers;
    std::map<unsigned, std::set<unsigned> > min_hashes;

    std::ifstream mins("min_hash.txt", std::ifstream::in);
    while (mins.good()) {
        mins.getline(buffer, BUFFER_SIZE);
        std::istringstream str(buffer);
        unsigned from;
        str >> from;
        if (!str.good())
            break;
        while (str.good()) {
            unsigned to;
            str >> to;
            min_hashes[from].insert(to);
        }
    }
    mins.close();

    std::ifstream numbers("pattern_numb.txt", std::ifstream::in);
    while (numbers.good()) {
        unsigned from, to;
        numbers >> from >> to;
        if (!numbers.good())
            break;
        hash_numbers[to] = from;
    }
    numbers.close();

    std::ofstream gammas_out("pattern_gammas.txt", std::ifstream::out);
    std::map<unsigned, std::set<unsigned> >::const_iterator min_hashes_it;
    for (min_hashes_it = min_hashes.begin();
            min_hashes_it != min_hashes.end(); ++min_hashes_it) {

        double gamma;
        if (hash_numbers.count(min_hashes_it->first) > 0) {
            unsigned hash_number = hash_numbers[min_hashes_it->first];
            gamma = model.gammas.Get(MM::kPatternFeature, hash_number);
        } else {
            gamma = UNKNOWN_HASH_GAMMA;
        }

        std::set<unsigned>::const_iterator it;

        for (it = min_hashes_it->second.begin();
                it != min_hashes_it->second.end(); ++it) {

            gammas_out << *it << " " << gamma << std::endl;
        }
    }
    gammas_out.close();

    std::cerr << "Output prepared. Nothing more to do." << std::endl;

    return 0;
}
