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

        public:
            static Pattern patterns[PATTERNS_AMOUNT];

        private:
            uint id;
            uint template_id;
            int row, column;
            static uint __pattern_dummy;
    };

    #include "pattern-inl.h"
}

#endif
