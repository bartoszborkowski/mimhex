/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_HASH_BOARD_H_
#define MIMHEX_HASH_BOARD_H_

#include <string>

#include "template.h"
#include "pattern.h"
#include "macro_definitions.h"

namespace HexPatterns
{
    class HashBoard
    {
        public:
            static const HashBoard EmptyHashBoard();

            HashBoard();
            HashBoard(const HashBoard &board);
            HashBoard & operator =(const HashBoard &board);

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

    #include "hash_board-inl.h"
} // namespace HexPatterns

#endif
