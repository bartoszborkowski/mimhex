/*******************************************************************************
 *                              Bartosz Borkowski                              *
 *              Faculty of Mathematics, Informatics and Mechanics              *
 *                              Warsaw University                              *
 *                               27th March 2010                               *
 *******************************************************************************/

#ifndef MIMHEX_HASH_BOARD_INL_13_H_
#define MIMHEX_HASH_BOARD_INL_13_H_

inline Hash HashBoard_13::GetHash(uint position) const
{
    return position_hash[position];
}

inline const Hash * HashBoard_13::GetAllHashes() const
{
    return position_hash;
}

inline void HashBoard_13::Play(uint position, uint player)
{
    rep(ii, pattern_count[position])
        position_hash[patterns_at[position][ii]] ^=
        Pattern::patterns[patterns_at[position][ii]].HashChange(position, player);

    return;
}

inline uint HashBoard_13::GetChangedPositionsAmount(uint position) const
{
    return pattern_count[position];
}

inline const uint * HashBoard_13::GetChangedPositions(uint position) const
{
    return patterns_at[position];
}

inline uint HashBoard_13::GetBoardSize() const
{
    return Hex::Dim::actual_size * Hex::Dim::actual_size;
}

#endif
