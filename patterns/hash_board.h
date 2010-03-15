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
            static const HashBoard EmptyHashBoard
                (const char *hash_board_file = "hash_data.txt");

            HashBoard();
            HashBoard(const HashBoard &board);
            HashBoard & operator =(const HashBoard &board);

            Hash GetHash(uint position) const;
            const Hash * GetAllHash() const;

            void Play(uint position, uint player);

            uint GetBoardSize() const;

            std::string ToAsciiArt() const;

        private:
            uint pattern_count[Hex::kFieldsAlignedAmount];
            uint patterns_at[Hex::kFieldsAlignedAmount][MAX_PATTERNS];
            Hash position_hash[Hex::kFieldsAlignedAmount];
    };

    #include "hash_board-inl.h"
} // namespace HexPatterns

#endif
