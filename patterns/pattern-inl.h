/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_PATTERN_INL_H_
#define MIMHEX_PATTERN_INL_H_

inline uint Pattern::GetPosition() const
{
    return central_position;
}

inline uint Pattern::HashChange(uint position, uint player) const
{
    return fields_base_hashes[position][player];
}

#endif
