#ifndef INVERSE_SQRT_H
#define INVERSE_SQRT_H

#include <cmath>
#include "switches.h"

float InverseSqrt (float x) {
    // FIXME: Exclude Switches from Hex namespace.
    if (!Hex::Switches::Hacks()) {
      return 1.0 / std::sqrt(x);
    } else {
        /*
        * Nowy kod z Wiki.
        */
        union {
            float f;
            int i;
        } tmp;
        tmp.f = x;
        tmp.i = 0x5f3759df - (tmp.i >> 1);
        float y = tmp.f;
        return y * (1.5f - 0.5f * x * y * y);
    }
}

#endif /* INVERSE_SQRT_H */
