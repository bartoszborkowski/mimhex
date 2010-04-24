/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#ifndef MIMHEX_SAMPLER_INL_H_
#define MIMHEX_SAMPLER_INL_H_

inline uint Sampler::RandomMove() const
{
    double r = SamplerRandom::random_generator() * all_sum;
    uint i, j;

    for (i = 2; i <= Dim::board_size; ++i)
        if (r < row_sums[i])
            break;
        else
            r -= row_sums[i];

    i *= Dim::actual_size;

    for (j = 2; j <= Dim::board_size; ++j)
        if (r < gammas[i + j])
            break;
        else
            r -= gammas[i + j];

    return i + j;
}

inline double Sampler::GetSum() const
{
    return all_sum;
}

inline HexPatterns::Hash Sampler::GetHash(uint position) const
{
    return hash_board.GetHash(position);
}

inline void Sampler::GetPlayableHashes(std::vector<HexPatterns::Hash> &out) const
{
    const HexPatterns::Hash *hashes = hash_board.GetAllHashes();

    rep(ii, Dim::board_size)
        rep(jj, Dim::board_size) {
            uint kk = NORMALISED_POSITION(ii, jj);
            if (used_fields[kk])
                out.push_back(hashes[kk]);
        }

    return;
}

#endif
