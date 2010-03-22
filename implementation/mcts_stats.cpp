#include "mcts_stats.h"
#include "inverse_sqrt.h"
#include "conditional_assert.h"

namespace Hex {

Statistics::Statistics(uint init_won, uint init_played):
    won(init_won),
    played(init_played),
    computed(false) {}

float Statistics::GetMu() const {
    ASSERT(played > 0);
    return static_cast<float>(won) / static_cast<float>(played);
}

float Statistics::GetBound() const {
    ASSERT(played > 0);
    return Params::alpha * InverseSqrt(played);
}

float Statistics::GetValue() const {
    if (!computed) {
        computed = true;
        value = GetMu() + GetBound();
    }
    return value;
}

uint Statistics::GetWon() const {
    return won;
}

uint Statistics::GetLost() const {
    return played - won;
}

uint Statistics::GetPlayed() const {
    return played;
}

void Statistics::Win() {
    ++won;
    ++played;
    computed = false;
}

void Statistics::Lose() {
    ++played;
    computed = false;
}

void Statistics::Update(bool w) {
    /// w & 1 evaluates to 1 for w == true, and 0 for w == false.
    won += (1 & w);
    ++played;
    computed = false;
}

} // namespace Hex
