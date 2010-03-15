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
            static uint InitialisePatterns(const char *pattern_file = PATTERNS_FILE);

            Pattern();
            Pattern(uint id, uint template_id, int position);
            Pattern(const Pattern &pattern);
            Pattern & operator =(const Pattern &pattern);

            uint GetPosition() const;
            Hash HashChange(uint position, uint player) const;

            std::string ToAsciiArt() const;

        private:
            uint id;
            uint template_id;
            uint central_position;
            Hash fields_base_hash[Hex::kFieldsAlignedAmount][FIELD_STATES];
    };

    Pattern patterns[PATTERNS_AMOUNT];
    uint __pattern_dummy = Pattern::InitialisePatterns();

    #include "pattern-inl.h"
}

#endif
