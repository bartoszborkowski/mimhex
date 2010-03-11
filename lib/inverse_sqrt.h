#ifndef INVERSE_SQRT_H
#define INVERSE_SQRT_H

#include <cmath>

#include "absolute_cast.h"

float InverseSqrt (float x) {
/*nowy kod z wikipedii:*/

	/*union {
		float f;
		int i;
	} tmp;
	tmp.f = x;
	tmp.i = 0x5f3759df - (tmp.i >> 1);
	float y = tmp.f;
	return y * (1.5f - 0.5f * x * y * y);*/



//     float xhalf = 0.5f * x;
//     int i = absolute_cast<int>(x);
//     i = 0x5f3759df - (i>>1);
//     x = absolute_cast<float>(i);
//     return x * (1.5f - xhalf * x * x);


      return 1.0/std::sqrt(x);
}


#endif /* INVERSE_SQRT_H */
