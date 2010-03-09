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

const uint kBoardSize = 11;
const uint kBoardSizeAligned = 16;	// kBoardSize aligned to nearest higher power of 2

// -----------------------------------------------------------------------------

class Player {
public:
    static Player First();
    static Player Second();
    static Player OfString (std::string);

    Player Opponent() const;

    bool operator== (const Player&);
    bool operator!= (const Player&);

    uint GetVal();

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
    std::string ToCoords() const;
    bool operator==(Location loc) const;
    bool operator!=(Location loc) const;
    static bool ValidLocation(const std::string& location);
    static bool ValidLocation(uint x, uint y);
    static bool ValidPosition(uint pos);
    static void ToCoords(uint pos, uint& x, uint& y);
    static uint ToTablePos(uint x, uint y);

private:
    /**
     * The size of the board as it is kept in memory.
     */
    static const uint board_size = kBoardSize;

    /**
     * The size of the board as it is kept in memory.
     */
    static const uint actual_board_size = kBoardSizeAligned;

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
    static uint ToPos(int x, int y);
    static uint ToPos(ushort val);
    static ushort ToFirst(ushort pos);
    static ushort ToSecond(ushort pos);
    static bool IsFirst(ushort val);
    static bool IsSecond(ushort val);
    static bool IsEmpty(ushort val);

    void MakeUnion(uint pos);
    uint MakeUnion(uint pos1, uint pos2);
    uint Find(uint pos);
    uint ConstFind(uint pos) const;

    void UpdateBridges(uint pos);
    void UpdateBridgeBound(uint pos);

    void clearShortestPathsStats();
    void UpdatePathsStatsFloodFillFU(Board& board, Player& winner);
    void UpdatePathsStatsFloodFillBFS(Board& board, Player& winner);
    void UpdatePathsStatsAllShortestPathsBFS(Board& aBoard, Player& winner);
    void UpdatePathsStatsOneShortestPathBFS(Board& aBoard, Player& winner);

private:
    /**
     * The number of guarding rows and columns around the board.
     */
    static const uint guard_count = 2;

    /**
     * The size of the board
     */
    static const uint board_size = kBoardSize;

    /**
     * The size of the board increased by the guarding margins on both sides.
     */
    static const uint guarded_board_size = board_size + guard_count * 2;

    /**
     * The size of the board as it is kept in memory.
     */
    static const uint actual_board_size = kBoardSizeAligned;

    /**
     * The number of fields in the board.
     */
    static const uint field_count = board_size * board_size;

    /**
     * The number of fields in the board as it is kept in memory.
     */
    static const uint actual_field_count = actual_board_size * actual_board_size;

    /**
     * Special values used for F&U roots for each side of the map.
     */
    static const ushort root_up = 1;
    static const ushort root_down = 2;
    static const ushort root_left = 3;
    static const ushort root_right = 4;

    /**
     * Magic value used for fields that are empty
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
     * single tree is maintained for the first player. A special value -1 is
     * used to mark fields occupied by the second player.
     * The root for the first player tree is field TODO The root for the tree
     * for the second player is TODO.
     */
    ushort _board[actual_field_count];

    /**
     * A custom optimization used in a fashion similar to AMAF. The structure
     * holds the number paths crossing through the field that caused a player
     * to win. If the flag TODO is set, actual shortest paths are counted,
     * otherwise - flooded areas containing the field.
     * TODO: Add the remaining variants.
     */
    short timesOfBeingOnShortestPath[kBoardSizeAligned * kBoardSizeAligned];

    /**
     * The table holds positions of all free fields in the board. The following
     * invariant always holds: fields associated with other fields through
     * bridges are always kept first.
     */
    ushort _fast_field_map[kBoardSizeAligned * kBoardSizeAligned];

    /**
     * The table holds _fast_field_map indices of all free
     */
    ushort _reverse_fast_field_map[kBoardSizeAligned * kBoardSizeAligned];

/*These ones are used in bridges.
    First in the pair is an index of second free field in bridge. Second guy from pair
    says if bridge is built by first player.*/
    SmallSet<pair<ushort,bool> > _field_bridge_connections[kBoardSizeAligned * kBoardSizeAligned];
    SmallSet<ushort, 50> attacked_bridges;

    uint _moves_left;
    int _field_map_bound;		// index of last field in _fast_field_map that isn't a bridge
    Player _current;
};

// -----------------------------------------------------------------------------

} // namespace Hex

#endif
