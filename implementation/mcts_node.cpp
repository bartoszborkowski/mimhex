#include "mcts_node.h"
#include "mcts_tree.h"
#include "inverse_sqrt.h"
#include "conditional_assert.h"
#include <cmath>
#include <vector>
#include <map>

namespace Hex {

/*finds child that should be chosen by UCT*/

MCTSNode* MCTSNode::FindChildToSelect(uint children_number) {
	ASSERT(children_number > 0);
	MCTSNode* best = &(children[0]);
	float best_val = best->Eval();
	for (uint i = 1; i < children_number; ++i) {
		float current_val = children[i].Eval();
		if (current_val > best_val) {
			best = &(children[i]);
			best_val = current_val;
		}
	}
	return best;
}


float MCTSNode::Eval() {
	float r = GetUCB() * GetUCBWeight();
    if (Switches::Rave())
        r += GetRAVE() * GetRAVEWeight();
    if (Switches::PathAmaf())
        r += GetPATHSAMAF() * GetPATHSAMAFWeight();
    return r;
}

/*finds child that should be played*/

MCTSNode* MCTSNode::FindBestChild(uint children_number) {
	ASSERT(children_number > 0);
	MCTSNode* best = &(children[0]);
	float best_mu = best->GetMu();
	for (uint i = 1; i < children_number; ++i) {
		float current_mu = children[i].GetMu();
		if (current_mu > best_mu) {
			best = &(children[i]);
			best_mu = current_mu;
		}
	}
	return best;
}

inline float MCTSNode::GetMu() {
	ASSERT(uct_stats.played > 0);
	return static_cast<float>(uct_stats.won) / static_cast<float>(uct_stats.played);
}

inline float MCTSNode::GetRAVEMu() {
	ASSERT(rave_stats.played > 0);
	return static_cast<float>(rave_stats.won) / static_cast<float>(rave_stats.played);
}

inline float MCTSNode::GetPATHSAMAFMu() {
	ASSERT(pathsamaf_stats.played > 0);
	return static_cast<float>(pathsamaf_stats.won) / static_cast<float>(pathsamaf_stats.played);
}

inline float MCTSNode::GetUCB() {
	ComputeUCBStats();
	return ucb;
}

inline float MCTSNode::GetRAVE() {
	ComputeRAVEStats();
	return rave;
}

inline float MCTSNode::GetPATHSAMAF() {
	ComputePATHSAMAFStats();
	return pathsamaf;
}

float MCTSNode::GetUCBWeight() {
	ComputeUCBStats();
	return ucb_weight;
}

inline void MCTSNode::ComputeUCBStats() {
	if (valid_ucb) return;
    valid_ucb = true;
    if (Switches::Rave() || Switches::PathAmaf())
        ucb_weight = static_cast<float>(uct_stats.played) /
                    (Params::beta + static_cast<float>(uct_stats.played));
    else
        ucb_weight = 1.0f;
	ucb = GetMu() + (Params::alpha * InverseSqrt(uct_stats.played));
}

float MCTSNode::GetRAVEWeight() {
	ComputeRAVEStats();
	return rave_weight;
}

inline void MCTSNode::ComputeRAVEStats() {
    ASSERT(Switches::Rave());
	if (valid_rave) return;
    valid_rave = true;
    if (Switches::PathAmaf() && Switches::Rave())
        rave_weight = (1.0 - ucb_weight)/2.0;
    else
        rave_weight = 1.0 - ucb_weight;
	rave = GetRAVEMu() + (Params::alpha * InverseSqrt(rave_stats.played));
}

float MCTSNode::GetPATHSAMAFWeight() {
	ComputePATHSAMAFStats();
	return pathsamaf_weight;
}

inline void MCTSNode::ComputePATHSAMAFStats() {
    ASSERT(Switches::PathAmaf());
	if (valid_pathsamaf) return;
    valid_pathsamaf = true;
    if (Switches::PathAmaf() && Switches::Rave())
        pathsamaf_weight = (1.0 - ucb_weight)/2.0;
    else
        pathsamaf_weight = 1.0 - ucb_weight;
	pathsamaf = GetPATHSAMAFMu(); //?? + (Params::alpha * InverseSqrt(pathsamaf_stats.played));
}

inline void MCTSNode::Expand(Board& board) {
	ASSERT(board.MovesLeft() > 0);
	unsigned short* locations;
	children = new MCTSNode[board.MovesLeft()];
	board.GetPossiblePositions(locations);
	pos_to_children_mapping = new MCTSNode*[kBoardSizeAligned * kBoardSizeAligned];
	for (uint i = 0; i < board.MovesLeft(); ++i) {
		children[i].loc = locations[i];
		pos_to_children_mapping[locations[i]] = &(children[i]);
		/*amaf_paths:*/
		children[i].pathsamaf_stats.won = board.timesOfBeingOnShortestPath[locations[i]];
		children[i].pathsamaf_stats.played = MCTSTree::amaf_paths_palyouts;
		children[i].valid_pathsamaf=false;
	}
}

void MCTSNode::RecursivePrint(std::ostream& stream, uint max_children,
		uint current_level, uint children_count, Player player) {

	ASSERT (children == NULL || children_count > 0);

	for (uint i = 0; i < current_level; ++i)
		stream << "  ";

	if (loc.GetPos() != 0) {
		if (player == Player::First())
			stream << "O ";
		else stream << "# ";
		stream << loc.ToCoords() << " ";
	} else {
		stream << "root ";
	}

	stream << "EVAL: "<<Eval()<<" GETMU "<<GetMu()<<" ";

	if ((current_level & 1) == 0) {
		stream << static_cast<double>(uct_stats.played - uct_stats.won)
				* 100 / uct_stats.played<<' '<<uct_stats.played<<' '<<uct_stats.won;
	} else {
		stream << static_cast<double>(uct_stats.won) * 100 / uct_stats.played;
	}
	stream << " " << uct_stats.played;
	stream << std::endl;

	if (children != NULL) {
		std::map<double, std::vector<uint> > children_indices;
		for (uint i = 0; i < children_count; ++i) {
			double val = static_cast<double>(children[i].uct_stats.won) /
					children[i].uct_stats.played;
			children_indices[val].push_back(i);
		}
		std::map<double, std::vector<uint> >::const_reverse_iterator it
			= children_indices.rbegin();
		for (uint i = 0; i < std::min(children_count, max_children); ++it) {
			std::vector<uint>::const_iterator vec_it;
			for (vec_it = it->second.begin(); vec_it != it->second.end() &&
					i < std::min(children_count, max_children); ++vec_it, ++i) {
				children[*vec_it].RecursivePrint(stream, max_children,
					current_level + 1, children_count - 1, player.Opponent());
			}
		}
	}
}

} // namespace Hex
