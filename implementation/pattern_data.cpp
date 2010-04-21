/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#include <fstream>
#include <iostream>
#include <sstream>

#include "conditional_assert.h"
#include "pattern_data.h"

namespace Hex
{
    std::map<HexPatterns::Hash, double> PatternData::strengths =
        PatternData::InitialisePatternData();

    std::map<HexPatterns::Hash, double>
        PatternData::InitialisePatternData(const char *pattern_file)
    {
        std::ifstream ifs(pattern_file, std::ifstream::in);
        std::map<HexPatterns::Hash, double> ret;
        HexPatterns::Hash hash;
        double strength;

        if (!ifs.good()) {
            std::cerr << "Error: could not open file \'" << pattern_file
                        << "\'" << std::endl;
            exit(1);
        }

        while(ifs.good()) {
            ifs >> hash >> strength;
            ret.insert(pair<HexPatterns::Hash, double>(hash, strength));
        }

        ifs.close();

        return ret;
    }

    double PatternData::GetStrength(HexPatterns::Hash hash)
    {
        std::map<HexPatterns::Hash, double>::iterator it = strengths.find(hash);

        ASSERT(it != PatternData::strengths.end());

        return it->second;
    }

    std::string PatternData::ToAsciiArt()
    {
        std::stringstream ret;

        for(std::map<HexPatterns::Hash, double>::const_iterator i =
            strengths.begin(); i != strengths.end(); ++i)
            ret << i->first << " " << i->second << std::endl;

        return ret.str();
    }
} // namespace Hex
