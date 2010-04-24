#ifndef HEX_BOARD_PLAYER_H_
#define HEX_BOARD_PLAYER_H_

#include <string>
#include "hextypes.h"
#include "params.h"
#include "switches.h"

using namespace std;

namespace Hex{

class Player {
    public:
        static Player First();
        static Player Second();
        static Player None();
        static Player OfString (std::string);

        Player Opponent() const;

        bool operator== (const Player&) const;
        bool operator!= (const Player&) const;

        uint GetVal();

        string ToString() const;

        static bool ValidPlayer(const std::string& player);

    private:
        Player();
        Player(uint val);

    private:
        uint _val;
};


} //namespace Hex

#endif
