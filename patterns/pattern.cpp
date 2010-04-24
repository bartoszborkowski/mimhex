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
    uint Pattern::InitialisePatterns()
    {
        rep(ii, Hex::Dim::actual_size)
            rep(jj, Hex::Dim::actual_size) {
                uint kk = ii * Hex::Dim::actual_size + jj;
                patterns[kk] = Pattern(kk, 0, ii, jj);
            }

        return 0;
    }

    Pattern::Pattern() : id(-1), template_id(-1), row(-1), column(-1)
    {
    }

    Pattern::Pattern(uint _id, uint _template_id, int _row, int _column) :
        id(_id), template_id(_template_id), row(_row), column(_column)
    {
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
            << "\trow \t" << row << std::endl
            << "\tcolumn \t" << column << std::endl;

        return ret.str();
    }

    Pattern Pattern::patterns[PATTERNS_AMOUNT];
    uint Pattern::__pattern_dummy = Pattern::InitialisePatterns();

} // namespace HexPatterns
