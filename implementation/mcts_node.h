#ifndef MCTS_NODE_H_
#define MCTS_NODE_H_

#include "board.h"
#include "auto_pointer.h"
#include "params.h"

namespace Hex {

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
		loc(0), valid_ucb(false), valid_rave(false) { }
	MCTSNode(Location location) : uct_stats(Params::initialization, 2 * Params::initialization),
			rave_stats(Params::initialization, 2 * Params::initialization),
			pathsamaf_stats(Params::initialization, 2 * Params::initialization), loc(location),
			valid_ucb(false), valid_rave(false) { }
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
