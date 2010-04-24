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
    void FillArrays(uint i, int row, int col, Hash h0, Hash h1, Hash h2,
        Hash h3, int pos[][DIMENSIONS], Hash h[][FIELD_STATES])
    {
        pos[i][ROW] = row;
        pos[i][COLUMN] = col;

        h[i][PLAYER_0_STATE] = h0;
        h[i][PLAYER_1_STATE] = h1;
        h[i][GUARDIAN_STATE] = h2;
        h[i][EMPTY_STATE] = h3;
    }

    uint Template::InitialiseTemplates()
    {
        int positions[6][DIMENSIONS];
        Hash hashes[6][FIELD_STATES];

        FillArrays(0, -1, 0, 1858371402, 637933676, 1993303069, 0, positions, hashes);
        FillArrays(1, -1, 1, 2029330270, 1495175269, 1134523642, 0, positions, hashes);
        FillArrays(2, 0, -1, 608453654, 2081962756, 792820039, 0, positions, hashes);
        FillArrays(3, 0, 1, 290675495, 1419729510, 1768091954, 0, positions, hashes);
        FillArrays(4, 1, -1, 336599391, 1251852077, 927732484, 0, positions, hashes);
        FillArrays(5, 1, 0, 1933830382, 260111653, 2066513445, 0, positions, hashes);

        templates[0] = Template(0, 0, 6, positions, hashes);

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
                [relative_positions[ii][ROW] + Hex::Dim::actual_size - 1]
                [relative_positions[ii][COLUMN] + Hex::Dim::actual_size - 1],
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
        Hash aux[2 * Hex::Dim::actual_size - 1][FIELD_STATES];
        size_t row_size =
            (2 * Hex::Dim::actual_size - 1) * FIELD_STATES * sizeof(Hash);
        uint offset = 2 * (Hex::Dim::actual_size - 1);

        rep(ii, Hex::Dim::actual_size - 1) {
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
        uint offset = 2 * (Hex::Dim::actual_size - 1);

        rep(ii, 2 * Hex::Dim::actual_size - 1)
            rep(jj, Hex::Dim::actual_size - 1) {
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
        rep(ii, 2 * Hex::Dim::actual_size - 1)
            rep(jj, 2 * Hex::Dim::actual_size - 1)
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
            rep(jj, 2 * Hex::Dim::actual_size - 1) {
                rep(kk, 2 * Hex::Dim::actual_size - 1)
                    if (field_hashes[jj][kk][ii])
                        ret << "\thash["
                            << static_cast<int>(jj - Hex::Dim::actual_size + 1)
                            << "]["
                            << static_cast<int>(kk - Hex::Dim::actual_size + 1)
                            << "] \t\t"
                            << field_hashes[jj][kk][ii] << std::endl;
            }
            ret << std::endl;
        }

        return ret.str();
    }

    Template Template::templates[TEMPLATES_AMOUNT];
    uint Template::__template_dummy = Template::InitialiseTemplates();

} //namespace HexPatterns
