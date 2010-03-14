/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#include <sstream>
#include <inttypes.h>
#include <time.h>

#include "sampler.h"

namespace Hex
{
    bool Sampler::use_patterns = true;
    boost::rand48 Sampler::base_generator;
    boost::uniform_01<boost::rand48> Sampler::random_generator(base_generator);

    uint Sampler::InitialiseSampler()
    {
        base_generator.seed(static_cast<int32_t>(time(0)));
        random_generator = boost::uniform_01<boost::rand48>(base_generator);

        return 0;
    }

    Sampler::Sampler()
    {
        hash_board = HexPatterns::HashBoard::EmptyHashBoard();

        all_sum = 0.0;

        memset(gammas, 0, kBoardSizeAligned * kBoardSizeAligned * sizeof(double));
        memset(row_sums, 0, kBoardSizeAligned * sizeof(double));

        for (uint ii = 0; ii < kBoardSizeAligned * kBoardSizeAligned; ++ii) {
            used_fields[ii] = 1;
            //TODO: implement get_strength
            //gammas[ii] = get_strength[hash_board[ii]];
        }
    }

    Sampler::Sampler(const Sampler &sampler)
    {
        memcpy(this, &sampler, sizeof(Sampler));
    }

    Sampler & Sampler::operator =(const Sampler &sampler)
    {
        memcpy(this, &sampler, sizeof(Sampler));

        return *this;
    }

    std::string Sampler::ToAsciiArt() const
    {
        std::stringstream ret;

        ret.precision(4);
        ret.width(6);

        for (uint ii = 0; ii < kBoardSize; ++ii) {
            for (uint jj = 0; jj < kBoardSize; ++jj)
                ret << gammas[(ii + 2) * kBoardSizeAligned + (jj + 2)] << " ";
            ret << "sum = " << row_sums[ii] << std::endl;
        }

        ret << "all_sum = " << all_sum << std::endl;

        return ret.str();
    }

    /* TODO: implement the following elsewhere */
    double get_strength(uint) {return 1.0;}

    void Sampler::Play(const Move &move)
    {
        uint position = move.GetLocation().GetPos();
        uint player = move.GetPlayer().GetVal();
        uint modulo = (kBoardSizeAligned << 1) - 1;
        /* CAUTION!
         * The above assumes that kBoardSizeAligned is a power of 2.
         */
        uint changed_positions_amount;
        const uint *changed_positions;

        hash_board.Play(position, player);
        changed_positions_amount = hash_board.GetChangedPositionsAmount(position);
        changed_positions = hash_board.GetChangedPositions(position);

        used_fields[position] = 0;

        for (uint i = 0; i < changed_positions_amount; ++i) {
            row_sums[changed_positions[i] & modulo] -= gammas[changed_positions[i]];
            all_sum -= gammas[changed_positions[i]];

        /* TODO: implement get_strength */
        /* TODO: make it so that out-of-bounds values are 0; implement it in patterns */
            gammas[changed_positions[i]] =
                get_strength(hash_board.GetHash(changed_positions[i])) *
                used_fields[changed_positions[i]];

            row_sums[changed_positions[i] & modulo] += gammas[changed_positions[i]];
            all_sum += gammas[changed_positions[i]];

            /* Amending double's lack of precision.                             */
            /* TODO: implement min_gamma; mayhaps a minimapl present gamma decreased tenfold */
            //if (row_sums[changed_positions[i] & modulo < min_gamma)
                //row_sums[changed_positions[i] & modulo = 0.0;
            /* Amending double's lack of precision.                             */
        }

        /* Zeroing out-of-bounds gammas
        gammas[(row - 1) * kBoardSizeAligned] = gammas[row * kBoardSizeAligned - 1] =
            gammas[row * kBoardSizeAligned] = gammas[(row + 1) * kBoardSizeAligned - 1] =
            gammas[(row + 1) * kBoardSizeAligned] = gammas[(row + 2) * kBoardSizeAligned - 1] = 0.0f;
         Zeroing out-of-bounds gammas */

        /* Zeroing out-of-bounds row sums */
        row_sums[0] = row_sums[1] = row_sums[kBoardSizeAligned - 2] =
            row_sums[kBoardSizeAligned - 3] = 0.0;
        /* Zeroing out-of-bounds row sums */

        return;
    }
} // namespace Hex
