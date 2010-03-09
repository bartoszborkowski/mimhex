#ifndef MIMHEX_SAMPLER_INL_H_
#define MIMHEX_SAMPLER_INL_H_

inline uint Sampler::RandomMove() const
{
/* returns random field from set
[kBoardSizeAligned + 1, kBoardSize * kBoardSizeAligned + kBoardSize] \
\ {i : i | kBoardSizeAligned or (i + 1) | kBoardSizeAligned} */
// TODO: change the random generator to boost
    double r = 1.0;//((double) Rand::next_rand() / ((uint(1) << 31) - 1 - 1)) * all_sum;
    uint i, j;

    for (i = 1; i <= kBoardSize; ++i)
        if (r < row_sums[i])
            break;
        else
            r -= row_sums[i];

    i *= kBoardSizeAligned;

    for (j = 1; j <= kBoardSize; ++j)
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

#endif
