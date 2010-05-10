#ifndef HEX_BOARD_DIM_H_
#define HEX_BOARD_DIM_H_

#include <string>
#include "hextypes.h"
#include "params.h"
#include "switches.h"

namespace Hex{

class Dim {
    public:
        /**
         * The number of guarding rows and columns around the board.
         */
        static const uint guard_count = 2;

        /**
         * The size of the board
         */
        static const uint board_size = 13;

        /**
         * The size of the board increased by the guarding margins on both sides.
         */
        static const uint guarded_size = board_size + guard_count * 2;

        /**
         * The size of the board as it is kept in memory.
         */
        static const uint actual_size = 32;

        /**
         * A special value added to variables in loops in order to iterate over
         * fields in the same column.
         */
        static const int down = actual_size;

        /**
         * Special values that can be added to positions in order to get another,
         * relatively shifted positions.
         */
        static const int upper_left = -down;
        static const int upper_right = -down + 1;
        static const int left = -1;
        static const int right = +1;
        static const int lower_left = +down - 1;
        static const int lower_right = +down;

        /**
         * An array containing all the directions. Used by FOR_SIX() macro.
         */
        static const int neighbours[6];

        /**
         * Special values that can be added to positions in order to get another,
         * relatively shifted position useful for bridges.
         */
        static const int up2 = -down * 2 + 1;
        static const int left_up2 = -down - 1;
        static const int left_down2 = +down - 2;
        static const int down2 = +down * 2 - 1;
        static const int right_up2 = -down + 2;
        static const int right_down2 = +down - 1;

        /**
         * An array containing all the directions. Used by FOR_SIX2() macro.
         */
        static const int other_bridge_ends[6];

        static const int clockwise[down * 2 + 1];
        static const int cclockwise[down * 2 + 1];

        /**
         * The number of fields in the board.
         */
        static const uint field_count = board_size * board_size;

        /**
         * The number of fields in the board as it is kept in memory.
         */
        static const uint actual_field_count = actual_size * actual_size;

        /**
         * @in @param x A field coordinant, where 1 describes leftmost column.
         * @in @param y A field coordinant, where 1 describes uppermost row.
         * @return The internal representation of position (x, y).
         */
        static uint OfPos(int x, int y);

        /**
         * @in @param x The horizontal shift.
         * @in @param y The vertical shift.
         */
        static int ByPos(int x, int y);

        /**
         * @in @param pos A field position
         * @return A field coordinant X, where 1 describes leftmost column.
         */
        static int ToX(uint pos);

        /**
         * @in @param pos A position.
         * @return A field coordinant Y, where 1 describes uppermost row.
         */
        static int ToY(uint pos);

        /**
         * @in @param pos A position.
         * @out @param x A field coordinant, where 1 describes leftmost column.
         * @out @param y A field coordinant, where 1 describes uppermost row.
         */
        static void ToCoords(uint pos, int& x, int& y);

        static int Clockwise(int pos);

        static int CClockwise(int pos);

    private:
        Dim();
};

} //namespace Hex

#endif
