#include <sstream>

#include "pattern.h"

namespace HexPatterns
{
    uint Pattern::InitialisePatterns(const char */*pattern_file*/)
    {
    // TODO: make this a 'from file' function
        rep(ii, Hex::kBoardSize)
            rep(jj, Hex::kBoardSize) {
                uint kk = NORMALISE_POSITION(ii, jj);
                patterns[kk] = Pattern(kk, 0, kk);
            }

        return 0;
    }

    Pattern::Pattern() : id(-1), template_id(-1), central_position(-1) {}

    Pattern::Pattern(uint _id, uint _template_id, int _position) : id(_id),
        template_id(_template_id), central_position(_position)
    {
        memset(fields_base_hash, 0, kHashMemory);

        rep(ii, Hex::kBoardSize)
            rep(jj, Hex::kBoardSize) {
                uint kk = NORMALISE_POSITION(ii, jj);
                rep(ff, FIELD_STATES)
                    fields_base_hash[kk][ff] =
                        templates[template_id].GetHash(kk - central_position, ff);
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
            if (fields_base_hash[ii][0]) {
                ret << "\thash[" << ii
                    << "][0] \t\t" << fields_base_hash[ii][0] << std::endl
                    << "\thash[" << ii
                    << "][1] \t\t" << fields_base_hash[ii][1] << std::endl
                    << "\thash[" << ii
                    << "][2] \t\t" << fields_base_hash[ii][2] << std::endl;
            }

        return ret.str();
    }
} // namespace HexPatterns
