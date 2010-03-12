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
        static const uint guarded_board_size = board_size + guard_count * 2;

        /**
         * The size of the board as it is kept in memory.
         */
        static const uint actual_board_size = 16;

        /**
         * A special value added to variables in loops in order to iterate over
         * fields in the same column.
         */
        static const int down = 16;

        /**
         * Special values that can be added to a positions in order to get another,
         * relatively shifted positions.
         */
        static const int upper_left = -actual_board_size;
        static const int upper_right = -actual_board_size + 1;
        static const int left = -1;
        static const int right = +1;
        static const int lower_left = actual_board_size - 1;
        static const int lower_right = actual_board_size;

        /**
         * An array containing all the directions. Used by FOR_SIX() macro.
         */
        static const int dirs[6];

        /**
         * The number of fields in the board.
         */
        static const uint field_count = board_size * board_size;

        /**
         * The number of fields in the board as it is kept in memory.
         */
        static const uint actual_field_count = actual_board_size * actual_board_size;

        /**
         * @in @param x A field coordinant, where 1 describes leftmost column.
         * @in @param y A field coordinant, where 1 describes uppermost row.
         * @return The internal representation of position (x, y).
         */
        static uint ToPos(int x, int y);

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
         * @in @param val A value in the format as used in the internal table.
         * @return The internal representation of the position retrieved from the
         * given value.
         */
        static uint ToPos(ushort val);

        /**
         * @in @param pos The internal representation of a position.
         * @return The value to store in the internal table representing the given
         * position and the first player.
         */
        static ushort ToFirst(ushort pos);

        /**
         * @in @param pos The internal representation of a position.
         * @return The value to store in the internal table representing the given
         * position and the second player.
         */
        static ushort ToSecond(ushort pos);

        /**
         * @in @param val A value in the format as used in the internal table.
         * @return true iff value represents the first player.
         */
        static bool IsFirst(ushort val);

        /**
         * @in @param val A value in the format as used in the internal table.
         * @return true iff value represents the second player.
         * @note false is returned for empty fields.
         */
        static bool IsSecond(ushort val);

        /**
         * @in @param val A value in the format as used in the internal table.
         * @return true iff value represents an empty field.
         */
        static bool IsEmpty(ushort val);

        /**
         * Unifies pawn at position pos with its neighbours with the same colour.
         * @in @param pos An internal representation of a position.
         */
        void MakeUnion(uint pos);

        /**
         * Unifies pawns at positions pos1 and pos2 (if present and their colour
         * matches).
         * @return A value stored at pos1 in the internal table.
         */
        uint MakeUnion(uint pos1, uint pos2);

        /**
         * @in @param pos An internal representation of a position.
         * @return A value at the root of a F&U tree containing position pos.
         */
        uint Find(uint pos);

        /**
         * A version of Find() making no modifications to the F&U trees.
         * @in @param pos An internal representation of a position.
         * @return A value at the root of a F&U tree containing position pos.
         */
        uint ConstFind(uint pos) const;

        /**
         * FIXME: Internal table representation changed.
         */
        void UpdateBridges(uint pos);

        /**
         * FIXME: Internal table representation changed.
         */
        void UpdateBridgeBound(uint pos);

        void clearShortestPathsStats();

    public:
        void UpdatePathsStatsFloodFillFU(Board& aBoard, const Player& winner);
        void UpdatePathsStatsFloodFillBFS(Board& aBoard, const Player& winner);
        void UpdatePathsStatsAllShortestPathsBFS(Board& aBoard, const Player& winner);
        void UpdatePathsStatsOneShortestPathBFS(Board& aBoard, const Player& winner);
        void ShowPathsStats();

    private:
        /**
         * Special values used for F&U roots for each side of the map.
         */
        static const ushort root_up = (Dim::guard_count - 1) * Dim::down
                                     + Dim::guard_count;
        static const ushort root_down = (Dim::board_size + Dim::guard_count) * Dim::down
                                       + Dim::guard_count;
        static const ushort root_left = Dim::guard_count * Dim::down
                                      + Dim::guard_count - 1;
        static const ushort root_right = Dim::guard_count * Dim::down
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
        ushort _board[Dim::actual_field_count];

    public:
        /**
         * A custom optimization used in a fashion similar to AMAF. The structure
         * holds the number paths crossing through the field that caused a player
         * to win. If the flag TODO is set, actual shortest paths are counted,
         * otherwise - flooded areas containing the field.
         * TODO: Add the switch.
         */
        short timesOfBeingOnShortestPath[Dim::actual_field_count];

    private:
        /**
         * The table holds positions of all free fields in the board. The following
         * invariant always holds: fields associated with other fields through
         * bridges are always kept first.
         */
        ushort _fast_field_map[Dim::actual_field_count];

        /**
         * The table holds _fast_field_map indices of all free
         */
        ushort _reverse_fast_field_map[Dim::actual_field_count];

        /*
         * These ones are used in bridges.
         * First in the pair is an index of second free field in bridge. Second guy from pair
         * says if bridge is built by first player.
         */
        SmallSet<pair<ushort,bool> > _field_bridge_connections[Dim::actual_field_count];
        SmallSet<ushort, 50> attacked_bridges;

        uint _moves_left;

        // index of last field in _fast_field_map that isn't a bridge
        int _field_map_bound;

        Player _current;
};

// -----------------------------------------------------------------------------

} // namespace Hex

#endif
