#include "board.h"
#include "random.h"
#include "conditional_assert.h"
#include "boost/foreach.hpp"
#include "board_dim.h"
#include <cstring>
#include <sstream>
#include <iostream>

namespace Hex {

#define FOR_SIX(x)  BOOST_FOREACH(x, Dim::dirs)
#define FOR_SIX2(x)  BOOST_FOREACH(x, Dim::dirs2)


uint Board::ToPos(ushort val) {
    if (Switches::DetectWins())
        return val & ~board_second;
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
        return board_second + 1;
}

uint Board::BridgeToPos(ushort val) {
    return val & ~board_second;
}

ushort Board::ToBridge(ushort pos, ushort owner) {
    return pos | (owner & board_second);
}

bool Board::IsFirst(ushort val) {
    return !((val - 1) & board_second);
}

bool Board::IsSecond(ushort val) {
    return val & board_second;
}

bool Board::IsEmpty(ushort val) {
    return val == 0;
}

bool Board::SamePlayer(ushort val, ushort val2) {
    return (val & board_second) == (val2 & board_second) && val2 != 0;
}

bool Board::OppPlayer(ushort val, ushort val2) {
    return (val & board_second) ^ (val2 & board_second);
}

const Board Board::Empty() {

    Board board;

    uint counter = 0;
    for (uint i = 0; i < Dim::actual_field_count; ++i)
        board.rev_map[i] = -1;
    for (uint i = 1; i <= Dim::board_size; ++i) {
        for (uint j = 1; j <= Dim::board_size; ++j) {
            signed int field = Dim::OfPos(i, j);
            board.field_map[counter] = field;
            board.rev_map[field] = counter++;
        }
    }

    for (uint i = 0; i < Dim::actual_field_count; ++i)
        board.board[i] = board_empty;
    for (uint i = 1; i <= Dim::board_size; ++i) {
        board.board[Dim::OfPos(i, 0)] = ToFirst(root_up);
        board.board[Dim::OfPos(i, Dim::board_size + 1)] = ToFirst(root_down);
        board.board[Dim::OfPos(0, i)] = ToSecond(root_left);
        board.board[Dim::OfPos(Dim::board_size + 1, i)] = ToSecond(root_right);
    }

    board.ClearShortestPathsStats();

    return board;
}

void Board::ClearShortestPathsStats() {

    for(uint i = 0; i < Dim::actual_field_count; i++)
        timesOfBeingOnShortestPath[i]=0;
}


void Board::UpdatePathsStatsAllShortestPathsBFS(Board& aBoard, const Player& winner) {

    // bfs queue
    short queue[Dim::actual_field_count / 2 + 1];
    // marks in which step node was visited
    unsigned short visited[Dim::actual_field_count];
    // queue markings
    short beg = 0, end = -1;

    memset(visited, 0, Dim::actual_field_count * sizeof(visited[0]));

    if (Player::First() == winner) {
        for (uint i = OfPos(1,1); i <= OfPos(Dim::board_size,1); i = i + Dim::right)
            if (IsFirst(board[i])) {
                // I put to queue every node by the edge
                queue[++end]=i;
                //and mark it as visited in first step
                visited[i]=1;
            }

        /*
         * Standard BFS:
         * I try to reach the other edge of the board using nodes
         * that belong to the first player. I know that is possible because
         * first player is the winner. For each visited guy I count length of
         * a shotrest path leading from first edge to him.
         */
        for (; beg <= end; ++beg)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsFirst(board[queue[beg] + dir])){
                    queue[++end] = queue[beg] + dir;
                    visited[queue[end]] = visited[queue[beg]] + 1;
                }

        /*
         * Now I have to find the shortest paths.
         * I find out which nodes by the other edge were visited as first ones
         */
        uint min = (uint) - 1;
        for (uint i = OfPos(1,Dim::board_size); i <= OfPos(Dim::board_size,Dim::board_size); i = i + Dim::right)
            if(visited[i] > 0 && visited[i] < min)
                min = visited[i];

        // cleaning queue
        beg = 0;
        end = -1;
        /*
         * Have to be prepared for second BFS. It shall start from the other edge
         * and shall be done backwards. We add to the queue neighbor(actual) if
         * player(actual)==player(neighbor(actual)) and if visited(actual)==visited(neighbor(actual))+1.
         * It guarantee that we visit only those nodes that are placed on a shortest path.
         * actual_mins says "how much nodes are waiting in queue to be expand and visited(those nodes)
         * was equal to min before it has been zeroed"
         * next_mins says "how much nodes are waiting in queue to be expand and visited(those nodes)
         * was equal to (min-1) before it has been zeroed"
         * I need those two variables to get know when decrease min.
         */
        int actual_mins = 0;
        int next_mins = 0;
        for (uint i = OfPos(1,Dim::board_size); i <= OfPos(Dim::board_size,Dim::board_size); i = i + Dim::right)
            if (visited[i] == min){
                queue[++end] = i;
                aBoard.timesOfBeingOnShortestPath[i]++;
                visited[i] = 0;
                actual_mins++;
            }

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
        /*
         * Analogically for the other player marked with values < 0
         * there are changes in 'for' statements (vertically, not horizontally)
         * and in checking if player is second, not first
         */

        for (uint i = OfPos(1,1); i <= OfPos(1,Dim::board_size); i = i + Dim::down)
            if (IsSecond(board[i])) {
                queue[++end] = i;
                visited[i] = 1;
            }

        for(; beg <= end; beg++)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsSecond(board[queue[beg] + dir])){
                    queue[++end] = queue[beg] + dir;
                    visited[queue[end]] = visited[queue[beg]] + dir;
                }

        uint min = (uint) -1;
        for (uint i = OfPos(Dim::board_size,1); i <= OfPos(Dim::board_size,Dim::board_size); i = i + Dim::down)
            if (visited[i] > 0 && visited[i] < min)
                min = visited[i];

        beg = 0;
        end = -1;
        int actual_mins = 0;
        int next_mins = 0;
        for (uint i = OfPos(Dim::board_size,1); i <= OfPos(Dim::board_size,Dim::board_size); i = i + Dim::down)
            if (visited[i] == min) {
                queue[++end] = i;
                aBoard.timesOfBeingOnShortestPath[i]++;
                visited[i] = 0;
                actual_mins++;
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

    // FIXME: Please merge both branches of this if into a single code snippet.
    // They are both very similar.
    if (Player::First() == winner) {
        // FIXME: Use OfPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                  i < Dim::guard_count * Dim::down + Dim::board_size + Dim::guard_count;
                  i += Dim::right)
            if (IsFirst(board[i])) {
                queue[++end] = i;
                visited[i] = 1;
            }

        for (; beg<=end; beg++)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsFirst(board[queue[beg] + dir])){
                    queue[++end] = queue[beg] + dir;
                    visited[queue[end]] = visited[beg] + 1;
                }

        // avoiding warnings
        uint current = 0;
        uint min = (uint) -1;
        // FIXME: Use OfPos() in order to aquire appropriate iteration range.
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

        // FIXME: Use OfPos() in order to aquire appropriate iteration range.
        for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                  i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count;
                  i = i + Dim::down)
            if (IsSecond(board[i])) {
                queue[++end] = i;
                visited[i] = 1;
            }

        for(; beg <= end; beg++)
            FOR_SIX(int dir)
                if ((visited[queue[beg] + dir] == 0) && IsSecond(board[queue[beg] + dir])) {
                    queue[++end]=queue[beg] + dir;
                    visited[queue[end]] = visited[beg] + 1;
                }

        //avoiding warnings
        uint current = 0;
        uint min = (uint) -1;
        // FIXME: Use OfPos() in order to aquire appropriate iteration range.
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
    /* board[(guardedboard_size - 2) * Dim::actualboard_size  + 2] is always a guard of a first type */
    /* board[2 * Dim::actualboard_size  + 1] is always a guard of a second type */

    // Two-directional F&U is required for the procedure to function.
    ASSERT(Switches::DetectWins());

    int startingPoint;
    uint parent;

    if (Player::First() == winner)
        startingPoint = (Dim::guardedboard_size - 2) * Dim::down  + 2;
    else
        startingPoint = 2 * Dim::down + 1;

    parent = Find(startingPoint);

    for(uint i = 0; i < Dim::actual_field_count; i++)
        aBoard.timesOfBeingOnShortestPath[i] += (short) (Find(board[i]) == parent);
}

void Board::ShowPathsStats() {

    cerr << "SHOREST PATHS STATS:\n";

    for (uint i = 0; i < Dim::actualboard_size; i++){
        for (uint j = 0; j < Dim::actualboard_size; j++)
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

    // FIXME: The second branch can be optimized in a manner similar to the first
    // one in a situation where symmetric F&U is maintained.
    if (Player::First() == winner) {
        int startingPoint;
        uint parent;

        startingPoint = (Dim::guardedboard_size - 2) * Dim::actualboard_size + 2;

        parent = Find(startingPoint);

        for (uint i = 0; i < Dim::actual_field_count; i++) {
            /* this check is necessary due to asymetric FU */
            if (IsFirst(board[i]))
                aBoard.timesOfBeingOnShortestPath[i] += (short)(Find(board[i]) == parent);
        }

    } else {
        /* FU is not performed for this player, BFS necessary */

        short queue[Dim::actual_field_count / 2 + 1];
        unsigned short visited[Dim::actual_field_count];
        short beg = 0, end = -1;

        memset(visited, 0, Dim::actual_field_count * sizeof(visited[0]));

        if (Player::First() == winner){
            // FIXME: Use OfPos() in order to aquire appropriate iteration range.
            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                      i < Dim::guard_count * Dim::down + Dim::board_size + Dim::guard_count;
                      i += Dim::right)
                if (IsFirst(board[i])) {
                    queue[++end] = i;
                    visited[i] = Dim::actualboard_size;
                }

            for (; beg<=end; beg++)
                FOR_SIX (int dir)
                    if ((visited[queue[beg] + dir] == 0) && IsFirst(board[queue[beg] + dir])){
                        queue[++end] = queue[beg] + dir;
                        visited[queue[end]] = visited[beg];
                    }

            for(uint i = Dim::down + Dim::guard_count;
                     i < Dim::down + Dim::board_size + Dim::guard_count;
                     i += Dim::right)
                visited[i] = 0;

            beg = 0;
            end = -1;
            // FIXME: Use OfPos() in order to aquire appropriate iteration range.
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
                    if (visited[queue[beg] + dir] > 0 && visited[queue[beg] + dir] < Dim::actualboard_size + Dim::board_size) {
                        queue[++end]=queue[beg] + dir;
                        visited[queue[end]] = 0;
                        aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                    }

        } else {

            // FIXME: Use OfPos() in order to aquire appropriate iteration range.
            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count;
                      i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count;
                      i = i + Dim::down)
                if (IsSecond(board[i])) {
                    queue[++end]=i;
                    visited[i]=1;
                }

            for (; beg <= end; beg++)
                FOR_SIX(int dir)
                    if((visited[queue[beg] + dir] == 0) && IsSecond(board[queue[beg] + dir])){
                        queue[++end]=queue[beg] + dir;
                        visited[queue[end]] = visited[beg] + 1;
                    }

            // FIXME: Use OfPos() in order to aquire appropriate iteration range.
            for (uint i = Dim::guard_count * Dim::down + Dim::guard_count - 1;
                      i < (Dim::board_size + Dim::guard_count) * Dim::down + Dim::guard_count - 1;
                      i = i + Dim::down)
                visited[i] = 0;

            beg=0;
            end=-1;
            // FIXME: Use OfPos() in order to aquire appropriate iteration range.
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
                    if (visited[queue[beg] + dir] > 0 && visited[queue[beg] + dir] < Dim::actualboard_size + Dim::board_size){
                        queue[++end] = queue[beg] + dir;
                        visited[queue[end]] = 0;
                        aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                    }
        }
    }
}

Player Board::CurrentPlayer() const {
    return current;
}

Move Board::RandomLegalMove (const Player& player) const {

    return Move(player, field_map[Rand::next_rand(moves_left)]);

}

Move Board::RandomLegalAvoidBridges (const Player& player) const {

    // bridge_range - last not-bridge
    // (bridge_range + 1) - first field after not-bridge fields == count of not-bridges
    // (moves_left - bridge_range - 1) - bridges count

    uint rnd = Rand::next_rand((bridge_range + 1) * Params::bridgeWeight + moves_left - bridge_range - 1);
    uint result;

    if (rnd >= static_cast<unsigned>(bridge_range + 1) * Params::bridgeWeight)
        result = rnd - (bridge_range + 1) * (Params::bridgeWeight - 1);
    else
        result = rnd / Params::bridgeWeight;

    return Move(player, field_map[result]);

}

void Board::SwapFree(uint index, uint index2) {
    uint x = field_map[index];
    field_map[index] = field_map[index2];
    field_map[index2] = x;

    rev_map[field_map[index]] = index;
    rev_map[field_map[index2]] = index2;
}

void Board::MoveFree(uint pos, uint index) {

    // The index in the field map containing pos before the update.
    uint fast_map_pos = rev_map[pos];
    SwapFree(fast_map_pos, index);
}

uint Board::RemoveFree(uint pos) {

    // The index in the field map containing pos before the update.
    uint fast_map_pos = rev_map[pos];
    // The new position to put instead of pos in the field map. It is the last
    // position in the array, which enables us to trim the array by one position.
    uint replace_pos = field_map[--moves_left];

    // Replacing pos with replace_pos in the field map.
    field_map[fast_map_pos] = replace_pos;
    // Now, new replace_pos index inside field map has to be updated in the
    // reverse field map.
    rev_map[replace_pos] = fast_map_pos;

    return replace_pos;
}

void Board::PlayLegal(const Move& move) {

    ASSERT(IsValidMove(move));

    current = current.Opponent();
    uint pos = move.GetLocation().GetPos();

    if (move.GetPlayer() == Player::First()) {
        board[pos] = ToFirst(pos);
        MakeUnion(pos);
    } else {
        board[pos] = ToSecond(pos);
        // No F&U is maintained for the second player if win detection is not
        // required.
        if (Switches::DetectWins())
            MakeUnion(pos);
    }

    uint replace_pos = RemoveFree(pos);

    if(Switches::IsAvoidingBridges() || Switches::IsDefendingBridges())
        UpdateBridgeData(pos, replace_pos);
}

void Board::UpdateBridgeData (uint pos, uint replace_pos) {

    // Ensure that bridge_range points to some valid free position within
    // the reverse field map.
    if (bridge_range >= static_cast<int>(moves_left))
        bridge_range--;

    UpdateBridgeBound(replace_pos);
    UpdateBridges(pos);

    // Set the value stored in reverse field map for pos as invalid (?).
    rev_map[pos] = moves_left;
}

void Board::MakeUnion(uint pos) {
    // Perform unions with neighbouring fields as needed.
    uint rep = MakeUnion(pos, pos + Dim::right);
    rep = MakeUnion(rep, pos + Dim::left);
    rep = MakeUnion(rep, pos + Dim::upper_left);
    rep = MakeUnion(rep, pos + Dim::upper_right);
    rep = MakeUnion(rep, pos + Dim::lower_left);
    MakeUnion(rep, pos + Dim::lower_right);
}

uint Board::MakeUnion(uint pos1, uint pos2) {
    if (Switches::DetectWins()) {
        // Two F&U trees are maintained. If pos2 is non-empty and pos, pos2 agree in
        // colour - perform a union.
        return (IsEmpty(board[pos2]) || IsSecond(board[pos1]) != IsSecond(board[pos2])) ? pos1 : ToPos(board[pos1] = Find(pos2));
    } else {
        // A single F&U tree is maintained for the first player. If pos and pos2
        // both belong to the first player, perform a union.
        return !IsFirst(board[pos2]) ? pos1 : ToPos(board[pos1] = Find(pos2));
    }
}

uint Board::Find(uint pos) const {
    // Simplified quasi-F&U implementation. In each step of finding root, link
    // node's grandfather and the node itself with the same father node.
    while (static_cast<uint>(ToPos(board[pos])) != pos)
        pos = ToPos(board[pos] = board[ToPos(board[ToPos(board[pos])])]);
    return board[pos];
}

uint Board::ConstFind(uint pos) const {
    // Perform a non modifying F&U search.
    while (static_cast<uint>(ToPos(board[pos])) != pos)
        pos = ToPos(board[pos]);
    return board[pos];
}

bool Board::IsFull() const {
    return moves_left == 0;
}

bool Board::IsWon() const {
    if (Switches::DetectWins())
        return Find(root_up) == Find(root_down) ||
               Find(root_left) == Find(root_right);
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
        moves_left(Dim::field_count),
        bridge_range(Dim::field_count - 1),
        current(Player::First()) {
    Rand::init(time(NULL));
}

uint Board::MovesLeft() const {
    return moves_left;
}

void Board::GetPossiblePositions(Board::ushort_ptr& locations) {
    locations = field_map;
}

void Board::UpdateBridges(uint pos) {

    ASSERT(!IsEmpty(board[pos]));

    ushort val = board[pos];

    // Remove known bridges with position pos.
    // Iterate over all such bridged positions kept in a set.
    SmallSetIterator<ushort, 3> it = bridge_conn[pos].GetIterator();
    for (; !it.IsEnd(); ++it) {
        uint br = BridgeToPos(*it);
        // Check if the bridge owner and the new pawn owner are different.
        if (Switches::IsDefendingBridges()) {
            if (OppPlayer(*it, val))
                // An attacked bridge has been detected.
                attacked_bridges.Insert(br);
        }

        // Remove the bridge from the neighbouring field.
        bridge_conn[br].Remove(ToBridge(pos, *it));
    }

    if (Switches::IsDefendingBridges()) {
        // As the bridges inside the pos are no longer valid remove it also from
        // the attacked bridges list.
        attacked_bridges.Remove(pos);
    }

    // Detect bridges around position pos.
    FOR_SIX(int dir) {
        // A candidate for the other side of a bridge to be detected.
        uint pos2 = pos + dir + Dim::Clockwise(dir);
        // Candidates for bridged fields
        uint br = pos + dir;
        uint br2 = pos + Dim::Clockwise(dir);
        // Test if the candidates are valid.
        if (IsEmpty(board[br]) && IsEmpty(board[br2]) && SamePlayer(board[pos], board[pos2])) {
            // Add both bridged fields to approrpriate sets.
            bridge_conn[br].Insert(ToBridge(br2, val));
            bridge_conn[br2].Insert(ToBridge(br, val));
        }
    }

    // Validate the order in the field maps.
    FOR_SIX(int dir)
        UpdateBridgeBound(pos + dir);
}

void Board::UpdateBridgeBound(uint pos) {
    if (bridge_conn[pos].Size() > 0) {
        // A bridge field might be stored outside the bridge range.
        if (rev_map[pos] <= bridge_range)
            // Move the bridge field inside the bridge range and extend the
            // range.
            MoveFree(pos, bridge_range--);
    } else if (rev_map[pos] < moves_left) {
        // A nonbridge field might be stored inside the bridge range.
        if (rev_map[pos] > bridge_range)
            // Move the nonbridge field outside the bridge range and shrink
            // the range.
            MoveFree(pos, ++bridge_range);
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
        if (Dim::OfPos(1, i) == last_move.GetPos())
            s << "(";
        else s << " ";
        for (uint j = 1; j <= Dim::board_size; ++j) {
            uint pos = Dim::OfPos(j, i);
            if (IsEmpty(board[pos]))
                s << ".";
            else if (IsFirst(board[pos]))
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
    for (unsigned i = static_cast<unsigned> (bridge_range + 1); i < moves_left; i++)
        s << " " << Location(field_map[i]).ToCoords();

    return s.str();
}

bool Board::IsValidMove(const Move& move) {
    return (Location::ValidPosition(move.GetLocation().GetPos())) &&
            IsEmpty(board[move.GetLocation().GetPos()]) &&
          ((moves_left % 2 == 0 && move.GetPlayer() == Player::Second()) ||
           (moves_left % 2 == 1 && move.GetPlayer() == Player::First()));
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
