#ifndef MIMHEX_MACRO_DEFINITIONS_H_
#define MIMHEX_MACRO_DEFINITIONS_H_

#include "board.h"

namespace HexPatterns
{
    #define rep(iterator, upper_bound)                                              \
        for (uint iterator = 0; iterator < upper_bound; ++iterator)

    #define rep2(iterator, lower_bound, upper_bound)                                \
        for (uint iterator = lower_bound; iterator < upper_bound; ++iterator)

    #define NORMALISE_POSITION(row, column)                                         \
        ((row + 2) * Hex::kBoardSizeAligned + (column + 2))

    #define TEMPLATES_AMOUNT        1
    #define TEMPLATES_FILE          "template_data.txt"

    #define PATTERNS_AMOUNT         (Hex::kFieldsAlignedAmount)
    #define PATTERNS_FILE           "pattern_data.txt"

    #define MAX_PATTERNS            8

    #define FIELD_STATES            4

    typedef uint Hash;

    const uint kHashMemory = Hex::kFieldsAlignedAmount * FIELD_STATES * sizeof(Hash);
} // namespace HexPatterns

#endif
