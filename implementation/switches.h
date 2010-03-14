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

        static bool Rave() {
            return raveOn;
        }

        static bool PathAmaf() {
            return pathAmafOn;
        }

        static bool PathRave() {
            return pathRaveOn;
        }

        static bool FloodFill() {
            return floodFillOn;
        }

        static bool SingleShortest() {
            return singleShortestOn;
        }

        static bool AllShortest() {
            return allShortestOn;
        }

        static void SetAvoidingBridges(bool b) {
            SetSwitch("Bridge avoiding", avoidingBridgesOn, b);
        }

        static void SetDefendingBridges(bool b) {
            SetSwitch("Bridge defending", defendingBridgesOn, b);
        }

        static void SetPatternsUsed(bool b) {
            SetSwitch("Pattern use", patternsOn, b);
        }

        static void SetDetectWins(bool b) {
            SetSwitch("Early win detection", detectWins, b);
        }

        static void SetRave(bool b) {
            SetSwitch("RAVE", raveOn, b);
        }

        static void SetPathAmaf(bool b) {
            SetSwitch("Path AMAF", pathAmafOn, b);
        }

        static void SetPathRave(bool b) {
            SetSwitch("Path RAVE", pathRaveOn, b);
        }

        static void SetFloodFill(bool b) {
            SetSwitch("Flood-fill strategy", floodFillOn, b);
            if (b) {
                SetSwitch("Single shortest strategy", singleShortestOn, false);
                SetSwitch("All shortest paths strategy", allShortestOn, false);
            }
        }

        static void SetSingleShortest(bool b) {
            SetSwitch("Single shortest path strategy", singleShortestOn, b);
            if (b) {
                SetSwitch("Flood-fill strategy", floodFillOn, false);
                SetSwitch("All shortest paths strategy", allShortestOn, false);
            }
        }

        static void SetAllShortest(bool b) {
            SetSwitch("All shortest paths strategy", allShortestOn, b);
            if (b) {
                SetSwitch("Flood-fill strategy", floodFillOn, false);
                SetSwitch("Single shortest path strategy", singleShortestOn, false);
            }
        }

    private:
        static SWITCH_TYPE avoidingBridgesOn;
        static SWITCH_TYPE defendingBridgesOn;
        static SWITCH_TYPE patternsOn;
        static SWITCH_TYPE detectWins;
        static SWITCH_TYPE raveOn;
        static SWITCH_TYPE pathAmafOn;
        static SWITCH_TYPE pathRaveOn;
        static SWITCH_TYPE floodFillOn;
        static SWITCH_TYPE singleShortestOn;
        static SWITCH_TYPE allShortestOn;

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
