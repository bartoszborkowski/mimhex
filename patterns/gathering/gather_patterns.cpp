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
            uses[pattern]++;
            ++used_patterns;
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

    board = new HashBoard_13;
    
    initPlayed();
  }
    
  ~GtpController(){
      delete board;
  }
private:
  void CNewGame(Gtp::Io &) {
    delete board;
    board = new HashBoard_13(HashBoard_13::EmptyHashBoard());
    initPlayed();
  }
  
  void initPlayed(){
    //set all to false
    memset(played, 0, Hex::kBoardSizeAligned * Hex::kBoardSizeAligned * sizeof(bool)); 
    
    
    //TODO here is initialization of GUARDS but only 1 line!! Now we have two line of GUARDS!
    // initialize GUARDS as true
    for (uint i = 0; i < Hex::kBoardSizeAligned; ++i) {
      played[i] = played[Hex::kBoardSizeAligned * Hex::kBoardSizeAligned - 1 - i] = true;
    }
    // TODO two lines of GUARDS
    // initialize GUARDS as true
    for (uint i = 1; i <= Hex::kBoardSize; ++i) {
      played[i * Hex::kBoardSizeAligned] = played[(i+1) * Hex::kBoardSizeAligned - 1] = true;
    }  
  }

  void CPlay(Gtp::Io& io) {
      std::string playerStr;
      std::string locCoordsStr;

      io.in >> playerStr;
      io.in >> locCoordsStr;
      
      Hex::Player player = Hex::Player::OfString(playerStr);
      uint location = HashBoard_13::GetLocation(locCoordsStr);

      //Hex::Location location = Hex::Location::OfCoords(locCoordsStr);
      //stara werja - nie można użyc bo zmieniony rozmiar planszy do zbierania patternów

      const uint *allBoardHashes = board->GetAllHashes();
      size_t allBoardHashesSize = board->GetBoardSize();

      uint playHash = board->GetHash(location);

      statsComp.reportPatternUse(&playHash, 1, allBoardHashes, allBoardHashesSize, played);
		
		board->Play(location, player.GetVal());
		played[location] = true;
  }

  void CPrint(Gtp::Io& io){
      statsComp.print(io.out);
  }

  HashBoard_13 *board;
  StatsComputerType statsComp;
  bool played[Hex::kBoardSizeAligned * Hex::kBoardSizeAligned];
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
