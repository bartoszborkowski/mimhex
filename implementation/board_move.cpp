#include "board_move.h"

namespace Hex {

Move::Move (const Player& player, const Location& location):
    _player(player), _location(location) {}

Move::Move():
    _player(Player::First()), _location(0) {}

Location Move::GetLocation() const { return _location; }

Player Move::GetPlayer() const { return _player; }

} //namespace Hex
