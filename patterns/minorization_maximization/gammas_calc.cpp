
#include <fstream>
#include <iostream>
#include <cmath>
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
		MM::Match& match = model.NewMatch();
		MM::Team& team = match.NewTeam();
		team.SetFeatureLevel(MM::kPatternFeature, chosen);
		match.SetWinnerLastTeam();
		while (str.good()) {
			unsigned hash;
			str >> hash;
			if (!str.good())
				break;
			MM::Team& team = match.NewTeam();
			team.SetFeatureLevel(MM::kPatternFeature, hash);
		}
	}

	model.PreprocessData();
	model.DoGradientUpdate(1000000);	// FIXME what function should be used?

	std::ifstream numbers("pattern_numb.txt", std::ifstream::in);
	std::ofstream gammas_out("pattern_gammas.txt", std::ifstream::out);
	while (numbers.good()) {
		unsigned from, to;
		numbers >> from >> to;
		if (!numbers.good())
			break;
		gammas_out << to << " " << model.gammas.Get(MM::kPatternFeature, from) << std::endl;
	}

	in.close();
	numbers.close();
	gammas_out.close();

	return 0;
}
