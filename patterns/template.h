/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_TEMPLATE_H_
#define MIMHEX_TEMPLATE_H_

#include <string>

#include "conditional_assert.h"
#include "macro_definitions.h"

namespace HexPatterns
{
    class Template
    {
        public:
            static uint InitialiseTemplates(const char *template_file = TEMPLATES_FILE);

            Template();
            Template(uint id, Hash base_hash, uint size, int min_relative,
                Hash fields_base_hash[][FIELD_STATES]);
            Template(const Template &t);
            Template & operator =(const Template &t);

            uint GetId() const;
            Hash GetBaseHash() const;
            uint GetSize() const;
            Hash GetHash(int relative_position, uint player) const;

            std::string ToAsciiArt() const;

        private:
            uint id;
            Hash base_hash;
            uint size;
            int min_relative;
            Hash fields_base_hash[Hex::kFieldsAlignedAmount][FIELD_STATES];
    };

    Template templates[TEMPLATES_AMOUNT];
    uint __template_dummy = Template::InitialiseTemplates();

    #include "template-inl.h"
} // namespace HexPatterns

#endif
