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
    double r = random_generator() * all_sum;
    uint i, j;

    for (i = 2; i <= kBoardSize; ++i)
        if (r < row_sums[i])
            break;
        else
            r -= row_sums[i];

    i *= kBoardSizeAligned;

    for (j = 2; j <= kBoardSize; ++j)
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
