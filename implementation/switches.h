#ifndef __SWITCHES_H__
#define __SWITCHES_H__

#include <iostream>
#include <string>

#define DYNAMIC_SWITCHES

#ifdef DYNAMIC_SWITCHES
    #define SWITCH_TYPE     bool
#else
    #define SWITCH_TYPE     const bool
#endif

namespace Hex {

class Switches {
    public:
        static bool IsAvoidingBridges() {
            return avoidingBridgesOn;
        }

        static bool IsDefendingBridges() {
            return avoidingBridgesOn;
        }

        static bool PatternsUsed() {
            return patternsOn;
        }

        static bool DetectWins() {
            return detectWins;
        }

        static bool PathAmaf() {
            return pathAmafOn;
        }

        static bool Rave() {
            return raveOn;
        }

        static void SetAvoidingBridges(bool b) {
            SetSwitch("avoidBridges", avoidingBridgesOn, b);
        }

        static void SetDefendingBridges(bool b) {
            SetSwitch("defendBridges", defendingBridgesOn, b);
        }

        static void SetPatternsUsed(bool b) {
            SetSwitch("patternsUsed", patternsOn, b);
        }

        static void SetDetectWins(bool b) {
            SetSwitch("detectWins", detectWins, b);
        }

        static void SetRave(bool b) {
            SetSwitch("RaveOn", raveOn, b);
        }

        static void SetPathAmaf(bool b) {
            SetSwitch("pathAmafOn", pathAmafOn, b);
        }

    private:
        static SWITCH_TYPE avoidingBridgesOn;
        static SWITCH_TYPE defendingBridgesOn;
        static SWITCH_TYPE patternsOn;
        static SWITCH_TYPE detectWins;
        static SWITCH_TYPE raveOn;
        static SWITCH_TYPE pathAmafOn;

        static void SetSwitch(const std::string& name, bool& value, bool newValue) {
#ifdef DYNAMIC_SWITCHES
            std::cerr << "INFO: Switched `" << name << "` to " << int(newValue) << std::endl;
            value = newValue;
#else
            // TODO: Improve on logging.
            std::cerr << "WARN: Could not switch `" << name << "` to " << int(newValue) << std::endl;
            std::cerr << "Current value of the switch: " << int(value) << std::endl;
#endif
        }

        Switches() {}
};

} // namespace Hex

#endif
