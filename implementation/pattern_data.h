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

#define PATT_MASK 0xFFFFF

namespace Hex
{
    class PatternData
    {
        public:
            static uint InitialisePatternData
                (const char *pattern_file = PATTERN_DATA_FILE);

            static double GetStrength(HexPatterns::Hash hash);

            static double GetThreshold();

            static std::string ToAsciiArt();

        private:
            PatternData();
            PatternData(const PatternData &data);
            PatternData & operator =(const PatternData &data);

            static double strengths[PATT_MASK];
            static double threshold;
    };
} // namespace Hex

uint __dummy_pattern_data = Hex::PatternData::InitialisePatternData();

#endif
