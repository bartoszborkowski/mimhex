#include <sstream>

#include "sampler.h"

namespace Hex
{
    bool Sampler::use_patterns = false;

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
        uint row = position & ((kBoardSizeAligned << 1) - 1);
        /* CAUTION!
         * The above assumes that kBoardSizeAligned is a power of 2.
         * Only then it is equivalent to:
         * uint row = position % kBoardSizeAligned;
         */

        hash_board.Play(position, player);

        used_fields[position] = 0;
        all_sum -= row_sums[row - 1] + row_sums[row] + row_sums[row + 1];

        /* TODO: change this so it accepts patterns of different shapes */

        /* Decreasing row sums */
        row_sums[row - 1] -=
            gammas[position - kBoardSizeAligned + 1] +
            gammas[position - kBoardSizeAligned];
        row_sums[row] -=
            gammas[position + 1] + gammas[position] + gammas[position - 1];
        row_sums[row + 1] -=
            gammas[position + kBoardSizeAligned] +
            gammas[position + kBoardSizeAligned - 1];
        /* Decreasing row sums */

        /* TODO: implement get_strength */
        /* Changing gammas */
        gammas[position - kBoardSizeAligned + 1] =
            get_strength(hash_board.GetHash(position - kBoardSizeAligned + 1)) *
            used_fields[position - kBoardSizeAligned + 1];
        gammas[position + 1] =
            get_strength(hash_board.GetHash(position + 1)) *
            used_fields[position + 1];
        gammas[position + kBoardSizeAligned] =
            get_strength(hash_board.GetHash(position + kBoardSizeAligned)) *
            used_fields[position + kBoardSizeAligned];
        gammas[position] = 0.0;
        gammas[position + kBoardSizeAligned - 1] =
            get_strength(hash_board.GetHash(position + kBoardSizeAligned - 1)) *
            used_fields[position + kBoardSizeAligned - 1];
        gammas[position - 1] =
            get_strength(hash_board.GetHash(position - 1)) *
            used_fields[position - 1];
        gammas[position - kBoardSizeAligned] =
            get_strength(hash_board.GetHash(position - kBoardSizeAligned)) *
            used_fields[position - kBoardSizeAligned];
        /* Changing gammas */

        /* Zeroing out-of-bounds gammas */
        gammas[(row - 1) * kBoardSizeAligned] = gammas[row * kBoardSizeAligned - 1] =
            gammas[row * kBoardSizeAligned] = gammas[(row + 1) * kBoardSizeAligned - 1] =
            gammas[(row + 1) * kBoardSizeAligned] = gammas[(row + 2) * kBoardSizeAligned - 1] = 0.0f;
        /* Zeroing out-of-bounds gammas */

        /* Increasing row sums */
        row_sums[row - 1] +=
            gammas[position - kBoardSizeAligned + 1] +
            gammas[position - kBoardSizeAligned];
        row_sums[row] +=
            gammas[position + 1] + gammas[position - 1];
        row_sums[row + 1] +=
            gammas[position + kBoardSizeAligned ] +
            gammas[position + kBoardSizeAligned - 1];

        /* Amending double's lack of precision */
        /* TODO: change 0.0 to min_gamma (the minimal present value of gamma) */
        if (row_sums[row - 1] < 0.0)
            row_sums[row - 1] = 0.0;
        if (row_sums[row] < 0.0)
            row_sums[row] = 0.0;
        if (row_sums[row + 1] < 0.0)
            row_sums[row + 1] = 0.0;
        /* Increasing row sums */

        /* Zeroing out-of-bounds row sums */
        row_sums[0] = row_sums[kBoardSizeAligned - 1] = 0.0;
        /* Zeroing out-of-bounds row sums */

        all_sum += row_sums[row - 1] + row_sums[row] + row_sums[row + 1];

        return;
    }
} // namespace Hex
