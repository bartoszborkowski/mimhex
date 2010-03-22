#ifndef MCTS_STATS_H_
#define MCTS_STATS_H_

#include "hextypes.h"

namespace Hex {

/**
 * The class for maintaining a single set of statistics. Performs simple
 * memoization in order to speed up GetValue() method. No memoization is done
 * for GetMu() or GetBound() methods alone.
 */
class Statistics {
    public:
        Statistics(uint init_won, uint init_played);

        /**
         * @return The mu factor, that is won / played
         */
        float GetMu() const;

        /**
         * @return The confidence bound for the current played value.
         */
        float GetBound() const;

        /**
         * @note This method is memoized.
         * @return The value increased by the confidence bound.
         */
        float GetValue() const;

        uint GetWon() const;
        uint GetLost() const;
        uint GetPlayed() const;

        /**
         * Update statistics to include another won game.
         */
        void Win();

        /**
         * Update statistics to include another lost game.
         */
        void Lose();

        /**
         * Update statistics to include another game.
         * @in @param won True if the game was won.
         */
        void Update(bool won);

    private:
        uint won;
        uint played;

        /**
         * The field used for memoization of the GetValue() function.
         */
        mutable float value;

        /**
         * True if field value is valid.
         */
        mutable bool computed;
};

} // namespace Hex

#endif /* MCTS_STATS_H_ */
