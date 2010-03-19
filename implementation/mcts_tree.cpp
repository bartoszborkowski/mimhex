#include "hextypes.h"
#include "mcts_tree.h"
#include "conditional_assert.h"

namespace Hex {

const uint MCTSTree::default_max_depth = -1;
const uint MCTSTree::default_per_move = 100000;
const uint MCTSTree::ultimate_depth = Dim::field_count;
const uint MCTSTree::visits_to_expand = 10;
const uint MCTSTree::amaf_paths_palyouts = 1000;

MCTSTree::MCTSTree():
    root(NULL),
    current(Player::First()),
    max_depth(default_max_depth),
    per_move(default_per_move) {}

void MCTSTree::Reset(const Board& board) {
    root = new MCTSNode(board);
}

Move MCTSTree::BestMove(Player player, const Board& board) {

    Reset(board);

    ASSERT(!board.IsWon());
    ASSERT(root != NULL);

    /// Temporary board to animate playouts
    Board brd;
    /// Current node in the MCTS tree.
    MCTSNode* node;
    /// All the nodes on the path in the MCTS tree. Valid range: 0..level - 1.
    MCTSNode* nodes[Dim::field_count];
    /// Number of moves made so far.
    uint level = 1;
    /// All the moves made so far. Valid range: 1..level - 1.
    uint history[Dim::field_count];

    nodes[0] = root.GetPointer();
    history[0] = 0; // Some invalid move.

    if (root->IsLeaf()) {
        root->Expand(board);
    }

    if (max_depth == 0)
        node = root->GetChild(Rand::next_rand(brd.MovesLeft()));

    for (uint i = 0; i < per_move; ++i) {

        level = 1;
        node = root.GetPointer();
        brd.Load(board);
        while (!node->IsLeaf()) {
            node = node->SelectChild();
            brd.PlayLegal(Move(brd.CurrentPlayer(), node->loc));
            nodes[level] = node;
            history[level] = node->loc.GetPos();
            ++level;
        }

        if (level < max_depth && node->GetPlayed() >= visits_to_expand
                              && brd.MovesLeft() > 0) {
            node->Expand(brd);
            node = node->SelectChild();
            brd.PlayLegal(Move(brd.CurrentPlayer(), node->loc));
            nodes[level] = node;
            history[level] = node->loc.GetPos();
            ++level;
        }

        Player winner = RandomFinish(brd, history, level);

        bool won = (winner == brd.CurrentPlayer());
        for (int i = level - 1; i >= 0; --i) {
            nodes[i]->Update(won, history + i + 1, history + level);
            won = !won;
        }
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

    std::cerr << ToAsciiArt(4);

    Move best(player, root->SelectBestChild()->loc);
    ASSERT(board.IsValidMove(best));
    return best;
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
