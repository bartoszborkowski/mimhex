/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_MACRO_DEFINITIONS_H_
#define MIMHEX_MACRO_DEFINITIONS_H_

#include "board_dim.h"
#include "rep.h"

namespace HexPatterns
{

    #define rep_bound(iterator, lower_bound, upper_bound)                           \
        for (uint iterator = lower_bound; iterator < upper_bound; ++iterator)

    #define GUARDED_POSITION(row, column)                                           \
        ((row + 1) * Hex::Dim::actual_size + (column + 1))

    #define NORMALISED_POSITION(row, column)                                        \
        ((row + 2) * Hex::Dim::actual_size + (column + 2))

    #define TEMPLATES_AMOUNT        1

    #define PATTERNS_AMOUNT         (Hex::Dim::actual_field_count)

    #define PATTERN_DATA_FILE       "pattern_strength.data"

    #define MAX_PATTERNS            8
    #define DIMENSIONS              2
    #define FIELD_STATES            4
    #define FIELD_STATES_SIZE       (FIELD_STATES * sizeof(uint))

    #define PLAYER_0_STATE          0
    #define PLAYER_1_STATE          1
    #define GUARDIAN_STATE          2
    #define EMPTY_STATE             3

    typedef uint Hash;
    typedef unsigned long ulong;

    const uint kTemplateHashMemory =
        (2 * Hex::Dim::actual_size - 1) * (2 * Hex::Dim::actual_size - 1) *
        FIELD_STATES * sizeof(Hash);
    const uint kPatternHashMemory =
        Hex::Dim::actual_field_count * FIELD_STATES * sizeof(Hash);
} // namespace HexPatterns

#endif
