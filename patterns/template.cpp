/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

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
        Hash field_hashes[][FIELD_STATES])
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
                field_hashes[ii][jj] =
                    ParseToken<uint>(input, separator, pos);
        }

        return;
    }

    uint Template::InitialiseTemplates(const char *template_file)
    {
        std::ifstream ifs(template_file, std::ifstream::in);
        char buffer[BUFFER_SIZE];

        int relative_positions[Hex::kFieldsAmount][DIMENSIONS];
        Hash field_hashes[Hex::kFieldsAmount][FIELD_STATES];

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
            memset(field_hashes, 0, fbh_size);

            ifs.getline(buffer, BUFFER_SIZE);
            if (buffer[0] != COMMENT_LINE) {
                ParseString(buffer, ' ', id, base_hash, size,
                    relative_positions, field_hashes);

                templates[id] =
                    Template(id, base_hash, size,
                        relative_positions, field_hashes);
            }
        }

        ifs.close();

        return 0;
    }

    Template::Template() : id(-1), base_hash(-1), size(0)
    {
        memset(field_hashes, 0, kTemplateHashMemory);
    }

    Template::Template(uint _id, Hash _base_hash, uint _size,
        int relative_positions[][DIMENSIONS],
        Hash _field_hashes[][FIELD_STATES]) :
        id(_id), base_hash(_base_hash), size(_size)
    {
        memset(field_hashes, 0, kTemplateHashMemory);

        rep(ii, size)
            memcpy(
                field_hashes
                [relative_positions[ii][ROW] + Hex::kBoardSizeAligned - 1]
                [relative_positions[ii][COLUMN] + Hex::kBoardSizeAligned - 1],
                _field_hashes[ii], FIELD_STATES_SIZE);
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
            memcpy(aux, ret.field_hashes[ii], row_size);
            memcpy(ret.field_hashes[ii],
                ret.field_hashes[offset - ii], row_size);
            memcpy(ret.field_hashes[offset - ii], aux, row_size);
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
                memcpy(aux, ret.field_hashes[ii][jj], field_size);
                memcpy(ret.field_hashes[ii][jj],
                    ret.field_hashes[ii][offset - jj], field_size);
                memcpy(ret.field_hashes[ii][offset - jj], aux, field_size);
            }

        return ret;
    }

    Template Template::GetMirrorXY() const
    {
        return GetMirrorY().GetMirrorX();
    }

    std::vector<Hash> Template::GetAllHashes() const
    {
        ulong len = 1;
        Hash **hashes;
        std::vector<Hash> ret;

        rep(ii, size)
            len *= FIELD_STATES;
        /* len = FIELD_STATES ^ size                                            */

        hashes = new Hash *[size];
        rep(ii, size)
            hashes[ii] = new Hash[FIELD_STATES];

        uint kk = 0;
        rep(ii, 2 * Hex::kBoardSizeAligned - 1)
            rep(jj, 2 * Hex::kBoardSizeAligned - 1)
                if (field_hashes[ii][jj][0])
                    memcpy(hashes[kk++], field_hashes[ii][jj],
                        FIELD_STATES * sizeof(Hash));
        /* hashes array containes non-zero hashes from the template.            */

        rep(ii, len)
            ret.push_back(0);

        rep(ii, len) {
            rep(jj, size)
                ret[ii] ^= hashes[jj][ii >> (jj * 2) & 3];
        }

        rep(ii, size)
            delete [] hashes[ii];
        delete hashes;

        return ret;
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
                    if (field_hashes[jj][kk][ii])
                        ret << "\thash["
                            << static_cast<int>(jj - Hex::kBoardSizeAligned + 1)
                            << "]["
                            << static_cast<int>(kk - Hex::kBoardSizeAligned + 1)
                            << "] \t\t"
                            << field_hashes[jj][kk][ii] << std::endl;
            }
            ret << std::endl;
        }

        return ret.str();
    }
} //namespace HexPatterns
