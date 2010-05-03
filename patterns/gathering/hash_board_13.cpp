/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                               27th March 2010                               *
 *******************************************************************************/

#include <fstream>
#include <sstream>

#include "hash_board_13.h"

namespace HexPatterns
{
    const HashBoard_13 HashBoard_13::EmptyHashBoard()
    {
        /* To anyone that will be reading the following code:                  *
         * I am very, very sorry.                                              *
         */
        HashBoard_13 board;
        uint height = (GATHER_SIZE + 1) * Hex::Dim::actual_size + 1;
        uint pos;

        rep(ii, Hex::Dim::actual_field_count) {
            board.pattern_count[ii] = 0;
            board.position_hash[ii] = 0;
        }

        /* Guardians' initialisation */
        rep(ii, GATHER_SIZE) {
            board.pattern_count[ii + 1] =
                board.pattern_count[(ii + 1) * Hex::Dim::actual_size] =
                board.pattern_count[(ii + 2) * Hex::Dim::actual_size - 2] =
                board.pattern_count[height + ii] = 4;

            pos = ii + 1;
            board.patterns_at[pos][0] = pos - 1;
            board.patterns_at[pos][1] = pos + 1;
            board.patterns_at[pos][2] = pos + Hex::Dim::actual_size - 1;
            board.patterns_at[pos][3] = pos + Hex::Dim::actual_size;

            pos = (ii + 1) * Hex::Dim::actual_size;
            board.patterns_at[pos][0] = pos - Hex::Dim::actual_size;
            board.patterns_at[pos][1] = pos - Hex::Dim::actual_size + 1;
            board.patterns_at[pos][2] = pos + 1;
            board.patterns_at[pos][3] = pos + Hex::Dim::actual_size;

            pos = (ii + 2) * Hex::Dim::actual_size - 2;
            board.patterns_at[pos][0] = pos - Hex::Dim::actual_size;
            board.patterns_at[pos][1] = pos - 1;
            board.patterns_at[pos][2] = pos + Hex::Dim::actual_size - 1;
            board.patterns_at[pos][3] = pos + Hex::Dim::actual_size;

            pos = height + ii;
            board.patterns_at[pos][0] = pos - Hex::Dim::actual_size;
            board.patterns_at[pos][1] = pos - Hex::Dim::actual_size + 1;
            board.patterns_at[pos][2] = pos - 1;
            board.patterns_at[pos][3] = pos + 1;
        }
        board.pattern_count[0] = 2;
        board.patterns_at[0][0] = 1;
        board.patterns_at[0][1] = Hex::Dim::actual_size;

        board.pattern_count[Hex::Dim::actual_size - 2] = 3;
        board.patterns_at[Hex::Dim::actual_size - 2][0] = Hex::Dim::actual_size - 3;
        board.patterns_at[Hex::Dim::actual_size - 2][1] = 2 * Hex::Dim::actual_size - 3;
        board.patterns_at[Hex::Dim::actual_size - 2][2] = 2 * Hex::Dim::actual_size - 2;

        board.pattern_count[(Hex::Dim::actual_size - 2) * Hex::Dim::actual_size] = 3;
        board.patterns_at[(Hex::Dim::actual_size - 2) * Hex::Dim::actual_size][0] =
            (Hex::Dim::actual_size - 3) * Hex::Dim::actual_size;
        board.patterns_at[(Hex::Dim::actual_size - 2) * Hex::Dim::actual_size][1] =
            (Hex::Dim::actual_size - 3) * Hex::Dim::actual_size + 1;
        board.patterns_at[(Hex::Dim::actual_size - 2) * Hex::Dim::actual_size][2] =
            (Hex::Dim::actual_size - 2) * Hex::Dim::actual_size + 1;

        board.pattern_count[(Hex::Dim::actual_size - 1) * Hex::Dim::actual_size - 2] = 2;
        board.patterns_at[(Hex::Dim::actual_size - 1) * Hex::Dim::actual_size - 2][0] =
            (Hex::Dim::actual_size - 2) * Hex::Dim::actual_size - 2;
        board.patterns_at[(Hex::Dim::actual_size - 1) * Hex::Dim::actual_size - 2][1] =
            (Hex::Dim::actual_size - 1) * Hex::Dim::actual_size - 3;
        /* Guardians' initialisation */

        /* Adding guardians to topmost and bottommost rows */
        rep(ii, Hex::Dim::actual_size) {
            board.Play(ii, GUARDIAN_STATE);
            board.Play(ii + height - 1, GUARDIAN_STATE);
        }
        /* Adding guardians to topmost and bottommost rows */

        /* Adding guardians to leftmost and rightmost columns */
        rep(ii, GATHER_SIZE) {
            uint left_column = (ii + 1) * Hex::Dim::actual_size;
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
                board.patterns_at[pos][0] = pos - Hex::Dim::actual_size;
                board.patterns_at[pos][1] = pos - Hex::Dim::actual_size + 1;
                board.patterns_at[pos][2] = pos - 1;
                board.patterns_at[pos][3] = pos + 1;
                board.patterns_at[pos][4] = pos + Hex::Dim::actual_size - 1;
                board.patterns_at[pos][5] = pos + Hex::Dim::actual_size;
            }
        /* Board fields' initialisation */

        return board;
    }

    HashBoard_13::HashBoard_13()
    {
        rep(ii, Hex::Dim::actual_field_count)
            pattern_count[ii] = 0;

        memset(position_hash, 0, Hex::Dim::actual_field_count * sizeof(Hash));
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

        rep(ii, Hex::Dim::board_size + 2) {
            rep(jj, Hex::Dim::board_size + 2)
                ret << position_hash[GUARDED_POSITION(ii, jj)] << " ";
            ret << std::endl;
        }

        return ret.str();
    }

    // Converts string Coords to LocationNumber in HashBoard
    uint HashBoard_13::GetLocation(string coords)
    {
        uint x = coords[0] >= 'a' ? coords[0] - 'a' : coords[0] - 'A';
        ++x;
        uint y = coords[1] - '0';
        if (coords.size() > 2)
            y = y * 10 + coords[2] - '0';
        uint res = (y + GUARDS_SIZE -1 ) * (Hex::Dim::actual_size) + x + GUARDS_SIZE - 1;
        return res;
    }
} // namespace HexPatterns
