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
    bool IsValidMove(const Move& move);

private:
    uint Get(uint pos);

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
     * The number of fields in the board.
     */
    static const uint table_size;

    /**
     * The number of guarding rows and columns around the board.
     */
    static const uint guard_count;

    /**
     * The actual size of the board (including guarding fields).
     */
    static const uint guarded_board_size;

    /**
     * The structure holds F&U structures. If Switches::DetectWins() is set
     * two separate F&U trees are maintained for each player. Otherwise only a
     * single tree is maintained for the first player. A special value -1 is
     * used to mark fields occupied by the second player.
     * The root for the first player tree is field TODO The root for the tree
     * for the second player is TODO.
     */
    short _board[kBoardSizeAligned * kBoardSizeAligned];

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
