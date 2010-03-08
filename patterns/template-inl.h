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

inline Hash Template::GetHash(int relative_position, uint player) const
{
    int position = relative_position - min_relative;

    if (position < 0 || static_cast<uint>(position) > kFieldsAlignedAmount)
        return 0;
    else
        return fields_base_hash[position][player];
}

#endif
