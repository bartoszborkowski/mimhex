#include "game.h"
#include "board.h"
#include "mcts_tree.h"
#include "conditional_assert.h"

namespace Hex {

Game::Game() : empty_board(Board::Empty()), last_move(0) {
    ClearBoard();
}

void Game::ClearBoard() {
    current_board.Load(empty_board);
    tree.Reset(current_board);
}

void Game::Play(const Move& move) {
    ASSERT(current_board.IsValidMove(move));
    current_board.PlayLegal(move);
    last_move = move.GetLocation();

//  FIXME remove this later
//  cerr << current_board.ToAsciiArt(last_move);
}

Move Game::GenMove() {
    ASSERT(!current_board.IsFull());
    return tree.BestMove(current_board);
}

void Game::SetMaxUTCTreeDepth(uint depth) {
    tree.SetMaxDepth(depth);
}

void Game::PrintBoard(std::string& board) {
    board = current_board.ToAsciiArt(last_move);
}

void Game::PrintTree(std::string& ascii_tree, uint children) {
    ascii_tree = tree.ToAsciiArt(children);
}

bool Game::IsValidMove(const Move& move) {
    return current_board.IsValidMove(move);
}

bool Game::IsFinished() {
    // TODO better check
    return current_board.IsFull();
}

void Game::setDefendingBridges(bool v){
    Switches::SetDefendingBridges(v);
}
void Game::setAvoidingBridges(bool v){
    Switches::SetAvoidingBridges(v);
}

Player Game::CurrentPlayer() {
    return current_board.CurrentPlayer();
}

Player Game::nowWinner() {
    if (current_board.IsWon())
        return current_board.Winner();
    else
        return Player::None();
}

TimeManager & Game::GetTimeManager() {
    return tree.GetTimeManager();
}


} // namespace Hex
