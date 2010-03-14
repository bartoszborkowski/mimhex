/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#ifndef MIMHEX_SAMPLER_H_
#define MIMHEX_SAMPLER_H_

#include <string>

#include <boost/random/uniform_01.hpp>
#include <boost/random/linear_congruential.hpp>

#include "board.h"
#include "hash_board.h"
#include "pattern_data.h"

typedef unsigned char uchar;

namespace Hex
{
    class SamplerRandom
    {
        public:
            static void InitialiseSamplerRandom();

            static boost::rand48 base_generator;
            static boost::uniform_01<boost::rand48> random_generator;
        private:
            SamplerRandom();
            SamplerRandom(const SamplerRandom &);
            SamplerRandom & operator =(const SamplerRandom &);
    };

    class Sampler
    {
        public:
            Sampler();
            Sampler(const Sampler &sampler);
            Sampler & operator =(const Sampler &sampler);

            void Play(const Move &move);
            uint RandomMove() const;

            double GetSum() const;
            HexPatterns::Hash GetHash(uint position) const;

            std::string ToAsciiArt() const;

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
