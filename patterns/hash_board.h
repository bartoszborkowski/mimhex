#ifndef MIMHEX_HASH_BOARD_H_
#define MIMHEX_HASH_BOARD_H_


#include "template.h"
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

            Hash GetHash(const Hex::Location &loc) const;
            Hash GetHash(uint position) const;
            Hash GetHash(const Hex::Move &move) const;

            const Hash * GetAllHash() const;

            void Play(const Hex::Location &loc, const Hex::Player &player);
            void Play(uint position, const Hex::Player &player);
            void Play(const Hex::Move &move);

        private:
            uint pattern_count[kFieldsAlignedAmount];
            uint patterns_at[kFieldsAlignedAmount][MAX_PATTERNS];
            Hash position_hash[kFieldsAlignedAmount];
    };

    #include "hash_board-inl.h"
} // namespace HexPatterns

#endif
