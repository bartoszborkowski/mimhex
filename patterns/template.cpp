/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <boost/lexical_cast.hpp>

#include "template.h"

#define BUFFER_SIZE 512
#define COMMENT_LINE '#'

#define ROW         0
#define COLUMN      1

namespace HexPatterns
{
    template<class T> T ParseToken(const std::string &input, const char separator,
        size_t &pos)
    {
        size_t end = input.find_first_of(separator, pos);
        std::string part =
            input.substr(pos, end == std::string::npos ? end : end - pos);
        pos = end == std::string::npos ? end : end + 1;

        try {
            return boost::lexical_cast<T>(part);
        } catch (boost::bad_lexical_cast &) {
            std::cerr << "Error: " << part << " is of invalid type" << std::endl;
            exit(1);
        }
    }

    void ParseString(const std::string &input, const char separator,
        uint &id, uint &base_hash, uint &size,
        int relative_positions[][DIMENSIONS],
        Hash fields_base_hashes[][FIELD_STATES])
    /*
    * TODO: move this to some util-like file
    */
    {
        size_t pos = 0;

        id = ParseToken<uint>(input, separator, pos);
        base_hash = ParseToken<uint>(input, separator, pos);
        size = ParseToken<uint>(input, separator, pos);

        if (size > Hex::kFieldsAmount) {
            std::cerr << "Error: Template " << id << " size out of bounds."
                        << std::endl;
            exit(1);
        }

        rep(ii, size) {
            rep(jj, DIMENSIONS)
                relative_positions[ii][jj] =
                    ParseToken<int>(input, separator, pos);
            rep(jj, FIELD_STATES - 1)
                fields_base_hashes[ii][jj] =
                    ParseToken<uint>(input, separator, pos);
        }

        return;
    }

    uint Template::InitialiseTemplates(const char *template_file)
    {
        std::ifstream ifs(template_file, std::ifstream::in);
        char buffer[BUFFER_SIZE];

        int relative_positions[Hex::kFieldsAmount][DIMENSIONS];
        Hash fields_base_hashes[Hex::kFieldsAmount][FIELD_STATES];

        size_t rp_size = Hex::kFieldsAmount * DIMENSIONS * sizeof(int);
        size_t fbh_size = Hex::kFieldsAmount * FIELD_STATES * sizeof(Hash);

        uint id, base_hash, size;

        if (!ifs.good()) {
            std::cerr << "Error: could not open file \'" << template_file
                        << "\'" << std::endl;
            exit(1);
        }

        while (ifs.good()) {
            memset(relative_positions, 0, rp_size);
            memset(fields_base_hashes, 0, fbh_size);

            ifs.getline(buffer, BUFFER_SIZE);
            if (buffer[0] != COMMENT_LINE) {
                ParseString(buffer, ' ', id, base_hash, size,
                    relative_positions, fields_base_hashes);

                templates[id] =
                    Template(id, base_hash, size,
                        relative_positions, fields_base_hashes);
            }
        }

        ifs.close();

        return 0;
    }

    Template::Template() : id(-1), base_hash(-1), size(0)
    {
        memset(fields_base_hashes, 0, kTemplateHashMemory);
    }

    Template::Template(uint _id, Hash _base_hash, uint _size,
        int relative_positions[][DIMENSIONS],
        Hash _fields_base_hashes[][FIELD_STATES]) :
        id(_id), base_hash(_base_hash), size(_size)
    {
        memset(fields_base_hashes, 0, kTemplateHashMemory);

        rep(ii, size)
            memcpy(
                fields_base_hashes
                [relative_positions[ii][ROW] + Hex::kBoardSizeAligned - 1]
                [relative_positions[ii][COLUMN] + Hex::kBoardSizeAligned - 1],
                _fields_base_hashes[ii], FIELD_STATES_SIZE);
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

    Template Template::GetMirrorX() const
    {
        Template ret = Template(*this);
        Hash aux[2 * Hex::kBoardSizeAligned - 1][FIELD_STATES];
        size_t row_size =
            (2 * Hex::kBoardSizeAligned - 1) * FIELD_STATES * sizeof(Hash);
        uint offset = 2 * (Hex::kBoardSizeAligned - 1);

        rep(ii, Hex::kBoardSizeAligned - 1) {
            memcpy(aux, ret.fields_base_hashes[ii], row_size);
            memcpy(ret.fields_base_hashes[ii],
                ret.fields_base_hashes[offset - ii], row_size);
            memcpy(ret.fields_base_hashes[offset - ii], aux, row_size);
        }

        return ret;
    }

    Template Template::GetMirrorY() const
    {
        Template ret = Template(*this);
        Hash aux[FIELD_STATES];
        size_t field_size = FIELD_STATES * sizeof(Hash);
        uint offset = 2 * (Hex::kBoardSizeAligned - 1);

        rep(ii, 2 * Hex::kBoardSizeAligned - 1)
            rep(jj, Hex::kBoardSizeAligned - 1) {
                memcpy(aux, ret.fields_base_hashes[ii][jj], field_size);
                memcpy(ret.fields_base_hashes[ii][jj],
                    ret.fields_base_hashes[ii][offset - jj], field_size);
                memcpy(ret.fields_base_hashes[ii][offset - jj], aux, field_size);
            }

        return ret;
    }

    Template Template::GetMirrorXY() const
    {
        return GetMirrorY().GetMirrorX();
    }

    std::string Template::ToAsciiArt() const
    {
        std::stringstream ret;

        ret << "Template" << std::endl
            << "\tid \t\t" << id << std::endl
            << "\tbase_hash \t"<< base_hash << std::endl
            << "\tsize \t\t" << size << std::endl << std::endl;

        rep(ii, FIELD_STATES) {
            ret << "STATE " << ii << std::endl;
            rep(jj, 2 * Hex::kBoardSizeAligned - 1) {
                rep(kk, 2 * Hex::kBoardSizeAligned - 1)
                    if (fields_base_hashes[jj][kk][ii])
                        ret << "\thash["
                            << static_cast<int>(jj - Hex::kBoardSizeAligned + 1)
                            << "]["
                            << static_cast<int>(kk - Hex::kBoardSizeAligned + 1)
                            << "] \t\t"
                            << fields_base_hashes[jj][kk][ii] << std::endl;
            }
            ret << std::endl;
        }

        return ret.str();
    }
} //namespace HexPatterns
