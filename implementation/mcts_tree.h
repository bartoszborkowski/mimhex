#ifndef MCTS_TREE_H_
#define MCTS_TREE_H_

#include "hextypes.h"
#include "mcts_node.h"

namespace Hex {

class MCTSTree {
    public:
        MCTSTree();

        void Reset(const Board& board);

        Move BestMove(Player player, const Board& board);
        Player RandomFinish(Board& board, uint* path, uint current_level);
        Player RandomFinishWithoutPath(Board& board);

        void SetMaxDepth(uint depth);
        void SetPerMove(uint playouts);

        std::string ToAsciiArt(uint children);

    private:
        AutoPointer<MCTSNode> root;
        uint max_depth;
        uint per_move;

        static const uint default_max_depth;
        static const uint default_per_move;
        static const uint ultimate_depth;
        static const uint visits_to_expand;
        static const uint amaf_paths_palyouts;
};

} // namespace Hex

#endif /* MCTS_TREE_H_ */
