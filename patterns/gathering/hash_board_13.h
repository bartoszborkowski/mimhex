/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                               27th March 2010                               *
 *******************************************************************************/

#ifndef MIMHEX_HASH_BOARD_13_H_
#define MIMHEX_HASH_BOARD_13_H_

#include <string>

#include "template.h"
#include "pattern.h"
#include "macro_definitions.h"

#define GATHER_SIZE 13
#define GUARDS_SIZE 1
#define FULL_SIZE (GATHER_SIZE + 2*GUARDS_SIZE)

namespace HexPatterns
{
    class HashBoard_13
    {
        public:
            static const HashBoard_13 EmptyHashBoard();
            static uint GetLocation(string position);

            HashBoard_13();
            HashBoard_13(const HashBoard_13 &board);
            HashBoard_13 & operator =(const HashBoard_13 &board);

            Hash GetHash(uint position) const;
            const Hash * GetAllHashes() const;

            void Play(uint position, uint player);

            uint GetChangedPositionsAmount(uint position) const;
            const uint * GetChangedPositions(uint position) const;

            uint GetBoardSize() const;

            std::string ToAsciiArt() const;

        private:
            uint pattern_count[Hex::Dim::actual_field_count];
            uint patterns_at[Hex::Dim::actual_field_count][MAX_PATTERNS];
            Hash position_hash[Hex::Dim::actual_field_count];
    };

    #include "hash_board-inl_13.h"
} // namespace HexPatterns

#endif
