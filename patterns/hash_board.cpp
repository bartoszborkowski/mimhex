#include <fstream>

#include "hash_board.h"

namespace HexPatterns
{
    const HashBoard HashBoard::EmptyHashBoard(const char */*hash_board_file*/)
    {
        // TODO: read from file
        //std::ifstream ifs(hash_board_file, std::ifstream::in);

        HashBoard board;

        rep(ii, kFieldsAlignedAmount)
            board.pattern_count[ii] = 0;

        rep(ii, Hex::kBoardSize)
            rep(jj, Hex::kBoardSize) {
                uint kk = NORMALISE_POSITION(ii, jj);
                board.pattern_count[kk] = 6;
                board.patterns_at[kk][0] = kk - Hex::kBoardSizeAligned;
                board.patterns_at[kk][1] = kk - Hex::kBoardSizeAligned + 1;
                board.patterns_at[kk][2] = kk - 1;
                board.patterns_at[kk][3] = kk + 1;
                board.patterns_at[kk][4] = kk + Hex::kBoardSizeAligned - 1;
                board.patterns_at[kk][5] = kk + Hex::kBoardSizeAligned;
                board.position_hash[kk] = 0;
            }

        return board;
    }

    HashBoard::HashBoard()
    {
        rep(ii, kFieldsAlignedAmount)
            pattern_count[ii] = 0;
    }

    HashBoard::HashBoard(const HashBoard &board)
    {
        memcpy(this, &board, sizeof(HashBoard));
    }

    HashBoard & HashBoard::operator =(const HashBoard &board)
    {
        memcpy(this, &board, sizeof(HashBoard));

        return *this;
    }
} // namespace HexPatterns
