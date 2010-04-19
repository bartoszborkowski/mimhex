/*******************************************************************************
 *                   Bartosz Borkowski, Michal Albrycht                        *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                            Warsaw University                                *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include <boost/lexical_cast.hpp>

#include "board.cpp"
#include "template.cpp"
#include "pattern.cpp"
#include "hash_board.cpp"
#include "sampler.cpp"
#include "pattern_data.cpp"

#define BUFF_SIZE 1024


using namespace std;
using namespace HexPatterns;

Hash CalculateHash(Hash h[][FIELD_STATES],
    uint f0, uint f1, uint f2, uint f3, uint f4, uint f5)
{
    return h[0][f0] ^ h[1][f1] ^ h[2][f2] ^ h[3][f3] ^ h[4][f4] ^ h[5][f5];
}


int main(int, char **){
    map<Hash, Hash> pattern_conv;
    map<Hash, bool> pattern_appearance;
    map<Hash, bool> min_pattern_appearance;
    map<Hash, uint> ordinal_no;
    Hash field_base_hash[6][FIELD_STATES];

    pattern_conv.clear();
    pattern_appearance.clear();
    min_pattern_appearance.clear();
    ordinal_no.clear();

    rep(i, FIELD_STATES) {
        field_base_hash[0][i] = templates[0].GetHash(-1, 0, i);
        field_base_hash[1][i] = templates[0].GetHash(-1, 1, i);
        field_base_hash[2][i] = templates[0].GetHash(0, 1, i);
        field_base_hash[3][i] = templates[0].GetHash(1, 0, i);
        field_base_hash[4][i] = templates[0].GetHash(1, -1, i);
        field_base_hash[5][i] = templates[0].GetHash(0, -1, i);
    }

    rep(f0, FIELD_STATES) // for every possible state of field 0
    rep(f1, FIELD_STATES) // for every possible state of field 1
    rep(f2, FIELD_STATES) // for every possible state of field 2
    rep(f3, FIELD_STATES) // for every possible state of field 3
    rep(f4, FIELD_STATES) // for every possible state 0f field 4
    rep(f5, FIELD_STATES) // for every possible state of field 5
    {
        uint hash1 = CalculateHash(field_base_hash, f0, f1, f2, f3, f4, f5); // normal hash
        uint hash2 = CalculateHash(field_base_hash, f4, f3, f2, f1, f0, f5); // x axis mirror
        uint hash3 = CalculateHash(field_base_hash, f1, f0, f5, f4, f3, f2); // y axis mirror
        uint hash4 = CalculateHash(field_base_hash, f3, f4, f5, f0, f1, f2); // x & y axis mirror

        uint min_hash = min(min(hash1, hash2), min(hash3, hash4));

        pattern_conv[hash1] = pattern_conv[hash2] =
            pattern_conv[hash3] = pattern_conv[hash4] = min_hash;
    }

	ifstream in("pattern_teams.txt", ifstream::in);
	ofstream out0("min_hash.txt", ios_base::out);
	ofstream out1("comp.txt", ios_base::out);
	ofstream out2("pattern_numb.txt", ios_base::out);

	uint line_no = 0;
	uint ordinal=0;
	string line;

	while (!in.eof()){
		getline(in, line);
		istringstream line_stream;
		line_stream.str(line);
		uint pattern;
		while (line_stream >> pattern){
			uint minHash = pattern_conv[pattern];
			min_pattern_appearance[minHash] = true;
			pattern_appearance[pattern] = true;
			if (!ordinal_no[minHash]) //assing orindal number if hasn't one
				ordinal_no[minHash] = ++ordinal;
			out1 << ordinal_no[minHash];
			if (!line_stream.eof())
				out1 << " ";
		}
		out1 << endl;
		line_no ++;
	}

	// iterate over pattern_appearance and and write pattern -> min_patterns
    for (map<Hash, bool>::iterator it_pc = pattern_appearance.begin(); it_pc != pattern_appearance.end(); ++it_pc){
        uint pattern = it_pc->first;
		out0 << pattern << " " << pattern_conv[pattern] << endl; // pattern -> minPattern
    }

    // iterater over min_pattern_appearanve and for each assign ordinal number
    for (map<Hash, bool>::iterator it_mpa = min_pattern_appearance.begin(); it_mpa != min_pattern_appearance.end(); ++it_mpa){
        uint minHash = it_mpa->first;
		out2 << ordinal_no[minHash] << " " << minHash << endl; // ordinal number -> minPattern
    }

	in.close();
	out0.close();
	out1.close();
	out2.close();

}
