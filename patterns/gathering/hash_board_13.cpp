/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                               27th March 2010                               *
 *******************************************************************************/

#include <fstream>
#include <sstream>

#include "hash_board_13.h"

#define GATHER_SIZE 13

namespace HexPatterns
{
    const HashBoard_13 HashBoard_13::EmptyHashBoard()
    {
        /* To anyone that will be reading the following code:                  *
         * I am very, very sorry.                                              *
         */
        HashBoard_13 board;
        uint height = (GATHER_SIZE + 1) * Hex::kBoardSizeAligned + 1;
        uint pos;

        rep(ii, Hex::kFieldsAlignedAmount) {
            board.pattern_count[ii] = 0;
            board.position_hash[ii] = 0;
        }

        /* Guardians' initialisation */
        rep(ii, GATHER_SIZE) {
            board.pattern_count[ii + 1] =
                board.pattern_count[(ii + 1) * Hex::kBoardSizeAligned] =
                board.pattern_count[(ii + 2) * Hex::kBoardSizeAligned - 2] =
                board.pattern_count[height + ii] = 4;

            pos = ii + 1;
            board.patterns_at[pos][0] = pos - 1;
            board.patterns_at[pos][1] = pos + 1;
            board.patterns_at[pos][2] = pos + Hex::kBoardSizeAligned - 1;
            board.patterns_at[pos][3] = pos + Hex::kBoardSizeAligned;

            pos = (ii + 1) * Hex::kBoardSizeAligned;
            board.patterns_at[pos][0] = pos - Hex::kBoardSizeAligned;
            board.patterns_at[pos][1] = pos - Hex::kBoardSizeAligned + 1;
            board.patterns_at[pos][2] = pos + 1;
            board.patterns_at[pos][3] = pos + Hex::kBoardSizeAligned;

            pos = (ii + 2) * Hex::kBoardSizeAligned - 2;
            board.patterns_at[pos][0] = pos - Hex::kBoardSizeAligned;
            board.patterns_at[pos][1] = pos - 1;
            board.patterns_at[pos][2] = pos + Hex::kBoardSizeAligned - 1;
            board.patterns_at[pos][3] = pos + Hex::kBoardSizeAligned;

            pos = height + ii;
            board.patterns_at[pos][0] = pos - Hex::kBoardSizeAligned;
            board.patterns_at[pos][1] = pos - Hex::kBoardSizeAligned + 1;
            board.patterns_at[pos][2] = pos - 1;
            board.patterns_at[pos][3] = pos + 1;
        }
        board.pattern_count[0] = 2;
        board.patterns_at[0][0] = 1;
        board.patterns_at[0][1] = Hex::kBoardSizeAligned;

        board.pattern_count[Hex::kBoardSizeAligned - 2] = 3;
        board.patterns_at[Hex::kBoardSizeAligned - 2][0] = Hex::kBoardSizeAligned - 3;
        board.patterns_at[Hex::kBoardSizeAligned - 2][1] = 2 * Hex::kBoardSizeAligned - 3;
        board.patterns_at[Hex::kBoardSizeAligned - 2][2] = 2 * Hex::kBoardSizeAligned - 2;

        board.pattern_count[(Hex::kBoardSizeAligned - 2) * Hex::kBoardSizeAligned] = 3;
        board.patterns_at[(Hex::kBoardSizeAligned - 2) * Hex::kBoardSizeAligned][0] =
            (Hex::kBoardSizeAligned - 3) * Hex::kBoardSizeAligned;
        board.patterns_at[(Hex::kBoardSizeAligned - 2) * Hex::kBoardSizeAligned][1] =
            (Hex::kBoardSizeAligned - 3) * Hex::kBoardSizeAligned + 1;
        board.patterns_at[(Hex::kBoardSizeAligned - 2) * Hex::kBoardSizeAligned][2] =
            (Hex::kBoardSizeAligned - 2) * Hex::kBoardSizeAligned + 1;

        board.pattern_count[(Hex::kBoardSizeAligned - 1) * Hex::kBoardSizeAligned - 2] = 2;
        board.patterns_at[(Hex::kBoardSizeAligned - 1) * Hex::kBoardSizeAligned - 2][0] =
            (Hex::kBoardSizeAligned - 2) * Hex::kBoardSizeAligned - 2;
        board.patterns_at[(Hex::kBoardSizeAligned - 1) * Hex::kBoardSizeAligned - 2][1] =
            (Hex::kBoardSizeAligned - 1) * Hex::kBoardSizeAligned - 3;
        /* Guardians' initialisation */

        /* Adding guardians to topmost and bottommost rows */
        rep(ii, Hex::kBoardSizeAligned) {
            board.Play(ii, GUARDIAN_STATE);
            board.Play(ii + height - 1, GUARDIAN_STATE);
        }
        /* Adding guardians to topmost and bottommost rows */

        /* Adding guardians to leftmost and rightmost columns */
        rep(ii, GATHER_SIZE) {
            uint left_column = (ii + 1) * Hex::kBoardSizeAligned;
            uint right_column = left_column + GATHER_SIZE + 1;
            board.Play(left_column, GUARDIAN_STATE);
            board.Play(right_column, GUARDIAN_STATE);
        }
        /* Adding guardians to two leftmost and rightmost columns */

        /* Board fields' initialisation */
        rep(ii, GATHER_SIZE)
            rep(jj, GATHER_SIZE) {
                pos = GUARDED_POSITION(ii, jj);
                board.pattern_count[pos] = 6;
                board.patterns_at[pos][0] = pos - Hex::kBoardSizeAligned;
                board.patterns_at[pos][1] = pos - Hex::kBoardSizeAligned + 1;
                board.patterns_at[pos][2] = pos - 1;
                board.patterns_at[pos][3] = pos + 1;
                board.patterns_at[pos][4] = pos + Hex::kBoardSizeAligned - 1;
                board.patterns_at[pos][5] = pos + Hex::kBoardSizeAligned;
            }
        /* Board fields' initialisation */

        return board;
    }

    HashBoard_13::HashBoard_13()
    {
        rep(ii, Hex::kFieldsAlignedAmount)
            pattern_count[ii] = 0;

        memset(position_hash, 0, Hex::kFieldsAlignedAmount * sizeof(Hash));
    }

    HashBoard_13::HashBoard_13(const HashBoard_13 &board)
    {
        memcpy(this, &board, sizeof(HashBoard_13));
    }

    HashBoard_13 & HashBoard_13::operator =(const HashBoard_13 &board)
    {
        memcpy(this, &board, sizeof(HashBoard_13));

        return *this;
    }

    std::string HashBoard_13::ToAsciiArt() const
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
