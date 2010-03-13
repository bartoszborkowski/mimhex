/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_TEMPLATE_INL_H_
#define MIMHEX_TEMPLATE_INL_H_

inline uint Template::GetId() const
{
    return id;
}

inline Hash Template::GetBaseHash() const
{
    return base_hash;
}

inline uint Template::GetSize() const
{
    return size;
}

inline Hash Template::GetHash(int relative_row, int relative_column, uint state) const
{
    return fields_base_hashes[relative_row + Hex::kBoardSizeAligned - 1]
        [relative_column + Hex::kBoardSizeAligned - 1][state];
}

#endif
