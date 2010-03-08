#include <iostream>
#include <string>
#include <fstream>
#include <boost/lexical_cast.hpp>

#include "template.cpp"
#include "pattern.cpp"

#define BUFF_SIZE 1024

using namespace std;
using namespace HexPatterns;

Hash CalculateHash(Hash h[][FIELD_STATES],
    uint f0, uint f1, uint f2, uint f3, uint f4, uint f5)
{
    return h[0][f0] ^ h[1][f1] ^ h[2][f2] ^ h[3][f3] ^ h[4][f4] ^ h[5][f5];
}

int main(int, char **)
{
    const uint max_patterns = ~0;
    Hash pattern_conv[max_patterns];
    Hash pattern_appearance[max_patterns];
    Hash min_pattern_appearance[max_patterns];
    Hash field_base_hash[6][FIELD_STATES];

    memset(pattern_conv, 0, max_patterns * sizeof(Hash));
    memset(pattern_appearance, 0, max_patterns * sizeof(Hash));
    memset(min_pattern_appearance, 0, max_patterns * sizeof(Hash));

    rep(i, FIELD_STATES) {
        field_base_hash[0][i] = templates[0].GetHash(-16, i);
        field_base_hash[1][i] = templates[0].GetHash(-15, i);
        field_base_hash[2][i] = templates[0].GetHash(-1, i);
        field_base_hash[3][i] = templates[0].GetHash(1, i);
        field_base_hash[4][i] = templates[0].GetHash(15, i);
        field_base_hash[5][i] = templates[0].GetHash(16, i);
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
	char buffer[BUFF_SIZE];
	string split;
	string part;
	size_t pos, end;

	while (in.good()) {
		in.getline(buffer, BUFF_SIZE);
		split = buffer;
		pos = 0;

		while (pos !=string::npos) {
			end = split.find_first_of(' ', pos);
			part = split.substr(pos, end == string::npos ? end : end - pos);

			try {
				uint p = boost::lexical_cast<uint>(part);

				min_pattern_appearance[pattern_conv[p]] = 1;
				pattern_appearance[p] = 1;

			} catch (boost::bad_lexical_cast &) {
			}

			if (end == string::npos)
				break;
			else
				pos = end + 1;
		}
	}

	for (uint i = 0; i < max_patterns; ++i)
		if (pattern_appearance[i])
			out0 << i << " " << pattern_conv[i] << endl; // pattern -> minPattern

	for (uint i = 0, j = 0; i < max_patterns; ++i)
		if (min_pattern_appearance[i]) {
			out2 << j << " " << i << endl; // ordinal number -> minPattern
			min_pattern_appearance[i] = j++; //minPattern -> ordinal number
		}

	in.close();
	in.open("pattern_teams.txt", fstream::in);

	while (in.good()) {
		in.getline(buffer, BUFF_SIZE);
		split = buffer;
		pos = 0;

		while (pos !=string::npos) {
			end = split.find_first_of(' ', pos);
			part = split.substr(pos, end == string::npos ? end : end - pos);

			try {
				uint p = boost::lexical_cast<uint>(part);

				out1 << min_pattern_appearance[pattern_conv[p]];
			} catch (boost::bad_lexical_cast &) {
			}

			if (end == string::npos)
				break;
			else {
				out1 << " ";
				pos = end + 1;
			}
		}

		out1 << endl;

	}

	in.close();
	out0.close();
	out1.close();
	out2.close();

}
