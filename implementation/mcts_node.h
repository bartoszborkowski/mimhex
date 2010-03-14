#ifndef MCTS_NODE_H_
#define MCTS_NODE_H_

#include "hextypes.h"
#include "board.h"
#include "auto_pointer.h"
#include "params.h"

namespace Hex {

/*
 * TODO LIST:
 *
 * (1) Remove ucb_valid flag handling from the main loop inside BestMove().
 *
 * (2) Move as many operations on MCTS nodes from the main loop inside BestMove()
 *     to MCTS node itself as possible.
 *
 * (3) Add Switches to the code.
 *
 * (4) Add PATH AMAF table to the MCTS tree (also to MCTS nodes).
 *
 * (5) Use a single is_valid flag instead of ucb_valid, rave_valid, etc.
 *     Remove separate Compute...() procedures and place the memoization inside
 *     a single Compute() procedure.
 */

class Statistics {
public:
	Statistics(uint start_won, uint start_played)
		: won(start_won), played(start_played) { }
public:
	uint won;
	uint played;
};

class MCTSNode {
public:
	MCTSNode() : uct_stats(Params::initialization, 2 * Params::initialization),
		rave_stats(Params::initialization, 2 * Params::initialization),
		pathsamaf_stats(Params::initialization, 2 * Params::initialization),
		loc(0), valid_ucb(false), valid_rave(false), valid_pathsamaf(false) { }
	MCTSNode(Location location) : uct_stats(Params::initialization, 2 * Params::initialization),
			rave_stats(Params::initialization, 2 * Params::initialization),
			pathsamaf_stats(Params::initialization, 2 * Params::initialization), loc(location),
			valid_ucb(false), valid_rave(false), valid_pathsamaf(false) { }
	MCTSNode* FindBestChild(uint children_number);
	MCTSNode* FindChildToSelect(uint children_number);
	float GetUCB();
	float GetRAVE();
	float GetPATHSAMAF();
	float GetMu();
	float GetRAVEMu();
	float GetPATHSAMAFMu();
	float GetUCBWeight();
	float GetRAVEWeight();
	float GetPATHSAMAFWeight();
	float Eval();
	void Expand(Board& board);
	void RecursivePrint(std::ostream& stream, uint max_children,
			uint current_level, uint children_count, Player player);
    void SetInvalid() {
        valid_ucb = false;
        valid_rave = false;
        valid_pathsamaf = false;
    }
	void SetInvalidUCB() { valid_ucb = false; }
	void SetInvalidRAVE() { valid_rave = false; }
	void SetInvalidPATHSAMAF() { valid_pathsamaf = false; }
	void ComputeUCBStats();
	void ComputeRAVEStats();
	void ComputePATHSAMAFStats();

	friend class MCTSTree;

public:
	Statistics uct_stats;
	Statistics rave_stats;
	Statistics pathsamaf_stats;

private:

	Location loc;
	AutoTable<MCTSNode> children;
	AutoTable<MCTSNode*> pos_to_children_mapping;
	float ucb;
	float rave;
	float pathsamaf;
	float ucb_weight;
	float rave_weight;
	float pathsamaf_weight;
	bool valid_ucb;
	bool valid_rave;
	bool valid_pathsamaf;
};

} // namespace Hex

#endif /* MCTS_NODE_H_ */
