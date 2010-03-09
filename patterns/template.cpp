#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "template.h"

#define BUFFER_SIZE 512
#define COMMENT_LINE '#'

namespace HexPatterns
{
    void SplitString(const std::string &input, const char separator,
        std::vector<int> &output)
    /*
    * TODO: move this to some util-like file
    */
    {
        size_t pos = 0, end = 0;
        std::string part;

        while (end != std::string::npos) {
            end = input.find_first_of(separator, pos);
            part = input.substr(pos, end == std::string::npos ? end : end - pos);
            pos = end + 1;

            try {
            output.push_back(boost::lexical_cast<int>(part));
            } catch (boost::bad_lexical_cast &) {
                std::cerr << "Error: " << part << " is not an integer" << std::endl;
                exit(1);
            }
        }

        return;
    }

    uint Template::InitialiseTemplates(const char *template_file)
    {
        std::ifstream ifs(template_file, std::ifstream::in);
        char buffer[BUFFER_SIZE];
        Hash fields_hash[Hex::kFieldsAlignedAmount][FIELD_STATES];
        std::vector<int> line;

        int min_relative_position;
        int fields_relative_positions[Hex::kFieldsAlignedAmount];
        Hash auxiliary_hash[Hex::kFieldsAlignedAmount][FIELD_STATES];

        if (!ifs.good()) {
            std::cerr << "Error: could not open file: " << template_file << std::endl;
            exit(1);
        }

        while (ifs.good()) {
            // TODO: make the following into a function... or something
            memset(fields_hash, 0, kHashMemory);
            memset(auxiliary_hash, 0, kHashMemory);

            min_relative_position = Hex::kFieldsAlignedAmount;
            line.clear();

            ifs.getline(buffer, BUFFER_SIZE);
            if (buffer[0] != COMMENT_LINE) {
                ASSERT(static_cast<uint>(line[2]) <= Hex::kFieldsAlignedAmount);

                SplitString(buffer, ' ', line);

                rep(ii, static_cast<uint>(line[2])) {
                    uint tmp = 3 + ii * 4;
                    fields_relative_positions[ii] = line[tmp];
                    auxiliary_hash[ii][0] = line[tmp + 1];
                    auxiliary_hash[ii][1] = line[tmp + 2];
                    auxiliary_hash[ii][2] = line[tmp + 3];
                    auxiliary_hash[ii][3] = 0;

                    if (min_relative_position > fields_relative_positions[ii])
                        min_relative_position = fields_relative_positions[ii];
                }

                rep(ii, static_cast<uint>(line[2])) {
                    memcpy(fields_hash[fields_relative_positions[ii] - min_relative_position],
                        auxiliary_hash[ii], FIELD_STATES * sizeof(Hash));
                }

                templates[line[0]] =
                    Template(line[0], line[1], line[2], min_relative_position, fields_hash);
            }
        }

        ifs.close();

        return 0;
    }

    Template::Template() : id(-1), base_hash(-1), size(0), min_relative(0)
    {
        memset(fields_base_hash, 0, kHashMemory);
    }

    Template::Template(uint _id, Hash _base_hash, uint _size, int _min_relative,
        Hash _fields_base_hash[][FIELD_STATES]) : id(_id), base_hash(_base_hash),
        size(_size), min_relative(_min_relative)
    {
        memcpy(fields_base_hash, _fields_base_hash, kHashMemory);
    }

    Template::Template(const Template &t)
    {
        memcpy(this, &t, sizeof(Template));
    }

    Template & Template::operator =(const Template &t)
    {
        memcpy(this, &t, sizeof(Template));
        return *this;
    }

    std::string Template::ToAsciiArt() const
    {
        std::stringstream ret;

        ret << "Template" << std::endl
            << "\tid \t\t" << id << std::endl
            << "\tbase_hash \t"<< base_hash << std::endl
            << "\tsize \t\t" << size << std::endl;

        rep(ii, Hex::kFieldsAlignedAmount)
            if (fields_base_hash[ii][0]) {
                ret << "\thash[" << static_cast<int>(ii + min_relative)
                    << "][0] \t\t" << fields_base_hash[ii][0] << std::endl
                    << "\thash[" << static_cast<int>(ii + min_relative)
                    << "][1] \t\t" << fields_base_hash[ii][1] << std::endl
                    << "\thash[" << static_cast<int>(ii + min_relative)
                    << "][2] \t\t" << fields_base_hash[ii][2] << std::endl;
            }

        return ret.str();
    }
} //namespace HexPatterns
