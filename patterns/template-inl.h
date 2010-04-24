/********************************************************************************
 *                              Bartosz Borkowski                               *
 *              Faculty of Mathematics, Informatics and Mechanics               *
 *                             University of Warsaw                             *
 *                               13th March 2010                                *
 ********************************************************************************/

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
    return field_hashes[relative_row + Hex::Dim::actual_size - 1]
        [relative_column + Hex::Dim::actual_size - 1][state];
}

#endif
