#include "board_location.h"
#include "conditional_assert.h"
#include "board_dim.h"
#include <cstring>
#include <sstream>
#include <iostream>

namespace Hex {

Location Location::OfCoords (std::string coords) {
    ASSERT(ValidLocation(coords));
    uint x = coords[0] >= 'a' ? coords[0] - 'a' : coords[0] - 'A';
    uint y = coords[1] - '0';
    if (coords.size() > 2)
        y = y * 10 + coords[2] - '0';
    return Location(++x, y);
}

Location::Location(uint x, uint y):
    _pos(Dim::OfPos(x, y)) {}

Location::Location(uint pos):
    _pos(pos) {}

Location::Location() {}

uint Location::GetPos() const { return _pos; }

uint Location::GetX() const { return _pos % Dim::actual_size - 1; }

uint Location::GetY() const { return _pos / Dim::actual_size - 1; }

std::string Location::ToCoords() const {
    std::stringstream coords;
    coords << static_cast<char>(Dim::ToX(_pos) + 'a' - 1);
    coords << Dim::ToY(_pos);
    return coords.str();
}

bool Location::operator==(Location loc) const {
    return loc._pos == _pos;
}

bool Location::operator!=(Location loc) const {
    return loc._pos != _pos;
}

bool Location::ValidLocation(const std::string& location) {
    if (location.size() == 0 || location.size() > 3)
        return false;
    uint x = location[0] >= 'a' ? location[0] - 'a' : location[0] - 'A';
    uint y = location[1] - '0';
    if (location.size() > 2)
        y = y * 10 + location[2] - '0';
    return ValidLocation(++x, y);
}

bool Location::ValidPosition(uint pos) {
    int x, y;
    Dim::ToCoords(pos, x, y);
    return ValidLocation(x, y);
}

bool Location::ValidLocation(uint x, uint y) {
    return x > 0 && y > 0 && x <= Dim::board_size && y <= Dim::board_size;
}

} //namespace Hex
