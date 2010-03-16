#include "hextypes.h"
#include "mcts_tree.h"
#include "conditional_assert.h"

namespace Hex {

const uint MCTSTree::default_max_depth = -1;
const uint MCTSTree::default_per_move = 100000;
const uint MCTSTree::ultimate_depth = Dim::field_count;
const uint MCTSTree::visits_to_expand = 10;
const uint MCTSTree::amaf_paths_palyouts = 1000;

inline MCTSTree::MCTSTree():
    current(Player::First()) {

    Reset();
    max_depth = default_max_depth;
    per_move = default_per_move;
}

void MCTSTree::Reset() {
    root = new MCTSNode();
    root_children_number = 0;
}

Move MCTSTree::BestMove(Player player, Board& board) {

    Reset();

    ASSERT(!board.IsWon());
    ASSERT(root != NULL);

    Board brd;
    MCTSNode* node;
    MCTSNode* nodes[Dim::field_count];
    uint level;
    uint history[Dim::field_count];

    nodes[0] = root.GetPointer();

    if (root->IsLeaf()) {
        root->Expand(board);
    }

    if (max_depth == 0)
        node = root->GetChild(Rand::next_rand(brd.MovesLeft()));

    for (uint i = 0; i < per_move; ++i) {

        level = 0;
        node = root.GetPointer();
        brd.Load(board);
        while (!node->IsLeaf()) {
            node = node->SelectChild();
            brd.PlayLegal(Move(brd.CurrentPlayer(), node->loc));
            nodes[++level] = node;
            history[level] = node->loc.GetPos();
        }

        if (level < max_depth && node->GetPlayed() >= visits_to_expand
                              && brd.MovesLeft() > 0) {
            node->Expand(brd);
            node = node->SelectChild();
            brd.PlayLegal(Move(brd.CurrentPlayer(), node->loc));
            nodes[++level] = node;
            history[level] = node->loc.GetPos();
        }

        Player player = brd.CurrentPlayer();
        Player winner = RandomFinish(brd, history, level);

        for (int i = level; i >= 0; --i)
            nodes[i]->Update(brd, history + i, history + level);

        // TODO: Scheduled for moving to MCTSNode::Update() (already partially
        // moved.
        /*
        for (int i = level; i >= 0; --i) {
            if (winner != current)
                path[level]->uct_stats.won++;
            else
                path[level]->uct_stats.played++;
            path[level]->SetInvalidUCB();
            current = current.Opponent();
        }
        */

        // TODO: Scheduled for moving to MCTSNode::Update().
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
    }

    // TODO: Scheduled for moving to MCTSNode::Update()
    /*
    for(uint i=0; i<amaf_paths_palyouts; i++){
        brd.Load(board);
        Player winner = RandomFinishWithoutPath(brd);
        brd.UpdatePathsStatsAllShortestPathsBFS(board,winner);
//        board.ShowPathsStats();
    }
    */

    MCTSNode* best = root->SelectBestChild();
    player = player.Opponent();

    return Move(player, best->loc);
}

Player MCTSTree::RandomFinish(Board& board, uint* path, uint current_level) {

    while (!board.IsFull()) {
      Player pl = board.CurrentPlayer();
      Move move = board.GenerateMoveUsingKnowledge(pl);
      path[++current_level] = move.GetLocation().GetPos();
      board.PlayLegal(move);
    }

    return board.Winner();
}

Player MCTSTree::RandomFinishWithoutPath(Board& board) {

    while (!board.IsFull()) {
      Player pl = board.CurrentPlayer();
      Move move = board.GenerateMoveUsingKnowledge(pl);
      board.PlayLegal(move);
    }

    return board.Winner();
}

void MCTSTree::SetMaxDepth(uint depth) {
    if (depth > ultimate_depth)
        max_depth = ultimate_depth;
    else max_depth = depth;
}

void MCTSTree::SetPerMove(uint playouts) {
    per_move = playouts;
}

std::string MCTSTree::ToAsciiArt(uint children) {
    std::stringstream stream;
    // FIXME
    root->ToAsciiArt(stream, children, -1);
    return stream.str();
}

} // namespace Hex
