#include <cmath>
#include <vector>
#include <map>
#include "conditional_assert.h"
#include "inverse_sqrt.h"
#include "mcts_stats.h"
#include "mcts_node.h"

namespace Hex {

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

MCTSNode* MCTSNode::SelectBestChild() const {

    ASSERT(count > 0);

    MCTSNode* best = &children[0];
    float best_mu = best->Best();
    for (uint i = 1; i < count; ++i) {
        float mu = children[i].Best();
        if (mu > best_mu) {
            best = &children[i];
            best_mu = mu;
        }
    }
    return best;
}

MCTSNode* MCTSNode::SelectChild() const {

    ASSERT(count > 0);

    MCTSNode* best = &children[0];
    float best_val = best->GetValue();
    for (uint i = 1; i < count; ++i) {
        float val = children[i].GetValue();
        if (val > best_val) {
            best = &children[i];
            best_val = val;
        }
    }
    return best;
}

MCTSNode* MCTSNode::GetChildByPos(uint pos) const {
    ASSERT(pos < Dim::actual_field_count);
    ASSERT(pos_to_child[pos] != NULL);
    return pos_to_child[pos];
}

Player MCTSNode::GetPlayer() const {
    // TODO: This could be optimized.
    ASSERT(count > 0 || IsLeaf());
    if (count % 2 == Dim::field_count % 2)
        return Player::Second();
    else
        return Player::First();
}

Move MCTSNode::GetMove() const {
    return Move(GetPlayer(), loc);
}

float MCTSNode::GetValue() const {
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

float MCTSNode::Best() const {
    float r = ucb.GetMu() * GetUcbWeight();
    if (Switches::Rave())
        r += rave.GetMu() * GetRaveWeight();
    if (Switches::PathRave())
        r += path.GetMu() * GetRaveWeight();
    if (Switches::PathAmaf())
        r += path.GetMu() * GetAmafWeight();
    return r;
}

uint MCTSNode::GetPlayed() const {
    return ucb.GetPlayed() - 2 * Params::initialization;
}

float MCTSNode::GetUcbWeight() const {
    ASSERT(ucb.GetPlayed() > 0);
    /// Use increasing UCB weight if any RAVE or AMAF technique is used.
    if (Switches::Rave() || Switches::PathRave() || Switches::PathAmaf())
        return static_cast<float>(ucb.GetPlayed())
               / (Params::beta + static_cast<float>(ucb.GetPlayed()));
    else
        return 1.0f;
}

float MCTSNode::GetRaveWeight() const {
    ASSERT(Switches::PathRave() || Switches::Rave());
    /// UCB, RAVE and AMAF weights sum to 1.0.
    float r = 1.0f - GetUcbWeight();
    if (Switches::PathAmaf())
        r *= Params::gamma;
    return r;
}

float MCTSNode::GetAmafWeight() const {
    ASSERT(Switches::PathAmaf());
    /// UCB, RAVE and AMAF weights sum to 1.0.
    float r = 1.0f - GetUcbWeight();
    if (Switches::PathRave() || Switches::Rave())
        r *= 1.0f - Params::gamma;
    return r;
}

bool MCTSNode::IsLeaf() const {
    return children == NULL;
}

void MCTSNode::Expand(const Board& board) {
    ASSERT(board.MovesLeft() == count);
    ASSERT(count > 0);
    ASSERT(children == NULL);
    const ushort* empty = board.GetEmpty();
    /// Allocate actual children nodes.
    children = new MCTSNode[count];
    /// Allocate the position dictionary.
    pos_to_child = new MCTSNode*[Dim::actual_field_count];
    for (uint i = 0; i < count; ++i) {
        /// Fill in uninitialized fields of a child
        children[i].loc = empty[i];
        children[i].count = count - 1;
        /// Fill in dictionary entry.
        pos_to_child[empty[i]] = &children[i];
    }
}

void MCTSNode::Update(bool won) {

    computed = false;
    rave.Update(won);
}

void MCTSNode::Update(bool won, uint* begin, uint* end) {

    computed = false;
    ucb.Update(won);

    if (Switches::Rave() && !IsLeaf()) {

        ASSERT(end - begin == count);
        /// Iterate over opponent moves.
        for (uint* it = begin; it != end && it != end + 1; it += 2) {
            ASSERT(*it != loc.GetPos());
            /// Update an immediate child.
            GetChildByPos(*it)->Update(!won);
        }
    }

    // TODO: Implement PATH-RAVE here. Use Switches.
}

void MCTSNode::ToAsciiArt(std::ostream& stream, uint max_children, uint max_level, Player pl) const {

    RecursivePrint(stream, max_children, max_level, 0, pl);
}

void MCTSNode::RecursivePrint(std::ostream& stream, uint max_children, uint max_level, uint level, Player player) const {

    ASSERT (IsLeaf() || count > 0);

    if (max_level == 0)
        return;

    for (uint i = 0; i < level; ++i)
        stream << "  ";

    if (loc.GetPos() != 0) {
        if (GetPlayer() == Player::First())
            stream << "# ";
        else
            stream << "O ";
        stream << loc.ToCoords() << " ";
    } else {
        stream << "root ";
    }

    if (GetPlayer() == player) {
        stream << "ev: " << 1.0f - GetValue() << " mu: " << 1.0f - ucb.GetMu();
        stream << " won: " << 1.0f - ucb.GetMu() * 100.0f << "%";
        if (Switches::Rave())
            stream << " rave: " << 1.0f - rave.GetMu();
        if (Switches::PathAmaf())
            stream << " path: " << 1.0f - path.GetMu();
    } else {
        stream << "ev: " << GetValue() << " mu: " << ucb.GetMu();
        stream << " won: " << ucb.GetMu() * 100.0f << "%";\
        if (Switches::Rave())
            stream << " rave: " << rave.GetMu();
        if (Switches::PathAmaf())
            stream << " path: " << path.GetMu();
    }
    stream << " all: " << ucb.GetPlayed();
    stream << std::endl;

    if (children != NULL) {
        std::map<double, std::vector<uint> > children_indices;
        for (uint i = 0; i < count; ++i) {
            double val = children[i].Best();
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
                    player
                );
            }
        }
    }
}

} // namespace Hex
