#ifndef HEX_BOARD_H_
#define HEX_BOARD_H_

#include <string>
#include "small_set.h"
#include "params.h"
#include "switches.h"

using namespace std;

typedef unsigned int uint;
typedef unsigned short ushort;

namespace Hex {


/*
 * TODO LIST:
 *
 * (1) Fix bridges. Make the code more readable and if this is not possible
 *     comment it in greater detail. (DONE - juleg)
 *
 * (2) Audit and retest UpdatePathsStats...(). Specificaly changes applied
 *     during migration to variable guarding could introduce some bugs. Also
 *     some of the procedures were not tested at all, because of lack of
 *     bidirectional F&U.
 *
 * (3) Make UpdatePathsStats() independent from the guarding - initialize
 *     visible[] as visited on borders to achieve this.
 *
 * (4) Make better use of constants introduced in class ::Dim.
 *     Specificaly:
 *     (a) When iterating through a stripe of fields use Dim::OfPos() to mark
 *         initial and final positions.
 *     (b) Use i += Dim::right to move iterator horizontally. Use i += Dim::down
 *         to move iterator vertically.
 *     (c) Use FOR_SIX(int dir) macro whenever there is a need for performing
 *         the same operation for each of the directions Dim::left, Dim::right,
 *         Dim::upper_left, Dim::upper_right, Dim::lower_left and Dim::lower_right.
 *     (d) If FOR_SIX() cannot be used for some reasons, use direction constants
 *         without the macro.
 *     (e) When locating a field within guarded area consider using
 *         Dim::guardedboard_size.
 */

// -----------------------------------------------------------------------------

class Dim {
    public:
        /**
         * The number of guarding rows and columns around the board.
         */
        static const uint guard_count = 2;

        /**
         * The size of the board
         */
        static const uint board_size = 11;

        /**
         * The size of the board increased by the guarding margins on both sides.
         */
        static const uint guardedboard_size = board_size + guard_count * 2;

        /**
         * The size of the board as it is kept in memory.
         */
        static const uint actualboard_size = 16;

        /**
         * A special value added to variables in loops in order to iterate over
         * fields in the same column.
         */
        static const int down = 16;

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
        static const int dirs[6];

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
        static const int dirs2[6];

        static const int clockwise[down * 2 + 1];
        static const int cclockwise[down * 2 + 1];

        /**
         * The number of fields in the board.
         */
        static const uint field_count = board_size * board_size;

        /**
         * The number of fields in the board as it is kept in memory.
         */
        static const uint actual_field_count = actualboard_size * actualboard_size;

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

class Player {
    public:
        static Player First();
        static Player Second();
        static Player None();
        static Player OfString (std::string);

        Player Opponent() const;

        bool operator== (const Player&) const;
        bool operator!= (const Player&) const;

        uint GetVal();

        string ToString() const;

        static bool ValidPlayer(const std::string& player);

    private:
        Player();
        Player(uint val);

    private:
        uint _val;
};

// -----------------------------------------------------------------------------

class Location {
    public:
        static Location OfCoords (std::string);
        Location (uint pos);
        Location (uint x, uint y);
        uint GetPos() const;
        uint GetX() const;
        uint GetY() const;
        std::string ToCoords() const;
        bool operator==(Location loc) const;
        bool operator!=(Location loc) const;
        static bool ValidLocation(const std::string& location);
        static bool ValidLocation(uint x, uint y);
        static bool ValidPosition(uint pos);

    private:
        Location();

    private:
        uint _pos;
};

// -----------------------------------------------------------------------------

class Move {
    public:
        Move (const Player&, const Location&);
        Location GetLocation() const;
        Player GetPlayer() const;

    private:
        Move();

    private:
        Player _player;
        Location _location;
};

// -----------------------------------------------------------------------------

class Board {
    public:
        const static Board Empty();

        typedef ushort* ushort_ptr;

        Board();
        Player CurrentPlayer() const;
        Move GenerateMoveUsingKnowledge(const Player&) const;
        Move RandomLegalMove (const Player&) const;
        Move RandomLegalAvoidBridges (const Player&) const;
        void PlayLegal (const Move&);
        void UpdateBridgeData (uint pos, uint replace_pos);
        bool IsFull() const;
        bool IsWon() const;
        Player Winner() const;
        void Load (const Board&);
        uint MovesLeft() const;
        void GetPossiblePositions(ushort_ptr& locations);
        std::string ToAsciiArt(Location last_move) const;
        void Show() const;
        bool IsValidMove(const Move& move);

    private:
        /**
         * @in @param val A value in the format as used in the internal array.
         * @return The internal representation of the position retrieved from the
         * given value.
         */
        static uint ToPos(ushort val);

        /**
         * @in @param pos The internal representation of a position.
         * @return The value to store in the internal array representing the given
         * position and the first player.
         */
        static ushort ToFirst(ushort pos);

        /**
         * @in @param pos The internal representation of a position.
         * @return The value to store in the internal array representing the given
         * position and the second player.
         */
        static ushort ToSecond(ushort pos);

        /**
         * @in @param pos The internal representation of a position
         * @in @param owner The owner to be associated with the position
         * @return The value to store in the internal bridge set representing the
         * given position and the given owner.
         */
        static ushort ToBridge(ushort pos, ushort owner);

        /**
         * @in @param pos A value in the format as it is stored inside a
         * bridge set.
         * @return The position associated with the value.
         */
        static uint BridgeToPos(ushort val);

        /**
         * @in @param val A value in the format as used in the internal array.
         * @return true iff value represents the first player.
         * @note false is returned for empty fields.
         */
        static bool IsFirst(ushort val);

        /**
         * @in @param val A value in the format as used in the internal array.
         * @return true iff value represents the second player.
         * @note false is returned for empty fields.
         */
        static bool IsSecond(ushort val);

        /**
         * @in @param val A value in the format as used in the internal array.
         * @return true iff value represents an empty field.
         */
        static bool IsEmpty(ushort val);

        /**
         * @in @param val The first value in the format as used in the interal
         * array.
         * @in @param val2 The second value in the format as used in the interal
         * array.
         * @return true iff val and val2 represent the same player.
         * @note val and val2 shouldn't be empty.
         */
        static bool SamePlayer(ushort val, ushort val2);

        /**
         * @in @param val The first value in the format as used in the interal
         * array.
         * @in @param val2 The second value in the format as used in the interal
         * array.
         * @return true iff val and val2 represent opposite players.
         * @note val and val2 shouldn't be empty.
         */
        static bool OppPlayer(ushort val, ushort val2);

        /**
         * Unifies pawn at position pos with its neighbours with the same colour.
         * @in @param pos An internal representation of a position.
         */
        void MakeUnion(uint pos);

        /**
         * Unifies groups of pawns at positions pos1 and pos2 (if present and
         * their colours match).
         * @return A value stored at pos1 in the internal array.
         */
        uint MakeUnion(uint pos1, uint pos2);

        /**
         * @in @param pos An internal representation of a position.
         * @return A value at the root of a F&U tree containing position pos.
         */
        uint Find(uint pos) const;

        /**
         * A version of Find() making no modifications to the F&U trees.
         * @in @param pos An internal representation of a position.
         * @return A value at the root of a F&U tree containing position pos.
         */
        uint ConstFind(uint pos) const;

        /**
         * Update bridges around position pos.
         * @in @param pos A recently changed position.
         */
        void UpdateBridges(uint pos);

        /**
         * Update the order of the field maps to store the bridges after the
         * remaining nonbridge fields. Move bridge fields inside the bridge
         * range of the field map, move nonbridge fields outside the bridge
         * range.
         * @in @param pos A position with possibly invalid index inside the
         * field map.
         */
        void UpdateBridgeBound(uint pos);

        void ClearShortestPathsStats();

    public:
        void UpdatePathsStatsFloodFillFU(Board& aBoard, const Player& winner);
        void UpdatePathsStatsFloodFillBFS(Board& aBoard, const Player& winner);
        void UpdatePathsStatsAllShortestPathsBFS(Board& aBoard, const Player& winner);
        void UpdatePathsStatsOneShortestPathBFS(Board& aBoard, const Player& winner);
        void ShowPathsStats();

    private:
        /**
         * Special values used for F&U roots for each side of the map.
         * FIXME: Preferably the functioning of all UpdatePathsStats...()
         * should be independant from the guarding used. visible[] array may be
         * initialized appropriately to achieve this.
         */
        static const uint root_up = (Dim::guard_count - 1) * Dim::down
                                  + Dim::guard_count;
        static const uint root_down = (Dim::board_size + Dim::guard_count) * Dim::down
                                    + Dim::guard_count;
        static const uint root_left = Dim::guard_count * Dim::down
                                    + Dim::guard_count - 1;
        static const uint root_right = Dim::guard_count * Dim::down
                                     + Dim::guard_count + Dim::board_size;

        /**
         * Special value used for fields that are empty
         */
        static const ushort board_empty = 0;

        /**
         * Magic value used for aquiring actual positions from values kept in the
         * board for the second player. Specifically: if x is a value kept in a
         * field with position y, than value of x & board_second can be used for
         * testing wheather the field is posessed by the second player. The value
         * is the link in the F&U tree for the second player.
         */
        static const ushort board_second = 0x8000;

        /**
         * The structure holds F&U structures. If Switches::DetectWins() is set
         * two separate F&U trees are maintained for each player. Otherwise only a
         * single tree is maintained for the first player and a special constant
         * value is used to mark fields occupied by the second player.
         */
        mutable ushort board[Dim::actual_field_count];

    public:
        /**
         * DEPRECATED: Scheduled for removal. Arrays inside mcts nodes will be
         * utilized instead.
         * A custom optimization used in a fashion similar to AMAF. The structure
         * holds the number paths crossing through the field that caused a player
         * to win. If the flag TODO is set, actual shortest paths are counted,
         * otherwise - flooded areas containing the field.
         */
        short timesOfBeingOnShortestPath[Dim::actual_field_count];

    private:
        /**
         * Swaps values kept in the field map at index and index2. Updates the
         * reverse field map accordingly.
         * @in @param index The index of the first value to be swaped
         * @in @param index2 The index of the second value to be swaped.
         */
        void SwapFree(uint index, uint index2);

        /**
         * Moves position pos kept in the field map to the index index. Updates
         * the reverse field map accordingly.
         * @in @param pos The position to be moved in the field map.
         * @in @param index The index for the value pos.
         */
        void MoveFree(uint pos, uint index);

        /**
         * Removes the position pos from the field map. Updates the reverse
         * field maps accordingly.
         * @in @param pos The position to be removed from the field map.
         * @return The new value stored in the field map in place of pos.
         */
        uint RemoveFree(uint pos);

        /**
         * The array holds positions of all free fields in the board. It is
         * used for fast iteration over all possible moves. If bridges in any
         * form are enabled then nonbridge fields are always kept first in the
         * array.
         * @note Formerly known as _fast_field_map
         */
        ushort field_map[Dim::actual_field_count];

        /**
         * The array holds field map indices for each position. The following
         * invariant is true:
         *   field_map[rev_map[x]] = x
         * where x is a valid, free position. Also holds:
         *   rev_map[field_map[x]] = x
         * for any x within bounds.
         * @note Formerly known as _reverse_fast_field_map
         */
        ushort rev_map[Dim::actual_field_count];

        /**
         * The array holds for each position bridges associated with it. By
         * simple computation there can be proven that there are only up to
         * 3 such bridges. Each element within a set represents a field
         * position and its owner. The format used is identical to the one
         * in board array.
         */
        SmallSet<ushort> bridge_conn[Dim::actual_field_count];

        /**
         * The array holds positions of all attacked bridges in the board.
         * By a simple estimate there can only be up to 1/2 such fields.
         */
        SmallSet<ushort, Dim::field_count / 3> attacked_bridges;

        /**
         * Moves left until the board is full.
         */
        uint moves_left;

        /**
         * The index of the last field in field_map that isn't a bridge.
         * All fields with indices from bridge_range + 1 up to moves_left - 1
         * are bridges.
         */
        int bridge_range;

        /**
         * Player to make the next move.
         */
        Player current;
};

// -----------------------------------------------------------------------------

} // namespace Hex

#endif
