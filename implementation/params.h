#ifndef PARAMS_H_
#define PARAMS_H_

namespace Hex {

class Params {
public:
	static float alpha;
	static float beta;
	static unsigned initialization;
	static unsigned bridgeWeight;
};

float Params::alpha = 1.0;
float Params::beta = 20.0;
unsigned Params::initialization = 1;
unsigned Params::bridgeWeight = 128;

} // namespace Hex

#endif /* PARAMS_H_ */
