/* A program to gather play patterns
 *
 * The program uses the following GTP commands through stdin/stdout:
 * 
 * - newgame
 * - play <color> <position>
 * - print
 *
 */

#include <map>

#include <iostream>
#include <inttypes.h>

#include "board.cpp"
#include "board_move.cpp"
#include "board_location.cpp"
#include "board_player.cpp"
#include "board_dim.cpp"
#include "random.cpp"
#include "params.cpp"
#include "switches.cpp"
#include "gtp.hpp"
#include "gtp.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "hash_board_13.cpp"
#include "sampler.cpp"

#include "pattern_data.cpp"

using HexPatterns::HashBoard_13;

/* StatsComputer concept:
 *
 * void reportPatternUse(const uint *used_patterns, size_t n_used_patterns, const uint *existingPatterns, size_t n_existing_patterns, const bool *played_positions);
 * report that on a board with a set of existingPatterns, a set usedPatterns has been played.
 *
 * void print(std::ostream &out);
 * prints the results
 *
 */

uint round;
Hex::Player save_player = Hex::Player::First();
uint save_location;

struct SimpleStatsComputer{
  
    SimpleStatsComputer(){
        uses.clear();
        occurences.clear();
    }

    void reportPatternUse(const uint *used_patterns, size_t n_used_patterns, const uint *existing_patterns, 
                size_t n_existing_patterns, const bool *played_positions){
      

        const uint *used_patterns_end = used_patterns + n_used_patterns;
        while (used_patterns != used_patterns_end){
            uint pattern = *used_patterns;
                ++used_patterns;
            uses[pattern]++;
// the important part: outputting the chosen pattern hash
                std::cerr << pattern;

        }

        const uint *existing_patterns_end = existing_patterns + n_existing_patterns;
        while (existing_patterns != existing_patterns_end){
            if (!*played_positions){
            	uint pattern = *existing_patterns;
                occurences[pattern]++;
                //the important part: outputting all patterns present
				std::cerr << " " << pattern;

            }
            ++existing_patterns;
            ++played_positions;
        }

			std::cerr << std::endl;
    }

    void print(std::ostream &out){
        map<uint, uint32_t>::iterator it = occurences.begin();
        int i = 0;
        while (it != occurences.end()){
            uint hash = it->first;
            if (occurences[hash] != 0){
                out << hash << ": " << uses[hash] << " / " << occurences[hash] << std::endl;
            }
            ++it;
            ++i;
        }
    }
private:
    //void increment(const uint *dataBegin, const uint *dataEnd, uint32_t *array){}
    map<uint, uint32_t> occurences;
    map<uint, uint32_t> uses;
};

template<typename StatsComputerType>
struct GtpController {

  GtpController(Gtp::Repl& gtp){
    gtp.Register("newgame"       , this, &GtpController::CNewGame);
    gtp.Register("play"          , this, &GtpController::CPlay);
    gtp.Register("print"         , this, &GtpController::CPrint);
    gtp.Register("swap"         , this, &GtpController::CSwap);

    board = new HashBoard_13(HashBoard_13::EmptyHashBoard());
    initPlayed();
    round = 1;
  }
    
  ~GtpController(){
      delete board;
  }
private:
  void CNewGame(Gtp::Io &) {
    // empty just because
  }

  // Function draws played array. Use only for debug.
    void printPlayed(){
        for (uint i=0; i<Hex::Dim::actual_size; ++i){
            for (uint j=0; j<Hex::Dim::actual_size; ++j){
                if (played[i*Hex::Dim::actual_size+j])
                    std::cout << "t ";
                else
                    std::cout <<". ";
            }
            std::cout << std::endl;
        }
    }
  
    void initPlayed(){
        memset(played, 0, Hex::Dim::actual_size * Hex::Dim::actual_size * sizeof(bool));
    
        // initialize top and bottom  line of GUARDS as already played
        // and all unused space
        for (uint i=0; i<FULL_SIZE; ++i) {
            played[i] = played[Hex::Dim::actual_size * (FULL_SIZE - 1) + i] = true;
        }
    
        // initialize left and right GUARDS as already played
        for (uint i=0; i<FULL_SIZE; ++i) {
            played[i * Hex::Dim::actual_size] = played[i * Hex::Dim::actual_size +FULL_SIZE - 1] = true;
        }
    
        //initialize empty area after guards as already played
        for (uint i=FULL_SIZE; i<Hex::Dim::actual_size; ++i){
            for (uint j=0; j<Hex::Dim::actual_size; ++j){
                played[j*Hex::Dim::actual_size + i] = played [i*Hex::Dim::actual_size+j] = true;
            }
        }
    
        printPlayed();
    }

    void UpdateBoard(uint location, Hex::Player player){
        const uint *allBoardHashes = board->GetAllHashes();
        size_t allBoardHashesSize = board->GetBoardSize();
        uint playHash = board->GetHash(location);
    
        statsComp.reportPatternUse(&playHash, 1, allBoardHashes, allBoardHashesSize, played);
    
        board->Play(location, player.GetVal());
        played[location] = true;
    
        return;
    }

    void CPlay(Gtp::Io& io)
    {
        std::string playerStr;
        std::string locCoordsStr;

        io.in >> playerStr;
        io.in >> locCoordsStr;

        Hex::Player player = Hex::Player::OfString(playerStr);
        uint location = HashBoard_13::GetLocation(locCoordsStr);

        switch (round) {
            case 1: // first move - just save, don't put at board
                save_player = player;
                save_location = location;
                break;
            case 2: // second move - no swap
                UpdateBoard(save_location, save_player);
                UpdateBoard(location, player);
                break;
            default:
                UpdateBoard(location, player);
                break;
        }

        ++round;
  }

    void CSwap(Gtp::Io&){
        ++round;
        ASSERT(round == 2);
        /* change save_location */
        /* the following is a mirror with respect to the long diagonal */
        save_location = ((save_location & 15) << 4) + (save_location >> 4);
        /* change save_player */
        save_player = save_player.Opponent();
        UpdateBoard(save_location, save_player);
    }

  void CPrint(Gtp::Io& io){
      statsComp.print(io.out);
  }

  HashBoard_13 *board;
  StatsComputerType statsComp;
  bool played[Hex::Dim::actual_size * Hex::Dim::actual_size];
};

int main(int, char**)
try {
    Gtp::Repl gtp;
    GtpController<SimpleStatsComputer> gtpc(gtp);
    gtp.Run(std::cin, std::cout);
    return 0;
} catch (std::exception &e) {
    std::cout << "exception caught: " << e.what();
    return 1;
}
