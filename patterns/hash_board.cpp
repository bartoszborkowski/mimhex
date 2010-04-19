/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <fstream>
#include <sstream>

#include "hash_board.h"

namespace HexPatterns
{
    const HashBoard HashBoard::EmptyHashBoard()
    {
        HashBoard board;

        rep(ii, Hex::kFieldsAlignedAmount) {
            board.pattern_count[ii] = 0;
            board.position_hash[ii] = 0;
        }

        rep(ii, Hex::kBoardSize + 2)
            rep(jj, Hex::kBoardSize + 2) {
                uint kk = GUARDED_POSITION(ii, jj);
                board.pattern_count[kk] = 6;
                board.patterns_at[kk][0] = kk - Hex::kBoardSizeAligned;
                board.patterns_at[kk][1] = kk - Hex::kBoardSizeAligned + 1;
                board.patterns_at[kk][2] = kk - 1;
                board.patterns_at[kk][3] = kk + 1;
                board.patterns_at[kk][4] = kk + Hex::kBoardSizeAligned - 1;
                board.patterns_at[kk][5] = kk + Hex::kBoardSizeAligned;
            }

        /* Adding guardians to two topmost and bottommost rows */
        uint board_height = Hex::kBoardSizeAligned * (Hex::kBoardSize + 2);
        rep(ii, Hex::kBoardSizeAligned) {
            uint second_row = ii + Hex::kBoardSizeAligned;
            board.Play(ii, GUARDIAN_STATE);
            board.Play(second_row, GUARDIAN_STATE);
            board.Play(ii + board_height, GUARDIAN_STATE);
            board.Play(second_row + board_height, GUARDIAN_STATE);
        }
        /* Adding guardians to two topmost and bottommost rows */

        /* Adding guardians to two leftmost and rightmost columns */
        rep(ii, Hex::kBoardSize) {
            uint left_column = (ii + 2) * Hex::kBoardSizeAligned;
            uint right_column = left_column + Hex::kBoardSize + 2;
            board.Play(left_column, GUARDIAN_STATE);
            board.Play(left_column + 1, GUARDIAN_STATE);
            board.Play(right_column, GUARDIAN_STATE);
            board.Play(right_column + 1, GUARDIAN_STATE);
        }
        /* Adding guardians to two leftmost and rightmost columns */

        return board;
    }

    HashBoard::HashBoard()
    {
        rep(ii, Hex::kFieldsAlignedAmount)
            pattern_count[ii] = 0;

        memset(position_hash, 0, Hex::kFieldsAlignedAmount * sizeof(Hash));
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

    std::string HashBoard::ToAsciiArt() const
    {
        std::stringstream ret;

        rep(ii, Hex::kBoardSize) {
            rep(jj, Hex::kBoardSize)
                ret << position_hash[NORMALISED_POSITION(ii, jj)] << " ";
            ret << std::endl;
        }

        return ret.str();
    }

} // namespace HexPatterns
