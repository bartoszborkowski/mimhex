#include "time_manager.h"

#include "conditional_assert.h"
#include "params.h"

void Timer::Start()
{
    begin = Clock();
    last = begin;
    buffer = 8;
    last_buffer = buffer;
    delay = 0;
    request = 0;
}

uint Timer::Clock()
{
    uint64 now = clock();
    return now * 1000 / CLOCKS_PER_SEC;
}

uint Timer::State()
{
    if (request == buffer - 1) {
        uint now = Clock();
        delay = now - last;
        if (delay < kClockDelay) {
            last_buffer = buffer;
            buffer *= 2;
        }
        last = now;
        request = 0;
    } else
        ++request;
    return last + request * delay / last_buffer - begin;
}

TimeManager::TimeManager()
    : management(kManagementPlayoutsPerMove),
    time_left(60000), playout_moves_left(50000000), playouts_per_move(100000),
    resources_left(0), resources_used(0)
{
}

void TimeManager::NewMove()
{
    switch (management) {
        case kManagementPlayoutsPerMove:
            resources_left = playouts_per_move;
            break;
        case kManagementPlayoutMovesPerGame:
            resources_left = playout_moves_left;
            break;
        case kManagementTime:
            resources_left = time_left;
            timer.Start();
            break;
    }
    resources_used = 0;
}

bool TimeManager::NewPlayout(uint empties)
{
    if (management == kManagementPlayoutsPerMove)
        return resources_left--;

    if (management == kManagementTime)
        resources_used = timer.State();

    ASSERT(Hex::Params::resources_part <= 1.0);
    bool permission = (resources_used < Hex::Params::resources_part * resources_left);

    if (management == kManagementPlayoutMovesPerGame)
        resources_used += empties;

    return permission;
}

void TimeManager::EndMove()
{
    switch (management) {
        case kManagementPlayoutsPerMove:
            break;
        case kManagementPlayoutMovesPerGame:
            playout_moves_left = (resources_used > playout_moves_left ? 0 : playout_moves_left - resources_used);
            break;
        case kManagementTime:
            uint used = timer.State();
            time_left = (used > time_left ? 0 : time_left - used);
            break;
    }
}
