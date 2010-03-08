/* A program to gather play patterns
 *
 * The program uses the following GTP commands through stdin/stdout:
 * 
 * - newgame
 * - play <color> <position>
 * - print
 *
 */

#include "gtp.hpp"
#include "gtp.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"

#include <iostream>
#include <inttypes.h>

// prints 7-elem hash
void printHash(uint hash, std::ostream &o)
{
#define GETNUM(x, pos) ".#oX"[(((x) >> (pos)) & 15) >> 2]

    o << " " << GETNUM(hash, 10) << " " << GETNUM(hash, 0) << std::endl
      << GETNUM(hash, 8) << " " << ".#oX"[hash & 3] << " " << GETNUM(hash, 2) << std::endl
      << " " << GETNUM(hash, 6) << " " << GETNUM(hash, 4) << std::endl;
}

using HexPatterns::HashBoard;

/* StatsComputer concept:
 *
 * void reportPatternUse(const uint *used_patterns, size_t n_used_patterns, const uint *existingPatterns, size_t n_existing_patterns, const bool *played_positions);
 * report that on a board with a set of existingPatterns, a set usedPatterns has been played.
 *
 * void print(std::ostream &out);
 * prints the results
 *
 */
template<size_t MaxPattern>
struct SimpleStatsComputer
{
    SimpleStatsComputer()
    {
        for (size_t i = 0; i <= MaxPattern; i++){
            _occurences[i] = 0;
            _uses[i] = 0;
        }
    }

    void reportPatternUse(const uint *used_patterns, size_t n_used_patterns, const uint *existing_patterns, size_t n_existing_patterns, const bool *played_positions, uint player)
    {
        const uint *used_patterns_end = used_patterns + n_used_patterns;
        while (used_patterns != used_patterns_end){
            if (*used_patterns > MaxPattern){
                std::cerr << "FATAL: *used_patterns = " << *used_patterns << " ; MaxPattern = " << MaxPattern << std::endl;
                exit(1);
            }
            uint pattern = *used_patterns;
            _uses[pattern]++;
            _occurences[pattern]++;
            _occurences[(pattern & (~3)) | (3 - (pattern & 3))]++;
            ++used_patterns;

// the important part: outputting the chosen pattern hash
				std::cerr << pattern;

        }

        const uint *existing_patterns_end = existing_patterns + n_existing_patterns;
        while (existing_patterns != existing_patterns_end){
            if (!*played_positions){
                if (*existing_patterns * 4 + 3 > MaxPattern){
                    std::cerr << "FATAL: *existing_patterns= " << *existing_patterns << " ; MaxPattern = " << MaxPattern << std::endl;
                    exit(1);
                }
                _occurences[*existing_patterns * 4 + 1]++;
                _occurences[*existing_patterns * 4 + 2]++;

//the important part: outputting all patterns present
				std::cerr << " " << *existing_patterns * 4 + player;

            }
            ++existing_patterns;
            ++played_positions;
        }

			std::cerr << std::endl;
    }

    void print(std::ostream &out, bool verbose = false)
    {
        for (size_t i = 0; i <= MaxPattern; i++){
            if (_occurences[i] > 0 /* && _uses[i] > 0 */ ){
                if (verbose){
                    out << "----" << std::endl;
                    printHash(i, out);
                }

                out << i << ": " << _uses[i] << " / " << _occurences[i] << std::endl;
            }
        }
    }
private:
    void increment(const uint *dataBegin, const uint *dataEnd, uint32_t *array)
    {
    }

    uint32_t _occurences[MaxPattern + 1];
    uint32_t _uses[MaxPattern + 1];
};

template<typename StatsComputerType>
struct GtpController {
  GtpController(Gtp::Repl& gtp)
  {
    gtp.Register("newgame"       , this, &GtpController::CNewGame);
    gtp.Register("play"          , this, &GtpController::CPlay);
    gtp.Register("print"         , this, &GtpController::CPrint);
    gtp.Register("print_verbose" , this, &GtpController::CPrintVerbose);

    _board = new HashBoard;
  }
    
  ~GtpController()
  {
      delete _board;
  }
private:
  void CNewGame(Gtp::Io &) {
    delete _board;
    _board = new HashBoard;
  }

  void CPlay(Gtp::Io& io) {
      std::string player;
      std::string locCoords;

      io.in >> player;
      io.in >> locCoords;

    // TODO: this needs to be parsed to uint, uint
    // lets say black/red player is 0 and white/blue player is 1
    /* !!!  _board->Play(locCoords, player); !!! */

      //const uint *allBoardHashes = _board->GetAllHash();
      //size_t allBoardHashesSize = _board->GetBoardSize();

    // TODO: change this like the above code
    /* !!!  uint playHash = _board->Hash(player, locCoords); !!! */

        //uint playHash = 0;

      //_statsComp.reportPatternUse(&playHash, 1, allBoardHashes, allBoardHashesSize, _board->GetPositionsPlayed(),
			//_board->ParseColor(player));
  }

  void CPrint(Gtp::Io& io)
  {
      _statsComp.print(io.out);
  }
  
  void CPrintVerbose(Gtp::Io& io)
  {
      _statsComp.print(io.out, true);
  }

  HashBoard *_board;
  StatsComputerType _statsComp;
};

int main(int, char**)
try {
    Gtp::Repl gtp;
    GtpController<SimpleStatsComputer<(1 << 2 * 7) * 4> > gtpc(gtp);
    gtp.Run(std::cin, std::cout);
    return 0;
} catch (std::exception &e) {
    std::cerr << "exception caught: " << e.what();
    return 1;
}
