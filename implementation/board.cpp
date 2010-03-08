/*uber*/

#include "board.h"
#include "random.h"
#include "conditional_assert.h"
#include <cstring>
#include <sstream>

#define SHORT_BIT_SIZE (sizeof(short) * 8)

namespace Hex {

// -----------------------------------------------------------------------------

inline Player Player::First() { return Player(0); }

inline Player Player::Second() { return Player(1); }

inline Player Player::OfString (std::string player) {
	ASSERT(ValidPlayer(player));
	if (player == "black")
		return Player::First();
	else return Player::Second();
}

inline Player Player::Opponent() const {
	return Player(_val ^ 1);
}

inline bool Player::operator== (const Player& player) {
	return player._val == _val;
}

inline bool Player::operator!= (const Player& player) {
	return player._val != _val;
}

inline Player::Player(uint val) : _val(val) {}

inline uint Player::GetVal() const { return _val; }

inline bool Player::ValidPlayer(const std::string& player) {
	return player == "black" || player == "white";
}

// -----------------------------------------------------------------------------

inline Location Location::OfCoords (std::string coords) {
	ASSERT(ValidLocation(coords));
	uint x = coords[0] >= 'a' ? coords[0] - 'a' : coords[0] - 'A';
	uint y = coords[1] - '0';
	if (coords.size() > 2)
		y = y * 10 + coords[2] - '0';
	return Location(++x, y);
}

inline Location::Location(uint x, uint y) : _pos(ToTablePos(x, y)) { }
inline Location::Location(uint pos) : _pos(pos) {}

inline Location::Location() {}

inline uint Location::GetPos() const { return _pos; }

inline std::string Location::ToCoords() const {
	std::stringstream coords;
	coords << static_cast<char>(_pos % kBoardSizeAligned + 'a' - 2);
	coords << _pos / kBoardSizeAligned - 1;
	return coords.str();
}

inline uint Location::ToTablePos(uint x, uint y) {
	ASSERT (ValidLocation(x, y));
	return (++y) * (kBoardSizeAligned) + x + 1;
}

inline bool Location::operator==(Location loc) const {
	return loc._pos == _pos;
}

inline bool Location::operator!=(Location loc) const {
	return loc._pos != _pos;
}

inline bool Location::ValidLocation(const std::string& location) {
	if (location.size() == 0 || location.size() > 3)
		return false;
	uint x = location[0] >= 'a' ? location[0] - 'a' : location[0] - 'A';
	uint y = location[1] - '0';
	if (location.size() > 2)
		y = y * 10 + location[2] - '0';
	return ValidLocation(++x, y);
}

inline bool Location::ValidPosition(uint pos) {
	uint x, y;
	ToCoords(pos, x, y);
	return ValidLocation(x, y);
}

inline bool Location::ValidLocation(uint x, uint y) {
	return x > 0 && y > 0 && x <= kBoardSize && y <= kBoardSize;
}

inline void Location::ToCoords(uint pos, uint& x, uint& y) {
	x = pos % kBoardSizeAligned - 1;
	y = pos / kBoardSizeAligned - 1;
}

// -----------------------------------------------------------------------------

inline Move::Move (const Player& player, const Location& location)
	: _player(player), _location(location) {}

inline Move::Move() :_player(Player::First()), _location(0) {}

inline Location Move::GetLocation() const { return _location; }

inline Player Move::GetPlayer() const { return _player; }

// -----------------------------------------------------------------------------

const uint Board::guarded_board_size = kBoardSize + 4;
const uint Board::table_size = kBoardSizeAligned * kBoardSizeAligned;

const Board Board::Empty() {

	Board board;

	uint counter = 0;
	for (uint i = 0; i < table_size; ++i)
		board._reverse_fast_field_map[i] = -1;
	for (uint i = 2; i <= kBoardSize + 1; ++i) {
		for (uint j = 2; j <= kBoardSize + 1; ++j) {
			uint field = i * kBoardSizeAligned + j;
			board._fast_field_map[counter] = field;
			board._reverse_fast_field_map[field] = counter++;
		}
	}

	for (uint i = 0; i < table_size; ++i)
		board._board[i] = 0;
	for (uint i = 2; i <= kBoardSize + 1; ++i)
		board._board[i + kBoardSizeAligned] = 2 + kBoardSizeAligned;
	for (uint i = (guarded_board_size - 2) * kBoardSizeAligned + 2;
			i < (guarded_board_size - 2) * (kBoardSizeAligned + 1); ++i) {
		board._board[i] = (guarded_board_size - 2) * kBoardSizeAligned + 2;
	}
	for (uint i = 2 * kBoardSizeAligned + 1, j = 0; j < guarded_board_size - 4;
			i += kBoardSizeAligned, j++) {
		board._board[i] = -1;
		board._board[i + kBoardSize + 1] = -1;
	}


	return board;
}

inline Player Board::CurrentPlayer() const {
	return _current;
}

Move Board::RandomLegalMove (const Player& player) const {

#define STRANGE_NUMBER 128

	// _field_map_bound - last not-bridge
	// (_field_map_bound + 1) - first field after not-bridge fields == count of not-bridges
	// (_moves_left - _field_map_bound - 1) - bridges count

	uint rnd = Rand::next_rand((_field_map_bound + 1) * STRANGE_NUMBER + _moves_left - _field_map_bound - 1);
	uint result;

	if (rnd >= static_cast<unsigned>(_field_map_bound + 1) * STRANGE_NUMBER)
		result = rnd - (_field_map_bound + 1) * (STRANGE_NUMBER - 1);
	else 
		result = rnd / STRANGE_NUMBER;

#undef STRANGE_NUMBER

	return Move(player, _fast_field_map[result]);
}

/*Move Board::RandomLegalMoveIgnoreBridges (const Player& player) const {
	return Move(player, _fast_field_map[Rand::next_rand(_moves_left)]);
}*/

inline void Board::PlayLegal (const Move& move) {
	ASSERT(IsValidMove(move));
	uint pos = move.GetLocation().GetPos();
	if (move.GetPlayer() == Player::First()) {
		_board[pos] = pos;
		MakeUnion(pos);
	} else {
		_board[pos] = -1;
	}
	uint fast_map_pos = _reverse_fast_field_map[pos];
	uint replace_pos = _fast_field_map[--_moves_left];
	_fast_field_map[fast_map_pos] = replace_pos;
	_reverse_fast_field_map[replace_pos] = fast_map_pos;
	_current = _current.Opponent();

	_reverse_fast_field_map[pos] = _moves_left;			/*WTF?*/
	if (_field_map_bound >= static_cast<int>(_moves_left))
		_field_map_bound--;					/*serio - WTF?*/

	UpdateBridgeBound(replace_pos);
	UpdateBridges(pos);
}

/*inline void Board::PlayLegalIgnoreBridges (const Move& move) {
	ASSERT(IsValidMove(move));
	uint pos = move.GetLocation().GetPos();
	if (move.GetPlayer() == Player::First()) {
		_board[pos] = pos;
		MakeUnion(pos);
	} else {
		_board[pos] = -1;
	}
	uint fast_map_pos = _reverse_fast_field_map[pos];
	uint replace_pos = _fast_field_map[--_moves_left];
	_fast_field_map[fast_map_pos] = replace_pos;
	_reverse_fast_field_map[replace_pos] = fast_map_pos;
	_current = _current.Opponent();
}*/

inline void Board::MakeUnion(uint pos) {
	uint rep = MakeUnion(pos, pos + 1);
	rep = MakeUnion(rep, pos - 1);
	rep = MakeUnion(rep, pos - kBoardSizeAligned);
	rep = MakeUnion(rep, pos - kBoardSizeAligned + 1);
	rep = MakeUnion(rep, pos + kBoardSizeAligned);
	MakeUnion(rep, pos + kBoardSizeAligned - 1);
}

uint Board::MakeUnion(uint pos1, uint pos2) {
	return _board[pos2] <= 0 ? pos1 : _board[pos1] = Find(pos2);
}

inline uint Board::Find(uint pos) {
	while (static_cast<uint>(_board[pos]) != pos)
		pos = _board[pos] = _board[_board[_board[pos]]];
	return pos;
}

inline uint Board::ConstFind(uint pos) const {
	while (static_cast<uint>(_board[pos]) != pos)
		pos = _board[pos];
	return pos;
}

inline bool Board::IsFull() const {
	return _moves_left == 0;
}

inline Player Board::Winner() const {
	if (ConstFind(kBoardSizeAligned + 2) ==
		ConstFind((guarded_board_size - 2) * kBoardSizeAligned + 2)) {
			return Player::First();
	}
	else return Player::Second();
}

inline void Board::Load (const Board& board) {
	memcpy(this, &board, sizeof(*this));
}

inline Board::Board() : _moves_left(kBoardSize * kBoardSize),
		_field_map_bound(kBoardSize * kBoardSize - 1), _current(Player::First()) {
	Rand::init(time(NULL));
}

inline uint Board::MovesLeft() const {
	return _moves_left;
}

inline void Board::GetPossiblePositions(Board::ushort_ptr& locations) {
	locations = _fast_field_map;
}

void Board::UpdateBridges(uint pos) {
/* 	_field_bridge_connections[pos].Clear();
	_field_bridge_connections[pos + 1].Remove(pos);
	_field_bridge_connections[pos - 1].Remove(pos);
	_field_bridge_connections[pos - kBoardSizeAligned].Remove(pos);
	_field_bridge_connections[pos - kBoardSizeAligned + 1].Remove(pos);
	_field_bridge_connections[pos + kBoardSizeAligned].Remove(pos);
	_field_bridge_connections[pos + kBoardSizeAligned - 1].Remove(pos);
*/

/*instead of those seven lines above developed by krzysiocrash
'while' by theolol is used. WHY CANNOT I CLEAR THE _field_bridge_connections[pos]??*/

	SmallSetIterator<pair<ushort,bool>, 3> it = _field_bridge_connections[pos].GetIterator();
	while (!it.IsEnd()){
		uint elem = (*it).first;

		if((*it).second ^ (_current!=Player::First())) 
							   
			attacked_bridges.Insert(elem);

		_field_bridge_connections[elem].Remove(pair<ushort,bool>(pos,(*it).second));
		it++;										/*to tylko Bóg wie czemu*/
	}

	short val = _board[pos];

	short second = _board[pos - 2 * kBoardSizeAligned + 1];
	if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
			&& _board[pos - kBoardSizeAligned] == 0 &&
			_board[pos - kBoardSizeAligned + 1] == 0) {
		_field_bridge_connections[pos - kBoardSizeAligned].Insert(
				pair<ushort,bool>(pos - kBoardSizeAligned + 1,!(val >> SHORT_BIT_SIZE)));
		_field_bridge_connections[pos - kBoardSizeAligned + 1].Insert(
				pair<ushort,bool>(pos - kBoardSizeAligned,!(val >> SHORT_BIT_SIZE)));
	}
	second = _board[pos - kBoardSizeAligned + 2];
	if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
			&& _board[pos - kBoardSizeAligned + 1] == 0 && _board[pos + 1] == 0) {
		_field_bridge_connections[pos - kBoardSizeAligned + 1].Insert(pair<ushort,bool>(pos + 1,!(val >> SHORT_BIT_SIZE)));
		_field_bridge_connections[pos + 1].Insert(pair<ushort,bool>(pos - kBoardSizeAligned + 1,!(val >> SHORT_BIT_SIZE)));
	}
	second = _board[pos + kBoardSizeAligned + 1];
	if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
			&& _board[pos + 1] == 0 && _board[pos + kBoardSizeAligned] == 0) {
		_field_bridge_connections[pos + 1].Insert(pair<ushort,bool>(pos + kBoardSizeAligned,!(val >> SHORT_BIT_SIZE)));
		_field_bridge_connections[pos + kBoardSizeAligned].Insert(pair<ushort,bool>(pos + 1,!(val >> SHORT_BIT_SIZE)));
	}
	second = _board[pos + 2 * kBoardSizeAligned - 1];
	if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
			&& _board[pos + kBoardSizeAligned] == 0 &&
			_board[pos + kBoardSizeAligned - 1] == 0) {
		_field_bridge_connections[pos + kBoardSizeAligned].Insert(
				pair<ushort,bool>(pos + kBoardSizeAligned - 1,!(val >> SHORT_BIT_SIZE)));
		_field_bridge_connections[pos + kBoardSizeAligned - 1].Insert(
				pair<ushort,bool>(pos + kBoardSizeAligned,!(val >> SHORT_BIT_SIZE)));
	}
	second = _board[pos + kBoardSizeAligned - 2];
	if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
			&& _board[pos + kBoardSizeAligned - 1] == 0 && _board[pos - 1] == 0) {
		_field_bridge_connections[pos + kBoardSizeAligned - 1].Insert(pair<ushort,bool>(pos - 1,!(val >> SHORT_BIT_SIZE)));
		_field_bridge_connections[pos - 1].Insert(pair<ushort,bool>(pos + kBoardSizeAligned - 1,!(val >> SHORT_BIT_SIZE)));
	}
	second = _board[pos - kBoardSizeAligned - 1];
	if (second != 0 && (second >> SHORT_BIT_SIZE) == (val >> SHORT_BIT_SIZE)
			&& _board[pos - 1] == 0 &&
			_board[pos - kBoardSizeAligned] == 0) {
		_field_bridge_connections[pos - 1].Insert(pair<ushort,bool>(pos - kBoardSizeAligned,!(val >> SHORT_BIT_SIZE)));
		_field_bridge_connections[pos - kBoardSizeAligned].Insert(pair<ushort,bool>(pos - 1,!(val >> SHORT_BIT_SIZE)));
	}

	UpdateBridgeBound(pos + 1);
	UpdateBridgeBound(pos - 1);
	UpdateBridgeBound(pos - kBoardSizeAligned);
	UpdateBridgeBound(pos - kBoardSizeAligned + 1);
	UpdateBridgeBound(pos + kBoardSizeAligned);
	UpdateBridgeBound(pos + kBoardSizeAligned - 1);

/*crucial line from theolol:*/

	attacked_bridges.Remove(pos);

}

void Board::UpdateBridgeBound(uint pos) {  /*Jezu*/
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

std::string Board::ToAsciiArt(Location last_move) const {

	std::stringstream s;

	for (unsigned char x = 'a'; x < 'a' + kBoardSize; ++x)
		s << " " << x;
	s << std::endl;
	for (uint i = 2; i <= kBoardSize + 1; ++i) {
		for (uint j = 2; j < (i <= 10 ? i : i - 1); ++j)
			s << " ";
		s << i - 1;
		if (i * kBoardSizeAligned + 1 == last_move.GetPos())
			s << "(";
		else s << " ";
		for (uint j = 2; j <= kBoardSize + 1; ++j) {
			uint pos = i * kBoardSizeAligned + j;
			if (_board[pos] == 0)
				s << ".";
			else if (_board[pos] < 0)
				s << "#";
			else s << "O";
			if (pos == last_move.GetPos())
				s << ")";
			else if (pos + 1 == last_move.GetPos())
				s << "(";
			else s << " ";
		}
		s << i - 1 << std::endl;
	}
	for (uint j = 0; j <= kBoardSize; ++j)
		s << " ";
	for (unsigned char x = 'a'; x < 'a' + kBoardSize; ++x)
		s << " " << x;

	s << std::endl << "Bridges:";
	for (unsigned i = static_cast<unsigned>(_field_map_bound + 1); i < _moves_left; i++)
		s << " " << Location(_fast_field_map[i]).ToCoords();

	return s.str();
}

bool Board::IsValidMove(const Move& move) {
	if (!Location::ValidPosition(move.GetLocation().GetPos()))
		return false;
	return _board[move.GetLocation().GetPos()] == 0;
}

//If bride exists -> defendig move, else -> random move
Move Board::DefendBridgeMove(const Player& player) const {
	if (!attacked_bridges.Empty()){
		//fprintf(stderr,"niepusty\n");
		Move m = Move(player, Location(attacked_bridges.RandomElem()));
		//fprintf(stderr,"%s",ToAsciiArt(m.GetLocation()).c_str());
		//sleep(10);
		return m;
	}
	return RandomLegalMove(player);
}

// -----------------------------------------------------------------------------

} // namespace Hex
