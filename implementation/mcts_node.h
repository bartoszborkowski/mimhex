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
 * (1) Remove ucb_valid flag handling from the main loop inside BestMove(). [DONE: julk]
 *
 * (2) Move as many operations on MCTS nodes from the main loop inside BestMove()
 *     to MCTS node itself as possible. [DONE: julk]
 *
 * (3) Add Switches to the code.
 *
 * (4) Add PATH AMAF table to the MCTS tree (also to MCTS nodes).
 *
 * (5) Use a single is_valid flag instead of ucb_valid, rave_valid, etc.
 *     Remove separate Compute...() procedures and place the memoization inside
 *     a single Compute() procedure. [INVALID: julk]
 */

/**
 * A class for maintaining single set of statistics. Performs simple memoization
 * in order to speed up GetValue() method. No memoization is done for
 * GetValue() or GetBound() methods alone.
 */
class Statistics {
    public:
        Statistics(uint init_won, uint init_played);

        /**
         * @return The mu factor, that is won / played
         */
        float GetMu() const;

        /**
         * @return The confidence bound for the current played value.
         */
        float GetBound() const;

        /**
         * @return The value increased by the confidence bound.
         * @note This method is memoized.
         */
        float GetValue() const;

        uint GetWon() const;
        uint GetLost() const;
        uint GetPlayed() const;

        /**
         * Update statistics to include another won game.
         */
        void Win();

        /**
         * Update statistics to include another lost game.
         */
        void Lose();

        /**
         * @in @param won True if the game was won.
         * Update statistics to include another game.
         */
        void Update(bool won);

    private:
        uint won;
        uint played;

        /**
         * The field used for memoization of the GetValue() function.
         */
        mutable float value;

        /**
         * True if field value is valid.
         */
        mutable bool computed;
};

class MCTSNode {
    public:
        /**
         * Perform minimal initialization.
         */
        MCTSNode();

        /**
         * Initialize as the root for a tree for the given board.
         * @in @param board
         */
        MCTSNode(const Board& board);

        /**
         * @return The child node for the best move.
         */
        MCTSNode* SelectBestChild() const;

        /**
         * @return The child node for the optimal exploration move.
         */
        MCTSNode* SelectChild() const;

        /**
         * @in @param pos A position on the board. Must be empty.
         * @return The child node for the given position.
         */
        MCTSNode* GetChildByPos(uint pos) const;

        /**
         * FIXME: This isn't very efficient at the moment.
         * @return The player whose move this node represents.
         */
        Player GetPlayer() const;

        Move GetMove() const;

        /**
         * @return The number of games played for this node.
         */
        uint GetPlayed() const;

        /**
         * @return True if the node has no children.
         */
        bool IsLeaf() const;

        /**
         * Expand this node according to the given board situation.
         * @in @param board
         */
        void Expand(const Board& board);

        /**
         * Update the node statistics according to the described game.
         * @note This may also update child node statistics if RAVE is enabled.
         * @in @param won Was the game won.
         * @in @param begin The begin of an array containing positions played
         * until the end of the game.
         * @in @param end The first position after the end of an array
         * containing positions played until the end of the game.
         */
        void Update(bool won, uint* begin, uint* end);

        void ToAsciiArt(std::ostream& stream, uint max_children, uint max_level, Player player) const;

    private:
        /**
         * Update RAVE statistics to include another game.
         * @in @param won True if the game was won.
         */
        void Update(bool won);

        /**
         * @return The current move assessment as used for exploration.
         * @note This method is memoized both directly through value and
         * computed fields and indirectly through memoiziation used in the
         * Statistics class.
         */
        float GetValue() const;

        /**
         * @return The current move final assessment.
         * @note This method is NOT memoized directly however some memoization
         * is done inside the Statistics class.
         */
        float Best() const;

        /**
         * @return The weight for UCB part of the formula.
         */
        float GetUcbWeight() const;

        /**
         * @return The weight for RAVE part of the formula. Currently used for
         * both RAVE and PATH-RAVE variants.
         */
        float GetRaveWeight() const;

        /**
         * @return The weight for PATH-AMAF part of the formula. Could be also
         * used for regular AMAF variant.
         */
        float GetAmafWeight() const;

        void RecursivePrint(std::ostream& stream, uint max_children, uint max_level, uint level, Player player) const;

    private:
        Statistics ucb;
        Statistics rave;
        Statistics path;

        /**
         * The location of the move represented by this node.
         */
        Location loc;

        /**
         * The number of child moves for this node. Valid even if no actual
         * children have been allocated yet. This field is also used for
         * determining player turn.
         */
        uint count;

        /**
         * Children array of length count.
         */
        AutoTable<MCTSNode> children;

        /**
         * A dictionary converting positions into valid children of the node.
         */
        AutoTable<MCTSNode*> pos_to_child;

        /**
         * A field used for memoization of Compute() method.
         */
        mutable float value;

        /**
         * True if the field value is valid.
         */
        mutable bool computed;
};

} // namespace Hex

#endif /* MCTS_NODE_H_ */
