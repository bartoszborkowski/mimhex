#ifndef HEX_BOARD_LOCATION_H_
#define HEX_BOARD_LOCATION_H_

#include <string>
#include "hextypes.h"
#include "params.h"
#include "switches.h"

namespace Hex{

class Location {
    public:
        Location();
        static Location OfCoords (std::string);
        Location (uint pos);
        Location (uint x, uint y);
        uint GetPos() const;
        uint GetX() const;
        uint GetY() const;
        std::string ToCoords() const;
        bool operator==(Location loc) const;
        bool operator!=(Location loc) const;
        static bool ValidLocation(const std::string& location);
        static bool ValidLocation(uint x, uint y);
        static bool ValidPosition(uint pos);

    private:
        uint _pos;
};

} //namespace Hex

#endif
