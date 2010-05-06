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
    double PatternData::strengths[];
    double PatternData::threshold = 10000.0;

    uint PatternData::InitialisePatternData(const char *pattern_file)
    {
        std::ifstream ifs(pattern_file, std::ifstream::in);
        HexPatterns::Hash hash;
        double strength;

        if (!ifs.good()) {
            std::cerr << "Error: could not open file \'" << pattern_file
                        << "\'" << std::endl;
            exit(1);
        }

        memset(strengths, 0, PATT_MASK * sizeof(double));

        while(ifs.good()) {
            ifs >> hash >> strength;
            strengths[hash & PATT_MASK] = strength;
            if (strength < threshold)
                threshold = strength;
        }

        threshold /= 10.0;

        ifs.close();

        return 1;
    }

    inline double PatternData::GetStrength(HexPatterns::Hash hash)
    {
        return PatternData::strengths[hash & PATT_MASK];
    }

    inline double PatternData::GetThreshold()
    {
        return threshold;
    }

    std::string PatternData::ToAsciiArt()
    {
        std::stringstream ret;

        rep(ii, PATT_MASK)
            ret << ii << " " << PatternData::strengths[ii] << std::endl;

        return ret.str();
    }
} // namespace Hex
