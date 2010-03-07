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
  Player Winner() const;
  void Load (const Board&);
  uint MovesLeft() const;
  void GetPossiblePositions(ushort_ptr& locations);
  std::string ToAsciiArt(Location last_move) const;
  bool IsValidMove(const Move& move);

 private:
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

 private:
  static const uint table_size;
  static const uint guarded_board_size;
  short _board[kBoardSizeAligned * kBoardSizeAligned];

/*this is used in feature which is variation of AMAF*/
  short timesOfBeingOnShortestPath[kBoardSizeAligned * kBoardSizeAligned];

/*these two ones are used in find and union. _fast_field_map is kind of sorted - 
  fields that aren't bridges are before those, which are.*/
  ushort _fast_field_map[kBoardSizeAligned * kBoardSizeAligned];
  ushort _reverse_fast_field_map[kBoardSizeAligned * kBoardSizeAligned];

/*These ones are used in bridges. 
  First in the pair is an index of second free field in bridge. Second guy from pair
  says if bridge is built by first player.*/
  SmallSet<pair<ushort,bool> > _field_bridge_connections[kBoardSizeAligned * kBoardSizeAligned]; 
  SmallSet<ushort, 50> attacked_bridges;

  uint _moves_left;
  int _field_map_bound;		// index of last field in _fast_field_map that isn't a bridge
  Player _current;

public:
  Switches switches;		//set of info about what knowledge we use.
							// Assuming that it doesn't change during single "random" playout

};

// -----------------------------------------------------------------------------

} // namespace Hex

#endif
