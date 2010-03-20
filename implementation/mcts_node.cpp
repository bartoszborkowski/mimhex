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

void Statistics::Update(bool b) {
    won += (1 & b);
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
    children(NULL),
    computed(false) {}

MCTSNode::MCTSNode(const Board& board):
    ucb(Params::initialization, 2 * Params::initialization),
    rave(Params::initialization, 2 * Params::initialization),
    path(Params::initialization, 2 * Params::initialization),
    loc(0),
    count(board.MovesLeft()),
    children(NULL),
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

MCTSNode* MCTSNode::GetChildByPos(uint pos) {
    ASSERT(pos < Dim::actual_field_count);
    ASSERT(pos_to_child[pos] != NULL);
    return pos_to_child[pos];
}

Player MCTSNode::GetPlayer() {
    // FIXME: This could be optimized.
    ASSERT(count > 0 || IsLeaf());
    if (count % 2 == (Dim::field_count - 1) % 2)
        return Player::First();
    else
        return Player::Second();
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
    ASSERT(Switches::PathRave() || Switches::Rave());
    float r = 1.0f - GetUcbWeight();
    if (Switches::PathAmaf() && Switches::Rave())
        r *= Params::gamma;
    return r;
}

float MCTSNode::GetAmafWeight() {
    ASSERT(Switches::PathAmaf());
    float r = 1.0f - GetUcbWeight();
    if (Switches::PathAmaf() && Switches::Rave())
        r *= 1.0f - Params::gamma;
    return r;
}

bool MCTSNode::IsLeaf() {
//     std::cerr << "count: " << count << std::endl;
    return children == NULL;
}

void MCTSNode::Expand(const Board& board) {
    ASSERT(board.MovesLeft() == count);
    ASSERT(count > 0);
    ASSERT(children == NULL);
    const ushort* empty = board.GetEmpty();
    children = new MCTSNode[count];
    pos_to_child = new MCTSNode*[Dim::actual_field_count];
    for (uint i = 0; i < count; ++i) {
        children[i].loc = empty[i];
        children[i].count = count - 1;
        pos_to_child[empty[i]] = &children[i];
        /*amaf_paths:*/
        // FIXME: Nodes take care of themselves
//         children[i].path.won = board.timesOfBeingOnShortestPath[locations[i]];
//         children[i].path.played = MCTSTree::amaf_paths_palyouts;
    }
}

void MCTSNode::Update(bool won, uint* begin, uint* end) {

    computed = false;

    ucb.Update(won);

    if (Switches::Rave() && !IsLeaf()) {

        ASSERT(end - begin == count);
        for (uint* it = begin; it != end; ++it) {
            won = !won;
            GetChildByPos(*it)->rave.Update(won);
        }
    }

    // TODO: Implement Path RAVE here. Use Switches.
    /*
    for(uint i=0; i<amaf_paths_palyouts; i++){
        brd.Load(board);
        Player winner = RandomFinishWithoutPath(brd);
        brd.UpdatePathsStatsAllShortestPathsBFS(board,winner);
//        board.ShowPathsStats();
    }
    */
}

void MCTSNode::ToAsciiArt(std::ostream& stream, uint max_children, uint max_level) {

    RecursivePrint(stream, max_children, max_level, 0, GetPlayer());
}

void MCTSNode::RecursivePrint(std::ostream& stream, uint max_children, uint max_level, uint level, Player player) {

    ASSERT (IsLeaf() || count > 0);

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
