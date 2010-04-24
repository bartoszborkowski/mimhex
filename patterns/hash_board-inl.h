/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                             9th March 2010                                  *
 *******************************************************************************/

#ifndef MIMHEX_HASH_BOARD_INL_H_
#define MIMHEX_HASH_BOARD_INL_H_

inline Hash HashBoard::GetHash(uint position) const
{
    return position_hash[position];
}

inline const Hash * HashBoard::GetAllHashes() const
{
    return position_hash;
}

inline void HashBoard::Play(uint position, uint player)
{
    rep(ii, pattern_count[position])
        position_hash[patterns_at[position][ii]] ^=
        Pattern::patterns[patterns_at[position][ii]].HashChange(position, player);

    return;
}

inline uint HashBoard::GetChangedPositionsAmount(uint position) const
{
    return pattern_count[position];
}

inline const uint * HashBoard::GetChangedPositions(uint position) const
{
    return patterns_at[position];
}

inline uint HashBoard::GetBoardSize() const
{
    return Hex::Dim::actual_field_count;
}

#endif
