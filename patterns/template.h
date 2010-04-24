/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

#ifndef MIMHEX_TEMPLATE_H_
#define MIMHEX_TEMPLATE_H_

#include <string>
#include <vector>

#include "macro_definitions.h"

namespace HexPatterns
{
    class Template
    {
    /* A template represents a shape on the board.                              *
     * Each field that belongs to the shape has four different hash values:     *
     * - empty hash used when the field is empty (always set to 0),             *
     * - player one hash used when the field is occupied by the first player,   *
     * - player two hash used when the field is occupied by the second player,  *
     * - guardian hash used when the field is out-of-bounds.                    *
     * A field that does not belong to the template has all hashes set to 0.    *
     * Fields' positions are kept relative to the "centre" of the template.     *
     * As of now a template has one centre.                                     *
     * Fields' hashes are kept in the following format:                         *
     * [row_relative_to_center + Hex::kBoardSizeAligned - 1]                    *
     * [column_relative_to_center + Hex::kBoardSizeAligned - 1][field_state]    *
     */
        public:
            static uint InitialiseTemplates();

            Template();
            Template(uint id, Hash base_hash, uint size,
                int relative_positions[][DIMENSIONS],
                Hash field_hashes[][FIELD_STATES]);
            Template(const Template &t);
            Template & operator =(const Template &t);

            uint GetId() const;
            Hash GetBaseHash() const;
            uint GetSize() const;
            Hash GetHash(int relative_row, int relative_column, uint state) const;

            Template GetMirrorX() const;
            Template GetMirrorY() const;
            Template GetMirrorXY() const;

            std::vector<Hash> GetAllHashes() const;
            std::string ToAsciiArt() const;

        public:
            static Template templates[TEMPLATES_AMOUNT];

        private:
            uint id;
            Hash base_hash;
            uint size;
            Hash field_hashes[2 * Hex::Dim::actual_size - 1]
                [2 * Hex::Dim::actual_size - 1][FIELD_STATES];
            static uint __template_dummy;
    };

    #include "template-inl.h"
} // namespace HexPatterns

#endif
