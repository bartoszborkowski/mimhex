#ifndef MIMHEX_IMPLEMENTATION_TIME_MANAGER_H
#define MIMHEX_IMPLEMENTATION_TIME_MANAGER_H

#include "hextypes.h"

const uint kClockDelay = 100;

struct Timer {
    void Start();
    uint Clock();
    uint State();

private:
    uint begin;
    uint last;
    uint buffer, last_buffer;
    uint request;
    uint delay;
};

enum TimeManagementType {
    kManagementPlayoutsPerMove = 0,
    kManagementPlayoutMovesPerGame = 1,
    kManagementTime = 2
};

struct TimeManager
{
    TimeManager();
    void NewMove();
    bool NewPlayout(uint);
    void EndMove();

    TimeManagementType management;

    uint time_left;
    uint64 playout_moves_left;
    uint playouts_per_move;

private:
    Timer timer;

    uint64 resources_left;
    uint64 resources_used;
};

#endif