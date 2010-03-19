/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_PATTERN_H_
#define MIMHEX_PATTERN_H_

#include "template.h"
#include "macro_definitions.h"

namespace HexPatterns
{
    class Pattern
    {
        public:
            static uint InitialisePatterns();

            Pattern();
            Pattern(uint id, uint template_id, int row, int column);
            Pattern(const Pattern &pattern);
            Pattern & operator =(const Pattern &pattern);

            uint GetId() const;
            int GetRow() const;
            int GetColumn() const;

            Hash HashChange(int position, uint player) const;

            std::string ToAsciiArt() const;

        private:
            uint id;
            uint template_id;
            int row, column;
    };

    Pattern patterns[PATTERNS_AMOUNT];
    uint __pattern_dummy = Pattern::InitialisePatterns();

    #include "pattern-inl.h"
}

#endif
