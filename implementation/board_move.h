#ifndef HEX_BOARD_MOVE_H_
#define HEX_BOARD_MOVE_H_

#include <string>
#include "small_set.h"
#include "params.h"
#include "switches.h"

namespace Hex{

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

} //namespace Hex

#endif
