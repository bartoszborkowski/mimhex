#include <fstream>
#include <iostream>
#include <map>
#include "mm.h"

#define BUFFER_SIZE 1024

int main() {

	char buffer[BUFFER_SIZE];
	std::ifstream in("comp.txt", std::ifstream::in);

	MM::BtModel model;

	while (in.good()) {
		in.getline(buffer, BUFFER_SIZE);
		std::istringstream str(buffer);
		unsigned chosen;
		str >> chosen;
		if (!str.good())
			break;
		MM::Match& match = model.NewMatch();
		bool winnerSet = false;
		while (str.good()) {
			unsigned hash;
			str >> hash;
			if (!str.good())
				break;
			MM::Team& team = match.NewTeam();
			team.SetFeatureLevel(MM::kPatternFeature, hash);
			if (hash == chosen  && !winnerSet) {
				match.SetWinnerLastTeam();
				winnerSet = true;
			}
		}
	}
	in.close();

	model.PreprocessData();
	for (int i = 0; i < 10000; ++i)
		model.TrainFeature(MM::kPatternFeature);

	std::map<unsigned, unsigned> hash_numbers;
	std::map<unsigned, unsigned> min_hashes;

	std::ifstream mins("min_hash.txt", std::ifstream::in);
	while (mins.good()) {
		unsigned from, to;
		mins >> from >> to;
		if (!mins.good())
			break;
		min_hashes[from] = to;
	}
	mins.close();

	std::ifstream numbers("pattern_numb.txt", std::ifstream::in);
	while (numbers.good()) {
		unsigned from, to;
		numbers >> from >> to;
		if (!numbers.good())
			break;
		hash_numbers[to] = from;
	}
	numbers.close();

	std::ofstream gammas_out("pattern_gammas.txt", std::ifstream::out);
	for (std::map<unsigned, unsigned>::const_iterator it = min_hashes.begin();
			it != min_hashes.end(); ++it) {
		gammas_out << it->first << " ";
		gammas_out << model.gammas.Get(MM::kPatternFeature, hash_numbers[it->second]);
		gammas_out << std::endl;
	}
	gammas_out.close();

	return 0;
}
