/*uber*/

#include "board.h"
#include "random.h"
#include "conditional_assert.h"
#include "boost/foreach.hpp"
#include <cstring>
#include <sstream>
#include <iostream>

#define SHORT_BIT_SIZE (sizeof(short) * 8)

namespace Hex {

// -----------------------------------------------------------------------------

const int Dim::dirs[6] = { upper_left, upper_right, left, right, lower_left, lower_right };

uint Dim::ToPos(int x, int y) {
    return (x + Dim::guard_count - 1) + (y + Dim::guard_count - 1) * Dim::down;
}

int Dim::ToX(uint pos) {
    return pos % Dim::actual_board_size - 1;
}

int Dim::ToY(uint pos) {
    return pos / Dim::actual_board_size - 1;
}

void Dim::ToCoords(uint pos, int& x, int& y) {
    x = ToX(pos);
    y = ToY(pos);
}

#define FOR_SIX(x)      BOOST_FOREACH(x, Dim::dirs)

// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------

Location Location::OfCoords (std::string coords) {
    ASSERT(ValidLocation(coords));
    uint x = coords[0] >= 'a' ? coords[0] - 'a' : coords[0] - 'A';
    uint y = coords[1] - '0';
    if (coords.size() > 2)
        y = y * 10 + coords[2] - '0';
    return Location(++x, y);
}

Location::Location(uint x, uint y):
    _pos(Dim::ToPos(x, y)) {}

Location::Location(uint pos):
    _pos(pos) {}

Location::Location() {}

uint Location::GetPos() const { return _pos; }

uint Location::GetX() const { return _pos % Dim::actual_board_size - 1; }

uint Location::GetY() const { return _pos / Dim::actual_board_size - 1; }

std::string Location::ToCoords() const {
    std::stringstream coords;
    coords << static_cast<char>(Dim::ToY(_pos) + 'a' - 1);
    coords << Dim::ToX(_pos);
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

// -----------------------------------------------------------------------------

Move::Move (const Player& player, const Location& location)
    : _player(player), _location(location) {}

Move::Move() :_player(Player::First()), _location(0) {}

Location Move::GetLocation() const { return _location; }

Player Move::GetPlayer() const { return _player; }

// -----------------------------------------------------------------------------

uint Board::ToPos(ushort val) {
    if (Switches::DetectWins())
        return val & (~board_second);
    else
        return val;
}

ushort Board::ToFirst(ushort pos) {
    return pos;
}

ushort Board::ToSecond(ushort pos) {
    if (Switches::DetectWins())
        return pos | board_second;
    else
        return static_cast<ushort>(-1);
}

bool Board::IsFirst(ushort val) {
    return !((val - 1) & board_second);
}

bool Board::IsSecond(ushort val) {
    if (Switches::DetectWins())
        return val & board_second;
    else
        return val == static_cast<ushort>(-1);
}

bool Board::IsEmpty(ushort val) {
    return val == 0;
}

const Board Board::Empty() {

    Board board;

    uint counter = 0;
    for (uint i = 0; i < Dim::actual_field_count; ++i)
        board._reverse_fast_field_map[i] = -1;
    for (uint i = 1; i <= Dim::board_size; ++i) {
        for (uint j = 1; j <= Dim::board_size; ++j) {
            signed int field = Dim::ToPos(i, j);
            board._fast_field_map[counter] = field;
            board._reverse_fast_field_map[field] = counter++;
        }
    }

    for (uint i = 0; i < Dim::actual_field_count; ++i)
        board._board[i] = board_empty;
    for (uint i = 1; i <= Dim::board_size; ++i) {
        board._board[Dim::ToPos(i, 0)] = ToFirst(root_up);
        board._board[Dim::ToPos(i, Dim::board_size + 1)] = ToFirst(root_down);
        board._board[Dim::ToPos(0, i)] = ToSecond(root_left);
        board._board[Dim::ToPos(Dim::board_size + 1, i)] = ToSecond(root_right);
    }

    board.clearShortestPathsStats();

    return board;
}

void Board::clearShortestPathsStats() {

    for(uint i = 0; i < Dim::actual_field_count; i++)
        timesOfBeingOnShortestPath[i]=0;
}

// TODO:
// from juleg: Improve comments - certain loops are uncommented. For the rest
// comments are very brief. For example single "bfs" comment before a loop does
// not really explain a lot. What is kept in visited[] in each loop? What is this
// loop intended to do: fill in predecessor table, discover the actual paths.
// From which position(s) does it start. What kind of initialization of the
// visited[] table is performed?
void Board::UpdatePathsStatsAllShortestPathsBFS(Board& aBoard, const Player& winner) {

    // bfs queue
    short queue[Dim::actual_field_count / 2 + 1];
    // marks in which step node was visited
    unsigned short visited[Dim::actual_field_count];
    // queue markings
    short beg = 0, end = -1;

    memset(visited, 0, Dim::actual_field_count * sizeof(visited[0]));

    if (Player::First() == winner) {
        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                  i < Dim::guard_count * Dim::down + Dim::board_size + Dim::guard_count;
                  i = i + Dim::right)
            if (IsFirst(_board[i])) {
                // I put to queue every node by the edge
                queue[++end]=i;
                //and mark it as visited in first step
                visited[i]=1;
            }

        // bfs
        for (; beg <= end; ++beg)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsFirst(_board[queue[beg] + dir])){
                    queue[++end] = queue[beg] + dir;
                    visited[queue[end]] = visited[queue[beg]] + 1;
                }

        /*
         * Now I have to find the shortest paths.
         * I find out which nodes by the other edge were visited as first ones
         */
        uint min = (uint) - 1;
        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count;
                  i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size;
                  i = i + Dim::right)
            if(visited[i] > 0 && visited[i] < min)
                min = visited[i];

        // cleaning queue
        beg = 0;
        end = -1;
        int actual_mins = 0;
        int next_mins = 0;
        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count;
                  i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size;
                  i = i + Dim::right)
            if (visited[i] == min){
                queue[++end] = i;
                aBoard.timesOfBeingOnShortestPath[i]++;
                visited[i] = 0;
                actual_mins++;
            }

        // second bfs done backwards
        for(; beg <= end; beg++) {
            FOR_SIX(int dir)
                if (visited[queue[beg] + dir] == min - 1 && visited[queue[beg] + 1]) {
                    next_mins++;
                    queue[++end]=queue[beg] + dir;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
            if (--actual_mins == 0){
                actual_mins =next_mins;
                next_mins = 0;
                min--;
            }
        }

    } else {
        // Analogically for the other player marked with values < 0
        // there are changes in 'for' statements (vertically, not horizontally)
        // and in comparisions to 0 (lesser, not greater)

        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::actual_board_size + Dim::guard_count;
                  i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count;
                  i = i + Dim::down)
            if (IsSecond(_board[i])) {
                queue[++end] = i;
                visited[i] = 1;
            }

        for(; beg <= end; beg++)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsSecond(_board[queue[beg] + dir])){
                    queue[++end] = queue[beg] + dir;
                    visited[queue[end]] = visited[queue[beg]] + dir;
                }

        uint min = (uint) -1;
        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count + Dim::board_size - 1;
                  i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size - 1;
                  i = i + Dim::down)
            if (visited[i] > 0 && visited[i] < min)
                min = visited[i];

        beg = 0;
        end = -1;
        int actual_mins = 0;
        int next_mins = 0;
        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count + Dim::board_size - 1;
                  i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size - 1;
                  i = i + Dim::down) {
            if (visited[i] == min) {
                queue[++end] = i;
                aBoard.timesOfBeingOnShortestPath[i]++;
                visited[i] = 0;
                actual_mins++;
            }
        }

        for (; beg <= end; beg++) {
            FOR_SIX(int dir)
                if (visited[queue[beg] + dir] == min - 1 && visited[queue[beg] + dir]) {
                    next_mins++;
                    queue[++end]=queue[beg] + dir;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }

            if (--actual_mins == 0) {
                actual_mins = next_mins;
                next_mins = 0;
                min--;
            }
        }
    }
}

// FIXME: Ensure variable guards are handled correctly.
// NOTE: This code should work for Dim::guard_count == 2.
// TODO: Add comments to each loop explaining briefly what it is doing.
// Explain what is kept in visited[] table, what kind of initialization of the
// table is performed. See UpdatePathsStatsAllShortestPathsBFS() for details.
void Board::UpdatePathsStatsOneShortestPathBFS(Board& aBoard, const Player& winner) {

    short queue[Dim::actual_field_count / 2 + 1];
    unsigned short visited[Dim::actual_field_count];
    short beg = 0, end = -1;

    memset(visited, 0, Dim::actual_field_count * sizeof(visited[0]));

    if (Player::First() == winner) {
        // FIXME: Use ToPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                  i < Dim::guard_count * Dim::down + Dim::board_size + Dim::guard_count;
                  i += Dim::right)
            if (IsFirst(_board[i])) {
                queue[++end] = i;
                visited[i] = 1;
            }

        for (; beg<=end; beg++)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsFirst(_board[queue[beg] + dir])){
                    queue[++end] = queue[beg] + dir;
                    visited[queue[end]] = visited[beg] + 1;
                }

        // avoiding warnings
        uint current = 0;
        uint min = (uint) -1;
        for (uint i = (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count;
                  i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size;
                  i += Dim::right) {
            if (visited[i] > 0 && visited[i] < min) {
                min = visited[i];
                current = i;
            }
        }
        aBoard.timesOfBeingOnShortestPath[current]++;

        for (;;) {
            loop: FOR_SIX (int dir)
                if (visited[current + dir] == visited[current] - 1){
                    current = current + dir;
                    aBoard.timesOfBeingOnShortestPath[current]++;
                    goto loop;
                }
            break;
        }

    } else {

        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                  i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count;
                  i = i + Dim::down)
            if (IsSecond(_board[i])) {
                queue[++end] = i;
                visited[i] = 1;
            }

        for(; beg <= end; beg++)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsSecond(_board[queue[beg] + dir])) {
                    queue[++end]=queue[beg] + dir;
                    visited[queue[end]] = visited[beg] + 1;
                }

        //avoiding warnings
        uint current = 0;
        uint min = (uint) -1;
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count - 1 + Dim::board_size;
                  i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size;
                  i = i + Dim::down) {
            if (visited[i] > 0 && visited[i] < min) {
                min = visited[i];
                current = i;
            }
        }

        aBoard.timesOfBeingOnShortestPath[current]++;
        for (;;) {
            loop2: FOR_SIX (int dir)
                if (visited[current + dir] == visited[current] - 1){
                    current = current + dir;
                    aBoard.timesOfBeingOnShortestPath[current]++;
                    goto loop2;
                }
            break;
        }
    }

}

/*
 * FIXME Test this procedure! No testing has been perfomed thus far.
 */
void Board::UpdatePathsStatsFloodFillFU(Board& aBoard, const Player& winner){
    /* first uses positive numbers, second -1s */
    /* _board[(guarded_board_size - 2) * Dim::actual_board_size  + 2] is always a guard of a first type */
    /* _board[2 * Dim::actual_board_size  + 1] is always a guard of a second type */

    // Two-directional F&U is required for the procedure to function.
    ASSERT(Switches::DetectWins());

    int startingPoint;
    uint parent;

    if (Player::First() == winner)
        startingPoint = (Dim::guarded_board_size - 2) * Dim::down  + 2;
    else
        startingPoint = 2 * Dim::down + 1;

    parent = Find(startingPoint);

    for(uint i = 0; i < Dim::actual_field_count; i++)
        aBoard.timesOfBeingOnShortestPath[i] += (short) (Find(_board[i]) == parent);
}

void Board::ShowPathsStats() {

    cerr << "SHOREST PATHS STATS:\n";

    for (uint i = 0; i < Dim::actual_board_size; i++){
        for (uint j = 0; j < Dim::actual_board_size; j++)
            cerr << " " << timesOfBeingOnShortestPath[i * Dim::down + j] << " ";
        cerr << "\n";
    }

    cerr << "END OF SHOREST PATHS STATS\n";
}

// FIXME: Ensure variable guards are handled correctly.
// NOTE: This code should work for Dim::guard_count == 2.
// TODO: Add comments to each loop explaining briefly what it is doing.
// Explain what is kept in visited[] table, what kind of initialization of the
// table is performed. See UpdatePathsStatsAllShortestPathsBFS() for details.
void Board::UpdatePathsStatsFloodFillBFS(Board& aBoard, const Player& winner){

    // Two-directional F&U is required for this procedure to function.
    ASSERT(Switches::DetectWins());

    if (Player::First() == winner) {
        int startingPoint;
        uint parent;

        startingPoint = (Dim::guarded_board_size - 2) * Dim::actual_board_size + 2;

        parent = Find(startingPoint);

        for (uint i = 0; i < Dim::actual_field_count; i++) {
            /* this check is necessary due to asymetric FU */
            if (IsFirst(_board[i]))
                aBoard.timesOfBeingOnShortestPath[i] += (short)(Find(_board[i]) == parent);
        }

    } else {
        /* FU is not performed for this player, BFS necessary */

        short queue[Dim::actual_field_count / 2 + 1];
        unsigned short visited[Dim::actual_field_count];
        short beg = 0, end = -1;

        memset(visited, 0, Dim::actual_field_count * sizeof(visited[0]));

        if (Player::First() == winner){
            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                      i < Dim::guard_count * Dim::down + Dim::board_size + Dim::guard_count;
                      i += Dim::right)
                if (IsFirst(_board[i])) {
                    queue[++end] = i;
                    visited[i] = Dim::actual_board_size;
                }

            for (; beg<=end; beg++)
                FOR_SIX (int dir)
                    if ((visited[queue[beg] + dir] == 0) && IsFirst(_board[queue[beg] + dir])){
                        queue[++end] = queue[beg] + dir;
                        visited[queue[end]] = visited[beg];
                    }

            for(uint i = Dim::down + Dim::guard_count;
                     i < Dim::down + Dim::board_size + Dim::guard_count;
                     i += Dim::right)
                visited[i] = 0;

            beg = 0;
            end = -1;
            for (uint i = (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count;
                      i < (Dim::board_size + Dim::guard_count - 1) * Dim::down + Dim::guard_count + Dim::board_size;
                      i += Dim::right)
                if (visited[i] > 0) {
                    queue[++end] = i;
                    aBoard.timesOfBeingOnShortestPath[i]++;
                    visited[i] = 0;
                }

            for(; beg <= end; beg++)
                FOR_SIX(int dir)
                    if (visited[queue[beg] + dir] > 0 && visited[queue[beg] + dir] < Dim::actual_board_size + Dim::board_size) {
                        queue[++end]=queue[beg] + dir;
                        visited[queue[end]] = 0;
                        aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                    }

        } else {

            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                      i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count;
                      i = i + Dim::down)
                if (IsSecond(_board[i])) {
                    queue[++end]=i;
                    visited[i]=1;
                }

            for (; beg <= end; beg++)
                FOR_SIX(int dir)
                    if((visited[queue[beg] + dir] == 0) && IsSecond(_board[queue[beg] + dir])){
                        queue[++end]=queue[beg] + dir;
                        visited[queue[end]] = visited[beg] + 1;
                    }

            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count - 1;
                      i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count - 1;
                      i = i + Dim::down)
                visited[i] = 0;

            beg=0;
            end=-1;
            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count - 1 + Dim::board_size;
                      i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count - 1 + Dim::board_size;
                      i = i + Dim::down)
                if (visited[i] > 0) {
                    queue[++end] = i;
                    aBoard.timesOfBeingOnShortestPath[i]++;
                    visited[i] = 0;
                }

            for(; beg <= end; beg++)
                FOR_SIX (int dir)
                    if (visited[queue[beg] + dir] > 0 && visited[queue[beg] + dir] < Dim::actual_board_size + Dim::board_size){
                        queue[++end] = queue[beg] + dir;
                        visited[queue[end]] = 0;
                        aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                    }
        }
    }
}

Player Board::CurrentPlayer() const {
    return _current;
}

Move Board::RandomLegalMove (const Player& player) const {

    return Move(player, _fast_field_map[Rand::next_rand(_moves_left)]);

}

Move Board::RandomLegalAvoidBridges (const Player& player) const {

    // _field_map_bound - last not-bridge
    // (_field_map_bound + 1) - first field after not-bridge fields == count of not-bridges
    // (_moves_left - _field_map_bound - 1) - bridges count

    uint rnd = Rand::next_rand((_field_map_bound + 1) * Params::bridgeWeight + _moves_left - _field_map_bound - 1);
    uint result;

    if (rnd >= static_cast<unsigned>(_field_map_bound + 1) * Params::bridgeWeight)
        result = rnd - (_field_map_bound + 1) * (Params::bridgeWeight - 1);
    else
        result = rnd / Params::bridgeWeight;

    return Move(player, _fast_field_map[result]);

}

void Board::PlayLegal (const Move& move) {

    ASSERT(IsValidMove(move));
    uint pos = move.GetLocation().GetPos();

    if (move.GetPlayer() == Player::First()) {
        _board[pos] = ToFirst(pos);
        MakeUnion(pos);
    } else {
        _board[pos] = ToSecond(pos);
        if (Switches::DetectWins())
            MakeUnion(pos);
    }

    // TODO: Add comments here.
    uint fast_map_pos = _reverse_fast_field_map[pos];
    uint replace_pos = _fast_field_map[--_moves_left];
    _fast_field_map[fast_map_pos] = replace_pos;
    _reverse_fast_field_map[replace_pos] = fast_map_pos;
    _current = _current.Opponent();

    if(Switches::IsAvoidingBridges() || Switches::IsDefendingBridges())
        UpdateBridgeData(pos,replace_pos);

}

void Board::UpdateBridgeData (uint pos, uint replace_pos) {

    _reverse_fast_field_map[pos] = _moves_left;            /*WTF?*/
    if (_field_map_bound >= static_cast<int>(_moves_left))
        _field_map_bound--;                                /*serio - WTF?*/

    UpdateBridgeBound(replace_pos);
    UpdateBridges(pos);
}

void Board::MakeUnion(uint pos) {
    uint rep = MakeUnion(pos, pos + Dim::right);
    rep = MakeUnion(rep, pos + Dim::left);
    rep = MakeUnion(rep, pos + Dim::upper_left);
    rep = MakeUnion(rep, pos + Dim::upper_right);
    rep = MakeUnion(rep, pos + Dim::lower_left);
    MakeUnion(rep, pos + Dim::lower_right);
}

uint Board::MakeUnion(uint pos1, uint pos2) {
    if (Switches::DetectWins()) {
        // TODO: Add comments here.
        return (IsEmpty(_board[pos2]) || IsSecond(_board[pos1]) != IsSecond(_board[pos2])) ? pos1 : ToPos(_board[pos1] = Find(pos2));
    } else {
        // TODO: Add comments here.
        return !IsFirst(_board[pos2]) ? pos1 : ToPos(_board[pos1] = Find(pos2));
    }
}

uint Board::Find(uint pos) {
    // TODO: Add comments here.
    while (static_cast<uint>(ToPos(_board[pos])) != pos)
        pos = ToPos(_board[pos] = _board[ToPos(_board[ToPos(_board[pos])])]);
    return _board[pos];
}

uint Board::ConstFind(uint pos) const {
    // TODO: Add comments here.
    while (static_cast<uint>(ToPos(_board[pos])) != pos)
        pos = ToPos(_board[pos]);
    return _board[pos];
}

bool Board::IsFull() const {
    return _moves_left == 0;
}

bool Board::IsWon() const {
    if (Switches::DetectWins())
        // TODO: ConstFind() or Find()?
        return ConstFind(root_up) == ConstFind(root_down) ||
               ConstFind(root_left) == ConstFind(root_right);
    else
        return IsFull();
}

Player Board::Winner() const {
    if (ConstFind(root_up) == ConstFind(root_down))
        return Player::First();
    else
        return Player::Second();
}

void Board::Load (const Board& board) {
    memcpy(this, &board, sizeof(*this));
}

Board::Board():
        _moves_left(Dim::field_count),
        _field_map_bound(Dim::field_count - 1),
        _current(Player::First()) {
    Rand::init(time(NULL));
}

uint Board::MovesLeft() const {
    return _moves_left;
}

void Board::GetPossiblePositions(Board::ushort_ptr& locations) {
    locations = _fast_field_map;
}

void Board::UpdateBridges(uint pos) {

    // TODO: Possible Switches:: use here.
    /*
    _field_bridge_connections[pos].Clear();
    _field_bridge_connections[pos + 1].Remove(pos);
    _field_bridge_connections[pos - 1].Remove(pos);
    _field_bridge_connections[pos - Dim::actual_board_size ].Remove(pos);
    _field_bridge_connections[pos - Dim::actual_board_size  + 1].Remove(pos);
    _field_bridge_connections[pos + Dim::actual_board_size ].Remove(pos);
    _field_bridge_connections[pos + Dim::actual_board_size  - 1].Remove(pos);
     */

    /*
     * instead of those seven lines above developed by krzysiocrash
     * 'while' by theolol is used.
     */

    // TODO: Add comments here.
    SmallSetIterator<pair<ushort,bool>, 3> it = _field_bridge_connections[pos].GetIterator();
    while (!it.IsEnd()){
        uint elem = (*it).first;

        if((*it).second ^ (_current!=Player::First())) /*I check owner of the bridge and if it fits*/
            attacked_bridges.Insert(elem);             /*I insert this bridge to attacked ones*/

        _field_bridge_connections[elem].Remove(pair<ushort,bool>(pos,(*it).second));
        it++;
    }

    short val = _board[pos];

    /*updating bridge structures - six cases*/

    // FIXME: This most likely does not work.
    // FIXME: Use IsFirst(), IsSecond() to find out about colours kept in fields.
    // TODO: It's absolutely essential to add some comments to this snippet.
    short second = _board[pos - 2 * Dim::actual_board_size + 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
                    && _board[pos - Dim::actual_board_size ] == 0
                    && _board[pos - Dim::actual_board_size + 1] == 0) {
        _field_bridge_connections[pos - Dim::actual_board_size ].Insert(
                pair<ushort,bool>(pos - Dim::actual_board_size + 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos - Dim::actual_board_size + 1].Insert(
                pair<ushort,bool>(pos - Dim::actual_board_size ,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos - Dim::actual_board_size  + 2];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos - Dim::actual_board_size  + 1] == 0 && _board[pos + 1] == 0) {
        _field_bridge_connections[pos - Dim::actual_board_size  + 1].Insert(pair<ushort,bool> (pos + 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos + 1].Insert(pair<ushort,bool>(pos - Dim::actual_board_size  + 1,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos + Dim::actual_board_size  + 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos + 1] == 0 && _board[pos + Dim::actual_board_size ] == 0) {
        _field_bridge_connections[pos + 1].Insert(pair<ushort,bool>(pos + Dim::actual_board_size ,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos + Dim::actual_board_size ].Insert(pair<ushort,bool>(pos + 1,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos + 2 * Dim::actual_board_size  - 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos + Dim::actual_board_size ] == 0 &&
            _board[pos + Dim::actual_board_size  - 1] == 0) {
        _field_bridge_connections[pos + Dim::actual_board_size ].Insert(
                pair<ushort,bool>(pos + Dim::actual_board_size  - 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos + Dim::actual_board_size  - 1].Insert(
                pair<ushort,bool>(pos + Dim::actual_board_size ,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos + Dim::actual_board_size  - 2];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos + Dim::actual_board_size  - 1] == 0 && _board[pos - 1] == 0) {
        _field_bridge_connections[pos + Dim::actual_board_size  - 1].Insert(pair<ushort,bool>(pos - 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos - 1].Insert(pair<ushort,bool>(pos + Dim::actual_board_size  - 1,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos - Dim::actual_board_size  - 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos - 1] == 0 &&
            _board[pos - Dim::actual_board_size ] == 0) {
        _field_bridge_connections[pos - 1].Insert(pair<ushort,bool>(pos - Dim::actual_board_size ,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos - Dim::actual_board_size ].Insert(pair<ushort,bool>(pos - 1,!(val >> SHORT_BIT_SIZE)));
    }

    // TODO: Use Dim:: constants here.
    UpdateBridgeBound(pos + 1);
    UpdateBridgeBound(pos - 1);
    UpdateBridgeBound(pos - Dim::actual_board_size );
    UpdateBridgeBound(pos - Dim::actual_board_size  + 1);
    UpdateBridgeBound(pos + Dim::actual_board_size );
    UpdateBridgeBound(pos + Dim::actual_board_size  - 1);

    /*crucial line from theolol:*/

    attacked_bridges.Remove(pos);

}

void Board::UpdateBridgeBound(uint pos) {
    // FIXME: This most likely does not work.
    // TODO: It's absolutely essential to comment this snippet.
    if (_field_bridge_connections[pos].Size() > 0) {
        if (_reverse_fast_field_map[pos] <= _field_map_bound) {
            ushort map_pos = _reverse_fast_field_map[pos];
            _fast_field_map[map_pos] = _fast_field_map[_field_map_bound];
            _fast_field_map[_field_map_bound--] = pos;

            _reverse_fast_field_map[pos] = _field_map_bound + 1;
            _reverse_fast_field_map[_fast_field_map[map_pos]] = map_pos;
        }
    } else if (_reverse_fast_field_map[pos] < _moves_left) {
        if (_reverse_fast_field_map[pos] > _field_map_bound) {
            ushort map_pos = _reverse_fast_field_map[pos];
            _fast_field_map[map_pos] = _fast_field_map[++_field_map_bound];
            _fast_field_map[_field_map_bound] = pos;

            _reverse_fast_field_map[pos] = _field_map_bound;
            _reverse_fast_field_map[_fast_field_map[map_pos]] = map_pos;
        }
    }
}

void Board::Show() const {
    std::cerr << ToAsciiArt(Location(1, 1));
}

std::string Board::ToAsciiArt(Location last_move) const {

    std::stringstream s;

    for (unsigned char x = 'a'; x < 'a' + Dim::board_size; ++x)
        s << " " << x;
    s << std::endl;
    for (uint i = 1; i <= Dim::board_size; ++i) {
        for (uint j = 1; j < (i < 10 ? i : i - 1); ++j)
            s << " ";
        s << i;
        if (Dim::ToPos(1, i) == last_move.GetPos())
            s << "(";
        else s << " ";
        for (uint j = 1; j <= Dim::board_size; ++j) {
            uint pos = Dim::ToPos(j, i);
            if (IsEmpty(_board[pos]))
                s << ".";
            else if (IsFirst(_board[pos]))
                s << "#";
            else
                s << "O";
            if (pos == last_move.GetPos())
                s << ")";
            else if (pos + 1 == last_move.GetPos())
                s << "(";
            else s << " ";
        }
        s << i << std::endl;
    }
    for (uint j = 0; j <= Dim::board_size; ++j)
        s << " ";
    for (unsigned char x = 'a'; x < 'a' + Dim::board_size; ++x)
        s << " " << x;

    s << std::endl << "Bridges:";
    for (unsigned i = static_cast<unsigned> (_field_map_bound + 1); i < _moves_left; i++)
        s << " " << Location(_fast_field_map[i]).ToCoords();

    return s.str();
}

bool Board::IsValidMove(const Move& move) {
    return (Location::ValidPosition(move.GetLocation().GetPos())) &&
          ((_moves_left % 2 == 0 && move.GetPlayer() == Player::Second()) ||
           (_moves_left % 2 == 1 && move.GetPlayer() == Player::First()));
}

Move Board::GenerateMoveUsingKnowledge(const Player& player) const {
    if (Switches::IsDefendingBridges())
        if (!attacked_bridges.Empty()) {
            Move m = Move(player, Location(attacked_bridges.RandomElem()));
            return m;
        }
    if (Switches::IsAvoidingBridges())
        return RandomLegalAvoidBridges(player);
    return RandomLegalMove(player);
}

// -----------------------------------------------------------------------------

} // namespace Hex
