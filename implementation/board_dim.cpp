#include "board_dim.h"

namespace Hex{

const int Dim::dirs[6] = { upper_left, upper_right, left, right, lower_left, lower_right };
const int Dim::dirs2[6] = { up2, left_up2, left_down2, down2, right_down2, right_up2 };

const int Dim::clockwise[Dim::down * 2 + 1] = {
    upper_right, // upper_left -> upper_right
    right, // upper_right -> right
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 13 times
    upper_left, // left -> upper_left
    0,
    lower_right, // right -> lower_right
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 13 times
    left, // lower_left -> left
    lower_left // lower_right -> lower_left
};

const int Dim::cclockwise[Dim::down * 2 + 1] = {
    left, // upper_left -> left
    upper_left, // upper_right -> upper_left
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 13 times
    lower_left, // left -> lower_left
    0,
    upper_right, // right -> lower_right
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 13 times
    lower_right, // lower_left -> lower_right
    right // lower_right -> right
};

uint Dim::OfPos(int x, int y) {
    return (x + Dim::guard_count - 1) + (y + Dim::guard_count - 1) * Dim::down;
}

int Dim::ByPos(int x, int y) {
    return x + y * Dim::down;
}

int Dim::ToX(uint pos) {
    return pos % Dim::actualboard_size - 1;
}

int Dim::ToY(uint pos) {
    return pos / Dim::actualboard_size - 1;
}

void Dim::ToCoords(uint pos, int& x, int& y) {
    x = ToX(pos);
    y = ToY(pos);
}

int Dim::Clockwise(int dir) {
    return clockwise[down + dir];
}

int Dim::CClockwise(int dir) {
    return cclockwise[down + dir];
}


} //namespace Hex
