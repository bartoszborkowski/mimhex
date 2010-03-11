/*uber*/

#include "board.h"
#include "random.h"
#include "conditional_assert.h"
#include <cstring>
#include <sstream>
#include <iostream>

#define SHORT_BIT_SIZE (sizeof(short) * 8)

namespace Hex {

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

Location::Location(uint x, uint y) : _pos(ToTablePos(x, y)) { }
Location::Location(uint pos) : _pos(pos) {}

Location::Location() {}

uint Location::GetPos() const { return _pos; }

uint Location::GetX() const { return _pos % actual_board_size - 1; }

uint Location::GetY() const { return _pos / actual_board_size - 1; }

std::string Location::ToCoords() const {
    std::stringstream coords;
    coords << static_cast<char>(_pos % actual_board_size + 'a' - 2);
    coords << _pos / actual_board_size - 1;
    return coords.str();
}

uint Location::ToTablePos(uint x, uint y) {
    ASSERT (ValidLocation(x, y));
    return (++y) * actual_board_size + x + 1;
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
    uint x, y;
    ToCoords(pos, x, y);
    return ValidLocation(x, y);
}

bool Location::ValidLocation(uint x, uint y) {
    return x > 0 && y > 0 && x <= board_size && y <= board_size;
}

void Location::ToCoords(uint pos, uint& x, uint& y) {
    x = pos % actual_board_size  - 1;
    y = pos / actual_board_size  - 1;
}

// -----------------------------------------------------------------------------

Move::Move (const Player& player, const Location& location)
    : _player(player), _location(location) {}

Move::Move() :_player(Player::First()), _location(0) {}

Location Move::GetLocation() const { return _location; }

Player Move::GetPlayer() const { return _player; }

// -----------------------------------------------------------------------------

uint Board::ToPos(int x, int y) {
    return (x + guard_count - 1) + (y + guard_count - 1) * actual_board_size;
}

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
    for (uint i = 0; i < actual_field_count; ++i)
        board._reverse_fast_field_map[i] = -1;
    for (uint i = 1; i <= board_size; ++i) {
        for (uint j = 1; j <= board_size; ++j) {
            signed int field = ToPos(i, j);
            board._fast_field_map[counter] = field;
            board._reverse_fast_field_map[field] = counter++;
        }
    }

    for (uint i = 0; i < actual_field_count; ++i)
        board._board[i] = board_empty;
    for (uint i = 1; i <= board_size; ++i) {
        board._board[ToPos(i, 0)] = ToFirst(root_up);
        board._board[ToPos(i, board_size + 1)] = ToFirst(root_down);
        board._board[ToPos(0, i)] = ToSecond(root_left);
        board._board[ToPos(board_size + 1, i)] = ToSecond(root_right);
    }

    board.clearShortestPathsStats();

    return board;
}

void Board::clearShortestPathsStats() {

    for(uint i=0; i<actual_field_count; i++)
        timesOfBeingOnShortestPath[i]=0;
}

void Board::UpdatePathsStatsAllShortestPathsBFS(Board& aBoard, const Player& winner) {

    short queue[actual_field_count / 2 + 1];    //bfs queue
    unsigned short visited[actual_field_count];    //marks in which step node was visited
    short beg=0, end=-1;        //queue markings

    memset(visited, 0, actual_field_count * sizeof(visited[0]));

    if (Player::First() == winner) {
        for (uint i = guard_count * actual_board_size + guard_count; i < guard_count * actual_board_size + board_size + guard_count; i++)
            if (IsFirst(_board[i])){
                queue[++end]=i;        //I put to queue every node by the edge
                visited[i]=1;        //and mark it as visited in first step
            }
        for (; beg<=end; beg++){            //bfs
            if ((visited[queue[beg]+1] == 0) && IsFirst(_board[queue[beg]+1])>0){
                queue[++end]=queue[beg]+1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if ((visited[queue[beg]-1] == 0) && IsFirst(_board[queue[beg]-1])>0){
                queue[++end]=queue[beg]-1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if ((visited[queue[beg]-actual_board_size +1] == 0) && IsFirst(_board[queue[beg]-actual_board_size+1])){
                queue[++end]=queue[beg]-actual_board_size +1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if ((visited[queue[beg]+actual_board_size -1] == 0) && IsFirst(_board[queue[beg]+actual_board_size-1])){
                queue[++end]=queue[beg]+actual_board_size -1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if ((visited[queue[beg]+actual_board_size ] == 0) && IsFirst(_board[queue[beg]+actual_board_size])){
                queue[++end]=queue[beg]+actual_board_size ;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if ((visited[queue[beg]-actual_board_size ] == 0) && IsFirst(_board[queue[beg]-actual_board_size])){
                queue[++end]=queue[beg]-actual_board_size ;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
        }

        /*Now I have to find the shortest paths.
          I find out which nodes by the other edge were visited as first ones*/
        uint min = (uint) -1;
        for (uint i = (board_size + guard_count - 1) * actual_board_size  + guard_count;
                i < (board_size + guard_count - 1) * actual_board_size  + guard_count + board_size; ++i) {
            if(visited[i] > 0 && visited[i] < min)
                min = visited[i];
        }
        beg=0;    //cleaning queue
        end=-1;
	int actual_mins=0;
	int next_mins=0;
        for (uint i = (board_size + guard_count - 1) * actual_board_size  + guard_count;
                i < (board_size + guard_count - 1) * actual_board_size  + guard_count + board_size; ++i)
            if(visited[i]==min){
                queue[++end]=i;
                aBoard.timesOfBeingOnShortestPath[i]++;
                visited[i]=0;
		actual_mins++;
            }

        for(; beg<=end; beg++){        //second bfs done backwards
            if(visited[queue[beg]+1] == min-1 && visited[queue[beg]+1]){
		next_mins++;
                queue[++end]=queue[beg]+1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]-1] == min-1 && visited[queue[beg]-1]){
		next_mins++;
                queue[++end]=queue[beg]-1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]-actual_board_size+1] == min-1 && visited[queue[beg]-actual_board_size+1]){
		next_mins++;
                queue[++end]=queue[beg]-actual_board_size +1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]+actual_board_size-1] == min-1 && visited[queue[beg]+actual_board_size-1]){
		next_mins++;
                queue[++end]=queue[beg]+actual_board_size -1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]+actual_board_size] == min-1 && visited[queue[beg]+actual_board_size]){
		next_mins++;
                queue[++end]=queue[beg]+actual_board_size ;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]-actual_board_size] == min-1 && visited[queue[beg]-actual_board_size]){
		next_mins++;
                queue[++end]=queue[beg]-actual_board_size ;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
		if(--actual_mins==0){
			actual_mins=next_mins;
			next_mins=0;
			min--;
		}
        }
    }else{    //analogically for the other player marked with values < 0
        //there are changes in 'for' statements (vertically, not horizontally)
        //and in comparisions to 0 (lesser, not greater)
        for(uint i=guard_count * actual_board_size + guard_count;
		i<(board_size+guard_count)*actual_board_size + guard_count; i=i+actual_board_size )
            if(IsSecond(_board[i])){
                queue[++end]=i;
                visited[i]=1;
            }
        for(; beg<=end; beg++){
            if((visited[queue[beg]+1] == 0) && IsSecond(_board[queue[beg]+1])){
                queue[++end]=queue[beg]+1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if((visited[queue[beg]-1] == 0) && IsSecond(_board[queue[beg]-1])){
                queue[++end]=queue[beg]-1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if((visited[queue[beg]-actual_board_size +1] == 0) && IsSecond(_board[queue[beg]-actual_board_size +1])){
                queue[++end]=queue[beg]-actual_board_size +1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if((visited[queue[beg]+actual_board_size -1] == 0) && IsSecond(_board[queue[beg]+actual_board_size -1])){
                queue[++end]=queue[beg]+actual_board_size -1;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if((visited[queue[beg]+actual_board_size ] == 0) && IsSecond(_board[queue[beg]+actual_board_size])){
                queue[++end]=queue[beg]+actual_board_size ;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
            if((visited[queue[beg]-actual_board_size ] == 0) && IsSecond(_board[queue[beg]-actual_board_size])){
                queue[++end]=queue[beg]-actual_board_size ;
                visited[queue[end]] = visited[queue[beg]]+1;
            }
        }

        uint min = (uint) -1;
        for (uint i = guard_count * actual_board_size + guard_count + board_size - 1;
                i < (board_size + guard_count - 1) * actual_board_size + guard_count + board_size - 1; i=i+actual_board_size )
            if(visited[i]>0 && visited[i]<min)
                min = visited[i];

        beg=0;
        end=-1;
	int actual_mins=0;
	int next_mins=0;
        for (uint i = guard_count * actual_board_size + guard_count + board_size - 1;
                i < (board_size + guard_count - 1) * actual_board_size + guard_count + board_size - 1; i=i+actual_board_size ) {
            if(visited[i]==min){
                queue[++end]=i;
                aBoard.timesOfBeingOnShortestPath[i]++;
                visited[i]=0;
		actual_mins++;
	    }
        }

        for(; beg<=end; beg++){
            if(visited[queue[beg]+1] == min-1 && visited[queue[beg]+1]){
		next_mins++;
                queue[++end]=queue[beg]+1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]-1] == min-1 && visited[queue[beg]-1]){
		next_mins++;
                queue[++end]=queue[beg]-1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]-actual_board_size+1] == min-1 && visited[queue[beg]-actual_board_size+1]){
		next_mins++;
                queue[++end]=queue[beg]-actual_board_size+1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]+actual_board_size-1] == min-1 && visited[queue[beg]+actual_board_size-1]){
		next_mins++;
                queue[++end]=queue[beg]+actual_board_size-1;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]+actual_board_size] == min-1 && visited[queue[beg]+actual_board_size]){
		next_mins++;
                queue[++end]=queue[beg]+actual_board_size;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }
            if(visited[queue[beg]-actual_board_size] == min-1 && visited[queue[beg]-actual_board_size]){
		next_mins++;
                queue[++end]=queue[beg]-actual_board_size;
                visited[queue[end]] = 0;
                aBoard.timesOfBeingOnShortestPath[queue[end]]++;
            }

		if(--actual_mins==0){
			actual_mins=next_mins;
			next_mins=0;
			min--;
		}
        }
    }
}


void Board::UpdatePathsStatsOneShortestPathBFS(Board& aBoard, const Player& winner) {

    short queue[actual_field_count/2+1];
    unsigned short visited[actual_field_count];
    short beg=0, end=-1;

    memset(visited,0,actual_field_count*sizeof(visited[0]));

    if (Player::First() == winner){
        for(uint i=2*actual_board_size +2; i<2*actual_board_size +board_size+2; i++)
            if(IsFirst(_board[i])){
                queue[++end]=i;
                visited[i]=1;
            }
        for(; beg<=end; beg++){
            if((visited[queue[beg]+1] == 0) && IsFirst(_board[queue[beg]+1])){
                queue[++end]=queue[beg]+1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]-1] == 0) && IsFirst(_board[queue[beg]-1])){
                queue[++end]=queue[beg]-1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]-actual_board_size +1] == 0) && IsFirst(_board[queue[beg]-actual_board_size +1])){
                queue[++end]=queue[beg]-actual_board_size +1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]+actual_board_size -1] == 0) && IsFirst(_board[queue[beg]+actual_board_size -1])){
                queue[++end]=queue[beg]+actual_board_size -1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]+actual_board_size ] == 0) && IsFirst(_board[queue[beg]+actual_board_size])){
                queue[++end]=queue[beg]+actual_board_size ;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]-actual_board_size ] == 0) && IsFirst(_board[queue[beg]-actual_board_size])){
                queue[++end]=queue[beg]-actual_board_size ;
                visited[queue[end]] = visited[beg]+1;
            }
        }
        uint current=0; //avoiding warnings
        uint min = (uint) -1;
        for (uint i = (board_size+1) * actual_board_size  + 2;
                i < (board_size+1) * actual_board_size  + 2 + board_size; ++i) {
            if(visited[i] > 0 && visited[i] < min){
                min = visited[i];
                current = i;
            }
        }
        aBoard.timesOfBeingOnShortestPath[current]++;

        for(;;){
            if(visited[current+1] == visited[current]-1){
                current = current + 1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current-1] == visited[current]-1){
                current = current - 1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current-actual_board_size +1] == visited[current]-1){
                current = current-actual_board_size +1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current+actual_board_size -1] == visited[current]-1){
                current = current+actual_board_size -1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current+actual_board_size ] == visited[current]-1){
                current = current+actual_board_size ;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current-actual_board_size ] == visited[current]-1){
                current = current-actual_board_size ;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            break;
        }
    }else{
        for(uint i=2*actual_board_size +2; i<(board_size+2)*actual_board_size +2; i=i+actual_board_size )
            if(IsSecond(_board[i])){
                queue[++end]=i;
                visited[i]=1;
            }
        for(; beg<=end; beg++){
            if((visited[queue[beg]+1] == 0) && IsSecond(_board[queue[beg]+1])){
                queue[++end]=queue[beg]+1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]-1] == 0) && IsSecond(_board[queue[beg]-1])){
                queue[++end]=queue[beg]-1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]-actual_board_size +1] == 0) && IsSecond(_board[queue[beg]-actual_board_size +1])){
                queue[++end]=queue[beg]-actual_board_size +1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]+actual_board_size -1] == 0) && IsSecond(_board[queue[beg]+actual_board_size -1])){
                queue[++end]=queue[beg]+actual_board_size -1;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]+actual_board_size ] == 0) && IsSecond(_board[queue[beg]+actual_board_size])){
                queue[++end]=queue[beg]+actual_board_size ;
                visited[queue[end]] = visited[beg]+1;
            }
            if((visited[queue[beg]-actual_board_size ] == 0) && IsSecond(_board[queue[beg]-actual_board_size])){
                queue[++end]=queue[beg]-actual_board_size ;
                visited[queue[end]] = visited[beg]+1;
            }
        }
        uint current=0; //avoiding warnings
        uint min = (uint) -1;
        for (uint i = 2 * actual_board_size  + 1 + board_size;
                i < (board_size+1) * actual_board_size + 2 + board_size; i=i+actual_board_size ) {
            if(visited[i]>0 && visited[i]<min){
                min = visited[i];
                current = i;
            }
        }
        aBoard.timesOfBeingOnShortestPath[current]++;
        for(;;){
            if(visited[current+1] == visited[current]-1){
                current = current + 1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current-1] == visited[current]-1){
                current = current - 1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current-actual_board_size +1] == visited[current]-1){
                current = current-actual_board_size +1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current+actual_board_size -1] == visited[current]-1){
                current = current+actual_board_size -1;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current+actual_board_size ] == visited[current]-1){
                current = current+actual_board_size ;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            if(visited[current-actual_board_size ] == visited[current]-1){
                current = current-actual_board_size ;
                aBoard.timesOfBeingOnShortestPath[current]++;
                continue;
            }
            break;
        }
    }

}

/* this method assumes symetric FU! */
//FIXME test it after implementing symetric FU!
void Board::UpdatePathsStatsFloodFillFU(Board& aBoard, const Player& winner){
    /* first uses positive numbers, second -1s */
    /* _board[(guarded_board_size - 2) * actual_board_size  + 2] is always a guard of a first type */
    /* _board[2 * actual_board_size  + 1] is always a guard of a second type */

    int startingPoint;
    uint parent;

    if (Player::First() == winner){
        startingPoint = (guarded_board_size - 2) * actual_board_size  + 2;
    }
    else{
        startingPoint = 2 * actual_board_size  + 1;
    }

    parent = Find(startingPoint);

    for(uint i=0; i<actual_field_count; i++){
        aBoard.timesOfBeingOnShortestPath[i] += (short)(Find(_board[i]) == parent);
    }
}

void Board::ShowPathsStats()
{

	cerr<<"SHOREST PATHS STATS:\n";

	for(uint i=0; i<kBoardSizeAligned; i++){
		for(uint j=0; j<kBoardSizeAligned; j++)
			cerr<<" "<<timesOfBeingOnShortestPath[i*kBoardSizeAligned+j]<<" ";
		cerr<<"\n";
	}

	cerr<<"END OF SHOREST PATHS STATS\n";


}

/* this method assumes asymetric FU! */
void Board::UpdatePathsStatsFloodFillBFS(Board& aBoard, const Player& winner){

    if (Player::First() == winner){
        int startingPoint;
        uint parent;

        startingPoint = (guarded_board_size - 2) * actual_board_size  + 2;

        parent = Find(startingPoint);

        for(uint i=0; i<actual_field_count; i++){
            /* this check is necessary due to asymetric FU */
            if(IsFirst(_board[i]))
                aBoard.timesOfBeingOnShortestPath[i] += (short)(Find(_board[i]) == parent);
        }
    }
    /* FU is not performed for this player, BFS necessary */
    else{

        short queue[actual_field_count/2+1];
        unsigned short visited[actual_field_count];
        short beg=0, end=-1;

        memset(visited,0,actual_field_count*sizeof(visited[0]));

        if (Player::First() == winner){
            for(uint i=2*actual_board_size +2; i<2*actual_board_size +board_size+2; i++)
                if(IsFirst(_board[i])){
                    queue[++end]=i;
                    visited[i]=actual_board_size ;
                }
            for(; beg<=end; beg++){
                if((visited[queue[beg]+1] == 0) && IsFirst(_board[queue[beg]+1])){
                    queue[++end]=queue[beg]+1;
                    visited[queue[end]] = visited[beg];
                }
                if((visited[queue[beg]-1] == 0) && IsFirst(_board[queue[beg]-1])){
                    queue[++end]=queue[beg]-1;
                    visited[queue[end]] = visited[beg];
                }
                if((visited[queue[beg]-actual_board_size +1] == 0) && IsFirst(_board[queue[beg]-actual_board_size +1])){
                    queue[++end]=queue[beg]-actual_board_size +1;
                    visited[queue[end]] = visited[beg]-1;
                }
                if((visited[queue[beg]+actual_board_size -1] == 0) && IsFirst(_board[queue[beg]+actual_board_size -1])){
                    queue[++end]=queue[beg]+actual_board_size -1;
                    visited[queue[end]] = visited[beg]+1;
                }
                if((visited[queue[beg]+actual_board_size ] == 0) && IsFirst(_board[queue[beg]+actual_board_size])){
                    queue[++end]=queue[beg]+actual_board_size ;
                    visited[queue[end]] = visited[beg]+1;
                }
                if((visited[queue[beg]-actual_board_size ] == 0) && IsFirst(_board[queue[beg]-actual_board_size])){
                    queue[++end]=queue[beg]-actual_board_size ;
                    visited[queue[end]] = visited[beg]-1;
                }
            }
            for(uint i=actual_board_size +2; i<actual_board_size +board_size+2; i++)
                visited[i]=0;
            beg=0;
            end=-1;
            for (uint i = (board_size+1) * actual_board_size  + 2;
                    i < (board_size+1) * actual_board_size  + 2 + board_size; ++i)
                if(visited[i]>0){
                    queue[++end]=i;
                    aBoard.timesOfBeingOnShortestPath[i]++;
                    visited[i]=0;
                }
            for(; beg<=end; beg++){
                if(visited[queue[beg]+1] > 0 && visited[queue[beg]+1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]+1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]-1] > 0 && visited[queue[beg]-1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]-1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]-actual_board_size +1] > 0 &&
                            visited[queue[beg]-actual_board_size +1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]-actual_board_size +1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]+actual_board_size -1] > 0 &&
                            visited[queue[beg]+actual_board_size -1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]+actual_board_size -1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]+actual_board_size ] > 0 &&
                            visited[queue[beg]+actual_board_size ] < actual_board_size +board_size){
                    queue[++end]=queue[beg]+actual_board_size ;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]-actual_board_size ] > 0 &&
                            visited[queue[beg]-actual_board_size ] < actual_board_size +board_size){
                    queue[++end]=queue[beg]-actual_board_size ;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
            }
        }else{
            for(uint i=2*actual_board_size +2; i<(board_size+2)*actual_board_size +2; i=i+actual_board_size )
                if(IsSecond(_board[i])){
                    queue[++end]=i;
                    visited[i]=1;
                }
            for(; beg<=end; beg++){
                if((visited[queue[beg]+1] == 0) && IsSecond(_board[queue[beg]+1])){
                    queue[++end]=queue[beg]+1;
                    visited[queue[end]] = visited[beg]+1;
                }
                if((visited[queue[beg]-1] == 0) && IsSecond(_board[queue[beg]-1])){
                    queue[++end]=queue[beg]-1;
                    visited[queue[end]] = visited[beg]-1;
                }
                if((visited[queue[beg]-actual_board_size +1] == 0) && IsSecond(_board[queue[beg]-actual_board_size +1])){
                    queue[++end]=queue[beg]-actual_board_size +1;
                    visited[queue[end]] = visited[beg]+1;
                }
                if((visited[queue[beg]+actual_board_size -1] == 0) && IsSecond(_board[queue[beg]+actual_board_size -1])){
                    queue[++end]=queue[beg]+actual_board_size -1;
                    visited[queue[end]] = visited[beg]-1;
                }
                if((visited[queue[beg]+actual_board_size ] == 0) && IsSecond(_board[queue[beg]+actual_board_size])){
                    queue[++end]=queue[beg]+actual_board_size ;
                    visited[queue[end]] = visited[beg];
                }
                if((visited[queue[beg]-actual_board_size ] == 0) && IsSecond(_board[queue[beg]-actual_board_size])){
                    queue[++end]=queue[beg]-actual_board_size ;
                    visited[queue[end]] = visited[beg];
                }
            }
            for(uint i=2*actual_board_size +1;
                        i<(board_size+2)*actual_board_size +1; i=i+actual_board_size )
                visited[i]=0;
            beg=0;
            end=-1;
            for(uint i=2*actual_board_size +1+board_size;
                        i<(board_size+2)*actual_board_size +1+board_size; i=i+actual_board_size )
                if(visited[i]>0){
                    queue[++end]=i;
                    aBoard.timesOfBeingOnShortestPath[i]++;
                    visited[i]=0;
                }
            for(; beg<=end; beg++){
                if(visited[queue[beg]+1] > 0 && visited[queue[beg]+1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]+1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]-1] > 0 && visited[queue[beg]-1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]-1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]-actual_board_size +1] > 0 &&
                            visited[queue[beg]-actual_board_size +1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]-actual_board_size +1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]+actual_board_size -1] > 0 &&
                            visited[queue[beg]+actual_board_size -1] < actual_board_size +board_size){
                    queue[++end]=queue[beg]+actual_board_size -1;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]+actual_board_size ] > 0 &&
                            visited[queue[beg]+actual_board_size ] < actual_board_size +board_size){
                    queue[++end]=queue[beg]+actual_board_size ;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
                if(visited[queue[beg]-actual_board_size ] > 0 &&
                            visited[queue[beg]-actual_board_size ] < actual_board_size +board_size){
                    queue[++end]=queue[beg]-actual_board_size ;
                    visited[queue[end]] = 0;
                    aBoard.timesOfBeingOnShortestPath[queue[end]]++;
                }
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
    uint rep = MakeUnion(pos, pos + 1);
    rep = MakeUnion(rep, pos - 1);
    rep = MakeUnion(rep, pos - actual_board_size );
    rep = MakeUnion(rep, pos - actual_board_size  + 1);
    rep = MakeUnion(rep, pos + actual_board_size );
    MakeUnion(rep, pos + actual_board_size  - 1);
}

uint Board::MakeUnion(uint pos1, uint pos2) {
    if (Switches::DetectWins()) {
        return (IsEmpty(_board[pos2]) || IsSecond(_board[pos1]) != IsSecond(_board[pos2])) ? pos1 : ToPos(_board[pos1] = Find(pos2));
    } else {
        return !IsFirst(_board[pos2]) ? pos1 : ToPos(_board[pos1] = Find(pos2));
    }
}

uint Board::Find(uint pos) {
    while (static_cast<uint>(ToPos(_board[pos])) != pos)
        pos = ToPos(_board[pos] = _board[ToPos(_board[ToPos(_board[pos])])]);
    return _board[pos];
}

uint Board::ConstFind(uint pos) const {
    while (static_cast<uint>(ToPos(_board[pos])) != pos)
        pos = ToPos(_board[pos]);
    return _board[pos];
}

bool Board::IsFull() const {
    return _moves_left == 0;
}

bool Board::IsWon() const {
    if (Switches::DetectWins()) {
        return ConstFind(root_up) == ConstFind(root_down) ||
               ConstFind(root_left) == ConstFind(root_right);
    } else
        return IsFull();
}

Player Board::Winner() const {
    if (ConstFind(root_up) == ConstFind(root_down)) {
        return Player::First();
    } else {
        return Player::Second();
    }
}

void Board::Load (const Board& board) {
    memcpy(this, &board, sizeof(*this));
}

Board::Board() : _moves_left(board_size * board_size),
        _field_map_bound(board_size * board_size - 1), _current(Player::First()) {
    Rand::init(time(NULL));
}

uint Board::MovesLeft() const {
    return _moves_left;
}

void Board::GetPossiblePositions(Board::ushort_ptr& locations) {
    locations = _fast_field_map;
}

void Board::UpdateBridges(uint pos) {
/*     _field_bridge_connections[pos].Clear();
    _field_bridge_connections[pos + 1].Remove(pos);
    _field_bridge_connections[pos - 1].Remove(pos);
    _field_bridge_connections[pos - actual_board_size ].Remove(pos);
    _field_bridge_connections[pos - actual_board_size  + 1].Remove(pos);
    _field_bridge_connections[pos + actual_board_size ].Remove(pos);
    _field_bridge_connections[pos + actual_board_size  - 1].Remove(pos);
*/

/*instead of those seven lines above developed by krzysiocrash
'while' by theolol is used.*/

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

    short second = _board[pos - 2 * actual_board_size  + 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos - actual_board_size ] == 0 &&
            _board[pos - actual_board_size  + 1] == 0) {
        _field_bridge_connections[pos - actual_board_size ].Insert(
                pair<ushort,bool>(pos - actual_board_size  + 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos - actual_board_size  + 1].Insert(
                pair<ushort,bool>(pos - actual_board_size ,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos - actual_board_size  + 2];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos - actual_board_size  + 1] == 0 && _board[pos + 1] == 0) {
        _field_bridge_connections[pos - actual_board_size  + 1].Insert(pair<ushort,bool>(pos + 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos + 1].Insert(pair<ushort,bool>(pos - actual_board_size  + 1,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos + actual_board_size  + 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos + 1] == 0 && _board[pos + actual_board_size ] == 0) {
        _field_bridge_connections[pos + 1].Insert(pair<ushort,bool>(pos + actual_board_size ,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos + actual_board_size ].Insert(pair<ushort,bool>(pos + 1,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos + 2 * actual_board_size  - 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos + actual_board_size ] == 0 &&
            _board[pos + actual_board_size  - 1] == 0) {
        _field_bridge_connections[pos + actual_board_size ].Insert(
                pair<ushort,bool>(pos + actual_board_size  - 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos + actual_board_size  - 1].Insert(
                pair<ushort,bool>(pos + actual_board_size ,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos + actual_board_size  - 2];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos + actual_board_size  - 1] == 0 && _board[pos - 1] == 0) {
        _field_bridge_connections[pos + actual_board_size  - 1].Insert(pair<ushort,bool>(pos - 1,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos - 1].Insert(pair<ushort,bool>(pos + actual_board_size  - 1,!(val >> SHORT_BIT_SIZE)));
    }
    second = _board[pos - actual_board_size  - 1];
    if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
            && _board[pos - 1] == 0 &&
            _board[pos - actual_board_size ] == 0) {
        _field_bridge_connections[pos - 1].Insert(pair<ushort,bool>(pos - actual_board_size ,!(val >> SHORT_BIT_SIZE)));
        _field_bridge_connections[pos - actual_board_size ].Insert(pair<ushort,bool>(pos - 1,!(val >> SHORT_BIT_SIZE)));
    }

    UpdateBridgeBound(pos + 1);
    UpdateBridgeBound(pos - 1);
    UpdateBridgeBound(pos - actual_board_size );
    UpdateBridgeBound(pos - actual_board_size  + 1);
    UpdateBridgeBound(pos + actual_board_size );
    UpdateBridgeBound(pos + actual_board_size  - 1);

/*crucial line from theolol:*/

    attacked_bridges.Remove(pos);

}

void Board::UpdateBridgeBound(uint pos) { /*I assume It's OK*/
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

    for (unsigned char x = 'a'; x < 'a' + board_size; ++x)
        s << " " << x;
    s << std::endl;
    for (uint i = 1; i <= board_size; ++i) {
        for (uint j = 1; j < (i < 10 ? i : i - 1); ++j)
            s << " ";
        s << i;
        if (ToPos(1, i) == last_move.GetPos())
            s << "(";
        else s << " ";
        for (uint j = 1; j <= board_size; ++j) {
            uint pos = ToPos(j, i);
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
    for (uint j = 0; j <= board_size; ++j)
        s << " ";
    for (unsigned char x = 'a'; x < 'a' + board_size; ++x)
        s << " " << x;

    s << std::endl << "Bridges:";
    for (unsigned i = static_cast<unsigned>(_field_map_bound + 1); i < _moves_left; i++)
        s << " " << Location(_fast_field_map[i]).ToCoords();

    return s.str();
}

bool Board::IsValidMove(const Move& move) {
    return (Location::ValidPosition(move.GetLocation().GetPos())) &&
          ((_moves_left % 2 == 0 && move.GetPlayer() == Player::Second()) ||
           (_moves_left % 2 == 1 && move.GetPlayer() == Player::First()));
}

Move Board::GenerateMoveUsingKnowledge(const Player& player) const {
    if(Switches::IsDefendingBridges())
        if (!attacked_bridges.Empty()){
            Move m = Move(player, Location(attacked_bridges.RandomElem()));
            return m;
        }
    if(Switches::IsAvoidingBridges())
        return RandomLegalAvoidBridges(player);
    return RandomLegalMove(player);
}

// -----------------------------------------------------------------------------

} // namespace Hex
