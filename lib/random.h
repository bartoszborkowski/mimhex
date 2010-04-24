#ifndef RANDOM_H_
#define RANDOM_H_

#include "conditional_assert.h"

class Rand {
public:
    static void init (unsigned seed) { _seed = seed; }
    static unsigned next_rand() {
        unsigned lo = 16807 * (_seed & 0xffff);
        unsigned hi = 16807 * (_seed >> 16);
        lo += (hi & 0x7fff) << 16;
        lo += hi >> 15;
        return _seed = (lo & 0x7FFFFFFF) + (lo >> 31);
    }
    static unsigned next_rand(unsigned n) {
        ASSERT(n < 0xffff);
        return ((next_rand() & 0xffff) * n) >> 16;
    }
    static double next_double() {
        const double inv_max_uint = 1.0 / static_cast<double>(1L << 31);
        return next_rand() * inv_max_uint;
    }
    static double next_double (double scale) {
        const double inv_max_uint = 1.0 / static_cast<double>(1L << 31);
        return static_cast<double>(next_rand()) * (inv_max_uint * scale);
    }

private:
    static unsigned _seed;
};

#endif /* RANDOM_H_ */
