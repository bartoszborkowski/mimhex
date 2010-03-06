#ifndef __SWITCHES_H__
#define __SWITCHES_H__

namespace Hex {

class Switches {
  public:
	bool avoidingBridgesOn;
	bool defendingBridgesOn;
	bool patternsOn;

	Switches() : avoidingBridgesOn(true), defendingBridgesOn(true), patternsOn(true) {};

};

} // namespace Hex

#endif
