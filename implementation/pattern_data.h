/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#ifndef MIMHEX_PATTERN_DATA_H_
#define MIMHEX_PATTERN_DATA_H_

#include <map>
#include <string>

#include "macro_definitions.h"

namespace Hex
{
    class PatternData
    {
        public:
            static std::map<HexPatterns::Hash, double> InitialisePatternData
                (const char *pattern_file = PATTERN_DATA_FILE);

            static double GetStrength(HexPatterns::Hash hash);

            static std::string ToAsciiArt();

        private:
            PatternData();
            PatternData(const PatternData &data);
            PatternData & operator =(const PatternData &data);

            static std::map<HexPatterns::Hash, double> strengths;
    };
} // namespace Hex

#endif
