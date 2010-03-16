#include "mcts_node.h"
#include "mcts_tree.h"
#include "inverse_sqrt.h"
#include "conditional_assert.h"
#include <cmath>
#include <vector>
#include <map>

namespace Hex {

Statistics::Statistics(uint init_won, uint init_played):
    won(init_won),
    played(init_played) {}

void Statistics::Won() {
    ++won;
    ++played;
}

void Statistics::Lost() {
    ++played;
}

float Statistics::GetMu() {
    ASSERT(played > 0);
    return static_cast<float>(won) / static_cast<float>(played);
}

float Statistics::GetBound() {
    ASSERT(played > 0);
    return Params::alpha * InverseSqrt(played);
}

float Statistics::GetValue() {
    return GetMu() + GetBound();
}

// -----------------------------------------------------------------------------

MCTSNode::MCTSNode():
    ucb(Params::initialization, 2 * Params::initialization),
    rave(Params::initialization, 2 * Params::initialization),
    path(Params::initialization, 2 * Params::initialization),
    loc(0),
    count(0),
    computed(false) {}

MCTSNode::MCTSNode(Location location):
    ucb(Params::initialization, 2 * Params::initialization),
    rave(Params::initialization, 2 * Params::initialization),
    path(Params::initialization, 2 * Params::initialization),
    loc(location),
    count(0),
    computed(false) {}

MCTSNode* MCTSNode::SelectBestChild() {

    ASSERT(count > 0);

    MCTSNode* best = &children[0];
    float best_mu = best->GetMu();
    for (uint i = 1; i < count; ++i) {
        float mu = children[i].GetMu();
        if (mu > best_mu) {
            best = &children[i];
            best_mu = mu;
        }
    }
    return best;
}

MCTSNode* MCTSNode::SelectChild() {

    ASSERT(count > 0);

    MCTSNode* best = &children[0];
    float best_val = best->Compute();
    for (uint i = 1; i < count; ++i) {
        float val = children[i].Compute();
        if (val > best_val) {
            best = &children[i];
            best_val = val;
        }
    }
    return best;
}

MCTSNode* MCTSNode::GetChild(uint index) {
    ASSERT(index < count);
    return &children[index];
}

Player MCTSNode::GetPlayer() {
    if (count % 2 == 0)
        return Player::Second();
    else
        return Player::First();
}

float MCTSNode::Compute() {
    if (!computed) {
        computed = true;
        value = ucb.GetValue() * GetUcbWeight();
        if (Switches::Rave())
            value += rave.GetValue() * GetRaveWeight();
        if (Switches::PathRave())
            value += path.GetValue() * GetRaveWeight();
        if (Switches::PathAmaf())
            value += path.GetValue() * GetAmafWeight();
    }
    return value;
}

float MCTSNode::GetMu() {
    return ucb.GetMu();
}

uint MCTSNode::GetPlayed() {
    return ucb.played - 2 * Params::initialization;
}

float MCTSNode::GetUcbWeight() {
    ASSERT(ucb.played > 0);
    if (Switches::Rave() || Switches::PathAmaf())
        return static_cast<float>(ucb.played)
               / (Params::beta + static_cast<float>(ucb.played));
    else
        return 1.0f;
}

float MCTSNode::GetRaveWeight() {
    float r = 1.0f - GetUcbWeight();
    if (Switches::PathAmaf() && Switches::Rave())
        r *= Params::gamma;
    return r;
}

float MCTSNode::GetAmafWeight() {
    float r = 1.0f - GetUcbWeight();
    if (Switches::PathAmaf() && Switches::Rave())
        r *= 1.0f - Params::gamma;
    return r;
}

bool MCTSNode::IsLeaf() {
    return children == NULL;
}

void MCTSNode::Expand(Board& board) {
    ASSERT(board.MovesLeft() > 0);
    unsigned short* locations;
    children = new MCTSNode[board.MovesLeft()];
    board.GetPossiblePositions(locations);
    pos_to_children_mapping = new MCTSNode* [Dim::actual_field_count];
    for (uint i = 0; i < board.MovesLeft(); ++i) {
        children[i].loc = locations[i];
        pos_to_children_mapping[locations[i]] = &(children[i]);
        /*amaf_paths:*/
        // FIXME: Nodes take care of themselves
//         children[i].path.won = board.timesOfBeingOnShortestPath[locations[i]];
//         children[i].path.played = MCTSTree::amaf_paths_palyouts;
    }
}

void MCTSNode::Update(const Board& board, uint* begin, uint* end) {

    computed = false;

    // TODO: Untested.
    if (GetPlayer() == Player::First())
        ucb.Won();
    else
        ucb.Lost();

    // TODO: Implement RAVE here. Use Switches.
    /*
    for (int level = board.MovesLeft(); level > 0; --level) {
        uint pos = full_path[level];
        int tree_level = current_level - 1;
        if (tree_level >= level)
            tree_level = level - 1;
        else if (((level + tree_level) & 1) == 0)
            tree_level--;
        if ((tree_level & 1) == 0)
            current = player;
        else current = player.Opponent();
        while (tree_level >= 0) {
            MCTSNode* updated = path[tree_level]->pos_to_children_mapping[pos];
            if (winner == current)
                updated->rave_stats.won++;
            updated->rave_stats.played++;
            updated->SetInvalidRAVE();
            tree_level -= 2;
        }
    }
    */

    // TODO: Implement Path RAVE here. Use Switches.
}

void MCTSNode::ToAsciiArt(std::ostream& stream, uint max_children, uint max_level) {

    RecursivePrint(stream, max_children, max_level, 0, GetPlayer());
}

void MCTSNode::RecursivePrint(std::ostream& stream, uint max_children, uint max_level, uint level, Player player) {

    ASSERT (children == NULL || count > 0);

    if (max_level == 0)
        return;

    for (uint i = 0; i < level; ++i)
        stream << "  ";

    if (loc.GetPos() != 0) {
        if (player == Player::First())
            stream << "O ";
        else stream << "# ";
        stream << loc.ToCoords() << " ";
    } else {
        stream << "root ";
    }

    stream << "ev: " << Compute() << " mu: " << GetMu();
    if (Switches::Rave())
        stream << " rave: " << rave.GetValue();
    if (Switches::PathAmaf())
        stream << " path: " << path.GetValue();

    if ((level & 1) == 0)
        stream << " won: " << static_cast<double> (ucb.played - ucb.won) * 100 / ucb.played << "%";
    else
        stream << " won: " << static_cast<double> (ucb.won) * 100 / ucb.played << "%";\
    stream << " all: " << ucb.played;
    stream << std::endl;

    if (children != NULL) {
        std::map<double, std::vector<uint> > children_indices;
        for (uint i = 0; i < count; ++i) {
            double val = static_cast<double>(children[i].ucb.won) /
                    children[i].ucb.played;
            children_indices[val].push_back(i);
        }
        std::map<double, std::vector<uint> >::const_reverse_iterator it
            = children_indices.rbegin();
        for (uint i = 0; i < std::min(count, max_children); ++it) {
            std::vector<uint>::const_iterator vec_it;
            for (vec_it = it->second.begin(); vec_it != it->second.end() &&
                    i < std::min(count, max_children); ++vec_it, ++i) {
                children[*vec_it].RecursivePrint(
                    stream,
                    max_children,
                    max_level - 1,
                    level + 1,
                    player.Opponent()
                );
            }
        }
    }
}

} // namespace Hex
