#ifndef MIMHEX_SAMPLER_H_
#define MIMHEX_SAMPLER_H_

#include <string>

#include "board.h"
#include "hash_board.h"

typedef unsigned char uchar;

namespace Hex
{
    class Sampler {
        public:
            Sampler();

            std::string ToAsciiArt() const;

            void Play(const Move &move);
            uint RandomMove() const;

            double GetSum() const;

            static bool use_patterns;

        private:
            double gammas[kFieldsAlignedAmount];
            double row_sums[kBoardSizeAligned];
            double all_sum;
            uchar used_fields[kFieldsAlignedAmount];

            HexPatterns::HashBoard hash_board;
    };

    #include "sampler-inl.h"
} // namespace Hex

#endif
