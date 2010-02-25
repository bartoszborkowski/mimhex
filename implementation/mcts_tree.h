#ifndef MCTS_TREE_H_
#define MCTS_TREE_H_

#include "mcts_node.h"
#include "time_manager.h"

namespace Hex {

class MCTSTree {
public:
	MCTSTree();
	void ClearTree();
	void Reset();
	Move BestMove(Player player, Board& board);
	Player RandomFinish(Board& board, uint* path,
			uint current_level);
	void SetMaxDepth(uint depth);
	void SetPlayoutsPerMove(uint playouts);
	std::string ToAsciiArt(uint children);
    TimeManager & GetTimeManager();

private:
	AutoPointer<MCTSNode> root;
	Player current_player;
	uint max_depth;
	uint root_children_number;
	TimeManager time_manager;
	static const uint default_max_depth;
	static const uint ultimate_depth;
	static const uint visits_to_expand;
};

} // namespace Hex

#endif /* MCTS_TREE_H_ */
