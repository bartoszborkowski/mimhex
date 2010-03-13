/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#include <sstream>

#include "pattern.h"

namespace HexPatterns
{
    uint Pattern::InitialisePatterns(const char */*pattern_file*/)
    {
    // TODO: make this a 'from file' function
        rep(ii, Hex::kBoardSize + 2)
            rep(jj, Hex::kBoardSize + 2) {
                uint kk = GUARDED_POSITION(ii, jj);
                patterns[kk] = Pattern(kk, 0, ii, jj);
            }

        return 0;
    }

    Pattern::Pattern() : id(-1), template_id(-1), central_position(-1)
    {
        memset(fields_base_hashes, 0, kPatternHashMemory);
    }

    Pattern::Pattern(uint _id, uint _template_id, int row, int column) :
        id(_id), template_id(_template_id),
        central_position(GUARDED_POSITION(row, column))
    {
        memset(fields_base_hashes, 0, kPatternHashMemory);

        rep(ii, Hex::kBoardSize + 2)
            rep(jj, Hex::kBoardSize + 2) {
                uint kk = GUARDED_POSITION(ii, jj);
                rep(ff, FIELD_STATES)
                    fields_base_hashes[kk][ff] =
                        templates[template_id].GetHash(ii - row, jj - column, ff);
            }
    }

    Pattern::Pattern(const Pattern &pattern)
    {
        memcpy(this, &pattern, sizeof(Pattern));
    }

    Pattern & Pattern::operator =(const Pattern &pattern)
    {
        memcpy(this, &pattern, sizeof(Pattern));

        return *this;
    }

    std::string Pattern::ToAsciiArt() const
    {
        std::stringstream ret;

        ret << "Pattern" << std::endl
            << "\tid \t\t" << id << std::endl
            << "\ttemplate_id \t"<< template_id << std::endl
            << "\tposition \t" << central_position << std::endl;

        rep(ii, Hex::kFieldsAlignedAmount)
            if (fields_base_hashes[ii][0]) {
                ret << "\thash[" << ii
                    << "][0] \t\t" << fields_base_hashes[ii][0] << std::endl
                    << "\thash[" << ii
                    << "][1] \t\t" << fields_base_hashes[ii][1] << std::endl
                    << "\thash[" << ii
                    << "][2] \t\t" << fields_base_hashes[ii][2] << std::endl;
            }

        return ret.str();
    }
} // namespace HexPatterns
