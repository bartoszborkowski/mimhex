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

typedef unsigned char uchar;

namespace Hex
{
    class Sampler {
        public:
            static uint InitialiseSampler();

            Sampler();
            Sampler(const Sampler &sampler);
            Sampler & operator =(const Sampler &sampler);

            std::string ToAsciiArt() const;

            void Play(const Move &move);
            uint RandomMove() const;

            double GetSum() const;

            static bool use_patterns;
            static boost::rand48 base_generator;
            static boost::uniform_01<boost::rand48> random_generator;

        private:
            double gammas[kFieldsAlignedAmount];
            double row_sums[kBoardSizeAligned];
            double all_sum;
            uchar used_fields[kFieldsAlignedAmount];

            HexPatterns::HashBoard hash_board;
    };

    uint __sampler_dummy = Sampler::InitialiseSampler();

    #include "sampler-inl.h"
} // namespace Hex

#endif
