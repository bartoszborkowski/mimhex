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
#include "conditional_assert.h"

namespace Hex
{
    boost::rand48 SamplerRandom::base_generator(static_cast<int32_t>(time(0)));
    boost::uniform_01<boost::rand48>
        SamplerRandom::random_generator(base_generator);

    Sampler::Sampler() : hash_board(HexPatterns::HashBoard::EmptyHashBoard())
    {
        uint kk;

        all_sum = 0.0;

        memset(gammas, 0, Dim::actual_field_count * sizeof(double));
        memset(row_sums, 0, Dim::actual_size * sizeof(double));
        memset(used_fields, 0, Dim::actual_field_count * sizeof(uchar));

        for (uint ii = 0; ii < Dim::board_size; ++ii)
            for (uint jj = 0; jj < Dim::board_size; ++jj) {
                kk = (ii + 2) * Dim::actual_size + (jj + 2);
                used_fields[kk] = 1;
                gammas[kk] = PatternData::GetStrength(hash_board.GetHash(kk));
                row_sums[ii + 2] += gammas[kk];
                all_sum += gammas[kk];
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

        for (uint ii = 0; ii < Dim::actual_size; ++ii) {
            for (uint jj = 0; jj < Dim::actual_size; ++jj)
                ret << gammas[ii * Dim::actual_size + jj] << " ";
            ret << "sum = " << row_sums[ii] << std::endl;
        }

        ret << "all_sum = " << all_sum << std::endl;

        return ret.str();
    }

    void Sampler::Play(const Move &move)
    {
        uint position = move.GetLocation().GetPos();
        uint player = move.GetPlayer().GetVal();

        uint changed_positions_amount;
        const uint *changed_positions;

        hash_board.Play(position, player);

        changed_positions_amount = hash_board.GetChangedPositionsAmount(position);
        changed_positions = hash_board.GetChangedPositions(position);

        ASSERT(used_fields[position]);

        /* Removing chosen field from the sampler.                              */
        row_sums[position >> 4] -= gammas[position];
        all_sum -= gammas[position];
        used_fields[position] = 0;
        gammas[position] = 0.0;
        /* Removing chosen field from the sampler.                              */
        /* Amending double's lack of precision.                             */
        if (row_sums[position >> 4] < PatternData::GetThreshold())
            row_sums[position >> 4] = 0.0;
        /* Amending double's lack of precision.                             */

        for (uint i = 0; i < changed_positions_amount; ++i) {
            row_sums[changed_positions[i] >> 4] -= gammas[changed_positions[i]];
            all_sum -= gammas[changed_positions[i]];

            /* NOTE: Out of bounds gammas are zeroed by used_fields values.     */
            gammas[changed_positions[i]] =
                PatternData::GetStrength(hash_board.GetHash(changed_positions[i])) *
                used_fields[changed_positions[i]];

            row_sums[changed_positions[i] >> 4] += gammas[changed_positions[i]];
            all_sum += gammas[changed_positions[i]];

            /* Amending double's lack of precision.                             */
            if (row_sums[changed_positions[i] >> 4] < PatternData::GetThreshold())
                row_sums[changed_positions[i] >> 4] = 0.0;
            /* Amending double's lack of precision.                             */
        }

        return;
    }
} // namespace Hex
