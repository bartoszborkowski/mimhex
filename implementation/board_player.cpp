#include "board_player.h"
#include "conditional_assert.h"
#include <cstring>
#include <sstream>
#include <iostream>

namespace Hex {


Player Player::First() { return Player(1); }

Player Player::Second() { return Player(3); }

Player Player::None() { return Player(0); }

Player Player::OfString (std::string player) {
    ASSERT(ValidPlayer(player));
    if (player == "black")
        return Player::First();
    else
        return Player::Second();
}

Player Player::Opponent() const {
    return Player(_val ^ 2);
}

bool Player::operator== (const Player& player) const {
    return player._val == _val;
}

bool Player::operator!= (const Player& player) const {
    return player._val != _val;
}

Player::Player(uint val) : _val(val) {}

uint Player::GetVal() { return _val; }

bool Player::ValidPlayer(const std::string& player) {
    return player == "black" || player == "white";
}

string Player::ToString() const {
    if (*this == First())
        return "black";
    else if (*this == Second())
        return "white";
    else
        return "none";
}

} //namespace Hex
