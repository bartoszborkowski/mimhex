#ifndef HEX_BOARD_H_
#define HEX_BOARD_H_

#include <string>

#include "small_set.h"

using namespace std;

typedef unsigned int uint;
typedef unsigned short ushort;

namespace Hex {

  const uint kBoardSize = 11;
  const uint kBoardSizeAligned = 16;	// kBoardSize aligned to nearest higher power of 2
  const uint kFieldsAlignedAmount = kBoardSizeAligned * kBoardSizeAligned;

// -----------------------------------------------------------------------------

class Player {
 public:
  static Player First();
  static Player Second();
  static Player OfString (std::string);

  Player Opponent() const;

  bool operator== (const Player&);
  bool operator!= (const Player&);

  uint GetVal() const;

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
}

#include "sampler.h"

namespace Hex {

class Board {
 public:
  const static Board Empty();

  typedef ushort* ushort_ptr;

  Board();
  Player CurrentPlayer() const;
  Move DefendBridgeMove(const Player&) const;
  Move RandomLegalMove (const Player&) const;
  //Move RandomLegalMoveIgnoreBridges (const Player&) const;
  void PlayLegal (const Move&);
  //void PlayLegalIgnoreBridges (const Move&);
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

 private:
  static const uint table_size;
  static const uint guarded_board_size;
  short _board[kBoardSizeAligned * kBoardSizeAligned];
  ushort _fast_field_map[kBoardSizeAligned * kBoardSizeAligned];
  ushort _reverse_fast_field_map[kBoardSizeAligned * kBoardSizeAligned];
  SmallSet<pair<ushort,bool> > _field_bridge_connections[kBoardSizeAligned * kBoardSizeAligned]; 
									/*bool mówi, czy most należy do pierwszego gracza*/
  uint _moves_left;
  int _field_map_bound;
  Player _current;

  Sampler sampler;

  SmallSet<ushort, 50> attacked_bridges;

};

// -----------------------------------------------------------------------------

} // namespace Hex

#endif
