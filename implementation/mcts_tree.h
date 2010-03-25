#ifndef MCTS_TREE_H_
#define MCTS_TREE_H_

#include "hextypes.h"
#include "mcts_node.h"
#include "time_manager.h"

namespace Hex {

class MCTSTree {
    public:
        MCTSTree();

        /**
         * Reset the tree to a single root associated with the given board.
         * @in @param board The given board.
         */
        void Reset(const Board& board);

        /**
         * @in @param board The given board.
         * @return The best move possible for the situation described by the
         * given board.
         */
        Move BestMove(const Board& board);

        /**
         * @in @param board The given board.
         * @out @param path A history array to fill in.
         * @in @param level The level of the tree from which the game is
         * finished
         * @return The winner of the finished game.
         */
        Player RandomFinish(Board& board, uint* history, uint level);

        void SetMaxDepth(uint depth);

        std::string ToAsciiArt(uint children);

        TimeManager & GetTimeManager();

    private:
        AutoPointer<MCTSNode> root;
        uint max_depth;
        uint per_move;
        TimeManager time_manager;

        static const uint default_max_depth;
        static const uint default_per_move;
        static const uint ultimate_depth;
        static const uint visits_to_expand;
        static const uint amaf_paths_palyouts;
};

} // namespace Hex

#endif /* MCTS_TREE_H_ */
