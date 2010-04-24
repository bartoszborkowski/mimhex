/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_PATTERN_INL_H_
#define MIMHEX_PATTERN_INL_H_

inline uint Pattern::GetId() const
{
    return id;
}

inline int Pattern::GetRow() const
{
    return row;
}

inline int Pattern::GetColumn() const
{
    return column;
}

inline Hash Pattern::HashChange(int position, uint player) const
{
    return Template::templates[template_id].
        GetHash((position >> 4) - row, (position & 15) - column, player);
}

#endif
