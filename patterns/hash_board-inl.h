#ifndef MIMHEX_HASH_BOARD_INL_H_
#define MIMHEX_HASH_BOARD_INL_H_

inline Hash HashBoard::GetHash(const Hex::Location &loc) const
{
    return GetHash(loc.GetPos());
}

inline Hash HashBoard::GetHash(uint position) const
{
    return position_hash[position];
}

inline Hash HashBoard::GetHash(const Hex::Move &move) const
{
    return GetHash(move.GetLocation().GetPos());
}

inline const Hash * HashBoard::GetAllHash() const
{
    return position_hash;
}

inline void HashBoard::Play(const Hex::Location &loc, const Hex::Player &player)
{
    return Play(loc.GetPos(), player);
}

inline void HashBoard::Play(uint position, const Hex::Player &player)
{
    rep(ii, pattern_count[position])
        position_hash[patterns[patterns_at[position][ii]].GetPosition()] ^=
        patterns[patterns_at[position][ii]].HashChange(position, player.GetVal());

    return;
}

inline void HashBoard::Play(const Hex::Move &move)
{
    return Play(move.GetLocation().GetPos(), move.GetPlayer());
}

#endif
